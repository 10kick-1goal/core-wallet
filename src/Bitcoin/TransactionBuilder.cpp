// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TransactionBuilder.h"
#include "TransactionSigner.h"

#include <algorithm>
#include <cassert>

namespace TW::Bitcoin {

/// Estimate encoded size by invoking sign(sizeOnly), get actual size
int64_t estimateSegwitFee(FeeCalculator& feeCalculator, const TransactionPlan& plan, int outputSize, const Bitcoin::Proto::SigningInput& input) {
    // duplicate input, with the current plan
    auto inputWithPlan = std::move(input);
    *inputWithPlan.mutable_plan() = plan.proto();

    auto signer = TransactionSigner<Transaction, TransactionBuilder>(std::move(inputWithPlan), true);
    auto result = signer.sign();
    if (!result) {
        // signing failed; return default estimate
        return feeCalculator.calculate(plan.utxos.size(), outputSize, input.byte_fee());
    }

    // Obtain the encoded size
    auto transaction = result.payload();
    Data dataNonSegwit;
    transaction.encode(dataNonSegwit, Transaction::SegwitFormatMode::NonSegwit);
    int64_t sizeNonSegwit = dataNonSegwit.size();
    uint64_t vSize = 0;
    // Check if there is segwit
    if (!transaction.hasWitness()) {
        // no segwit, virtual size is defined as non-segwit size
        vSize = sizeNonSegwit;
    } else {
        Data dataWitness;
        transaction.encodeWitness(dataWitness);
        int64_t witnessSize = 2 + dataWitness.size();
        // compute virtual size:  (smaller) non-segwit + 1/4 of the diff (witness-only)
        // (in other way: 3/4 of (smaller) non-segwit + 1/4 of segwit size)
        vSize = sizeNonSegwit + witnessSize/4 + (witnessSize % 4 != 0);
    }
    uint64_t fee = input.byte_fee() * vSize;

    return fee;
}

TransactionPlan TransactionBuilder::plan(const Bitcoin::Proto::SigningInput& input) {
    auto plan = TransactionPlan();
    plan.amount = input.amount();

    auto output_size = 2;
    auto& feeCalculator = getFeeCalculator(static_cast<TWCoinType>(input.coin_type()));
    auto unspentSelector = UnspentSelector(feeCalculator);

    // select UTXOs
    if (!input.use_max_amount()) {
        output_size = 2; // output + change
        plan.utxos = unspentSelector.select(input.utxo(), plan.amount, input.byte_fee(), output_size);
    } else {
        output_size = 1; // no change
        plan.utxos = unspentSelector.selectMaxAmount(input.utxo(), input.byte_fee());
    }
    // Note: if utxos.size() == 0, all fields will be computed to 0
    plan.availableAmount = UnspentSelector::sum(plan.utxos);

    // Compute fee.
    // must preliminary set change so that there is a second output
    if (!input.use_max_amount()) {
        plan.amount = input.amount();
        plan.fee = 0;
        plan.change = plan.availableAmount - plan.amount;
    } else {
        plan.amount = plan.availableAmount;
        plan.fee = 0;
        plan.change = 0;
    }
    plan.fee = estimateSegwitFee(feeCalculator, plan, output_size, input);
    // If fee is larger then availableAmount (can happen in special maxAmount case), we reduce it (and hope it will go through)
    plan.fee = std::min(plan.availableAmount, plan.fee);
    assert(plan.fee >= 0 && plan.fee <= plan.availableAmount);
    
    // adjust/compute amount
    if (!input.use_max_amount()) {
        // reduce amount if needed
        plan.amount = std::max(Amount(0), std::min(plan.amount, plan.availableAmount - plan.fee));
    } else {
        // max available amount
        plan.amount = std::max(Amount(0), plan.availableAmount - plan.fee);
    }
    assert(plan.amount >= 0 && plan.amount <= plan.availableAmount);

    // compute change
    plan.change = plan.availableAmount - plan.amount - plan.fee;
    assert(plan.change >= 0 && plan.change <= plan.availableAmount);
    assert(!input.use_max_amount() || plan.change == 0); // change is 0 in max amount case

    assert(plan.amount + plan.change + plan.fee == plan.availableAmount);

    return plan;
}

} // namespace TW::Bitcoin
