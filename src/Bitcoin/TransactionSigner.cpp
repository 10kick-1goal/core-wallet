// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TransactionSigner.h"

#include "SignatureBuilder.h"

#include "../Groestlcoin/Transaction.h"
#include "../Zcash/Transaction.h"
#include "../Zcash/TransactionBuilder.h"

using namespace TW;
using namespace TW::Bitcoin;

template <typename Transaction, typename TransactionBuilder>
TransactionPlan TransactionSigner<Transaction, TransactionBuilder>::plan(const SigningInput& input) {
    return TransactionBuilder::plan(input);
}

template <typename Transaction, typename TransactionBuilder>
Result<Transaction, Common::Proto::SigningError> TransactionSigner<Transaction, TransactionBuilder>::sign(const SigningInput& input, bool estimationMode, std::optional<SignaturePubkeyList> optionalExternalSigs) {
    TransactionPlan plan;
    if (input.plan.has_value()) {
        plan = input.plan.value();
    } else {
        plan = TransactionBuilder::plan(input);
    }
    auto transaction = TransactionBuilder::template build<Transaction>(plan, input.toAddress, input.changeAddress, input.coinType, input.lockTime);
    SigningMode signingMode =
        estimationMode ? SigningMode_SizeEstimationOnly :
        optionalExternalSigs.has_value() ? SigningMode_External : SigningMode_Normal;
    SignatureBuilder<Transaction> signer(std::move(input), plan, transaction, signingMode, optionalExternalSigs);
    return signer.sign();
}

template <typename Transaction, typename TransactionBuilder>
Result<HashPubkeyList, Common::Proto::SigningError> TransactionSigner<Transaction, TransactionBuilder>::preImageHashes(const SigningInput& input) {
    TransactionPlan plan;
    if (input.plan.has_value()) {
        plan = input.plan.value();
    } else {
        plan = TransactionBuilder::plan(input);
    }
    auto transaction = TransactionBuilder::template build<Transaction>(plan, input.toAddress, input.changeAddress, input.coinType, input.lockTime);
    SignatureBuilder<Transaction> signer(std::move(input), plan, transaction, SigningMode_HashOnly);
    auto signResult = signer.sign();
    if (!signResult) {
        return Result<HashPubkeyList, Common::Proto::SigningError>::failure(signResult.error());
    }
    return Result<HashPubkeyList, Common::Proto::SigningError>::success(signer.getHashesForSigning());
}

// Explicitly instantiate a Signers for compatible transactions.
template class Bitcoin::TransactionSigner<Bitcoin::Transaction, TransactionBuilder>;
template class Bitcoin::TransactionSigner<Zcash::Transaction, Zcash::TransactionBuilder>;
template class Bitcoin::TransactionSigner<Groestlcoin::Transaction, TransactionBuilder>;
