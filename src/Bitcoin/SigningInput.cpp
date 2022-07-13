// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "SigningInput.h"

using namespace TW;
using namespace TW::Bitcoin;

SigningInput::SigningInput(const Proto::SigningInput& input) {
    hashType = static_cast<TWBitcoinSigHashType>(input.hash_type());
    amount = input.amount();
    byteFee = input.byte_fee();
    toAddress = input.to_address();
    changeAddress = input.change_address();
    for (auto&& key: input.private_key()) {
        privateKeys.emplace_back(key);
    }
    for (auto&& script: input.scripts()) {
        scripts[script.first] = Script(script.second.begin(), script.second.end());
    }
    for (auto&& u: input.utxo()) {
        utxos.emplace_back(u);
    }
    useMaxAmount = input.use_max_amount();
    coinType = static_cast<TWCoinType>(input.coin_type());
    if (input.has_plan()) {
        plan = TransactionPlan(input.plan());
    }
    outputOpReturn = data(input.output_op_return());
    lockTime = input.lock_time();
}
