// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Signer.h"
#include "Serialization.h"

#include "../Hash.h"
#include "../PrivateKey.h"
#include "../Data.h"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <string>

using namespace TW;
using namespace TW::Cosmos;

using json = nlohmann::json;

Signer::Signer(Proto::SigningInput&& input) {
    if (input.type_prefix().empty()) {
        input.set_type_prefix(TYPE_PREFIX_MSG_SEND);
    }

    if (input.has_send_coins_message()) {
        auto message = input.send_coins_message();
        if (message.type_prefix().empty()) {
            message.set_type_prefix(TYPE_PREFIX_MSG_SEND);
        }
        *input.mutable_send_coins_message() = message;
    } else if (input.has_stake_message()) {
        auto message = input.stake_message();
        if (message.type_prefix().empty()) {
            message.set_type_prefix(TYPE_PREFIX_MSG_DELEGATE);
        }
        *input.mutable_stake_message() = message;
    } else if(input.has_unstake_message()) {
        auto message = input.unstake_message();
        if (message.type_prefix().empty()) {
            message.set_type_prefix(TYPE_PREFIX_MSG_UNDELEGATE);
        }
        *input.mutable_unstake_message() = message;
    } else if(input.has_withdraw_stake_reward_message()) {
        auto message = input.withdraw_stake_reward_message();
        if (message.type_prefix().empty()) {
            message.set_type_prefix(TYPE_PREFIX_MSG_WITHDRAW_REWARD);
        }
        *input.mutable_withdraw_stake_reward_message() = message;
    } else if(input.has_restake_message()) {
        auto message = input.restake_message();
        if (message.type_prefix().empty()) {
            message.set_type_prefix(TYPE_PREFIX_MSG_REDELEGATE);
        }
        *input.mutable_restake_message() = message;
    } else if (input.has_withdraw_stake_rewards_all_message()) {
        auto message = input.withdraw_stake_rewards_all_message();
        if (message.type_prefix().empty()) {
            message.set_type_prefix(TYPE_PREFIX_MSG_WITHDRAW_REWARDS_ALL);
        }
        *input.mutable_withdraw_stake_rewards_all_message() = message;
    }

    this->input = input;
}

std::vector<uint8_t> Signer::sign() const {
    auto key = PrivateKey(input.private_key());
    auto hash = Hash::sha256(signaturePreimage());
    auto signature = key.sign(hash, TWCurveSECP256k1);
    return std::vector<uint8_t>(signature.begin(), signature.end() - 1);
}

std::string Signer::signaturePreimage() const {
    return signaturePreimageJSON(input).dump();
}

json Signer::buildTransactionJSON(const Data& signature) const {    
    auto sig = Cosmos::Proto::Signature();
    sig.set_signature(signature.data(), signature.size());
    auto privateKey = PrivateKey(input.private_key());
    auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeSECP256k1);
    sig.set_public_key(publicKey.bytes.data(), publicKey.bytes.size());

    auto transaction = Cosmos::Proto::Transaction();
    *transaction.mutable_fee() = input.fee();
    transaction.set_memo(input.memo());

    if (input.has_send_coins_message()) {
        *transaction.mutable_send_coins_message() = input.send_coins_message();
    } else if (input.has_stake_message()) {
        *transaction.mutable_stake_message() = input.stake_message();
    } else if (input.has_unstake_message()) {
        *transaction.mutable_unstake_message() = input.unstake_message();
    } else if (input.has_restake_message()) {
        *transaction.mutable_restake_message() = input.restake_message();
    } else if (input.has_withdraw_stake_reward_message()) {
        *transaction.mutable_withdraw_stake_reward_message() = input.withdraw_stake_reward_message();
    } else if (input.has_withdraw_stake_rewards_all_message()) {
        *transaction.mutable_withdraw_stake_rewards_all_message() = input.withdraw_stake_rewards_all_message();
    }
    
    *transaction.mutable_signature() = sig;
    
    return transactionJSON(transaction, input.type_prefix());
}

std::string Signer::buildTransaction() const {    
    auto signature = sign();
    return buildTransactionJSON(signature).dump();
}

Proto::SigningOutput Signer::build() const {
    auto output = Proto::SigningOutput();

    auto signature = sign();
    auto txJson = buildTransactionJSON(signature);

    output.set_json(txJson.dump());
    output.set_signature(signature.data(), signature.size());

    return output;
}
