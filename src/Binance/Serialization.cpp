// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Serialization.h"

#include "Address.h"
#include "Bech32Address.h"
#include "Ethereum/Address.h"
#include "../HexCoding.h"

using namespace TW;
using namespace TW::Binance;
using namespace google::protobuf;

using json = nlohmann::json;

static inline std::string addressString(const std::string& bytes) {
    auto data = Data(bytes.begin(), bytes.end());
    return Address(data).string();
}

static inline std::string validatorAddress(const std::string& bytes) {
    auto data = Data(bytes.begin(), bytes.end());
    return Bech32Address(Address::hrpValidator, data).string();
}

json Binance::signatureJSON(const Proto::SigningInput& input) {
    json j;
    j["account_number"] = std::to_string(input.account_number());
    j["chain_id"] = input.chain_id();
    j["data"] = nullptr;
    j["memo"] = input.memo();
    j["msgs"] = json::array({orderJSON(input)});
    j["sequence"] = std::to_string(input.sequence());
    j["source"] = std::to_string(input.source());
    return j;
}

json Binance::orderJSON(const Proto::SigningInput& input) {
    json j;
    if (input.has_trade_order()) {
        j["id"] = input.trade_order().id();
        j["ordertype"] = 2;
        j["price"] = input.trade_order().price();
        j["quantity"] = input.trade_order().quantity();
        j["sender"] = addressString(input.trade_order().sender());
        j["side"] = input.trade_order().side();
        j["symbol"] = input.trade_order().symbol();
        j["timeinforce"] = input.trade_order().timeinforce();
    } else if (input.has_cancel_trade_order()) {
        j["refid"] = input.cancel_trade_order().refid();
        j["sender"] = addressString(input.cancel_trade_order().sender());
        j["symbol"] = input.cancel_trade_order().symbol();
    } else if (input.has_send_order()) {
        j["inputs"] = inputsJSON(input.send_order());
        j["outputs"] = outputsJSON(input.send_order());
    } else if (input.has_freeze_order()) {
        j["from"] = addressString(input.freeze_order().from());
        j["symbol"] = input.freeze_order().symbol();
        j["amount"] = input.freeze_order().amount();
    } else if (input.has_unfreeze_order()) {
        j["from"] = addressString(input.unfreeze_order().from());
        j["symbol"] = input.unfreeze_order().symbol();
        j["amount"] = input.unfreeze_order().amount();
    } else if (input.has_htlt_order()) {
        j["from"] = addressString(input.htlt_order().from());
        j["to"] = addressString(input.htlt_order().to());
        j["recipient_other_chain"] = input.htlt_order().recipient_other_chain();
        j["sender_other_chain"] = input.htlt_order().sender_other_chain();
        j["random_number_hash"] = hex(input.htlt_order().random_number_hash());
        j["timestamp"] = input.htlt_order().timestamp();
        j["amount"] = tokensJSON(input.htlt_order().amount());
        j["expected_income"] = input.htlt_order().expected_income();
        j["height_span"] = input.htlt_order().height_span();
        j["cross_chain"] = input.htlt_order().cross_chain();
    } else if (input.has_deposithtlt_order()) {
        j["from"] = addressString(input.deposithtlt_order().from());
        j["swap_id"] = hex(input.deposithtlt_order().swap_id());
        j["amount"] = tokensJSON(input.deposithtlt_order().amount());
    } else if (input.has_claimhtlt_order()) {
        j["from"] = addressString(input.claimhtlt_order().from());
        j["swap_id"] = hex(input.claimhtlt_order().swap_id());
        j["random_number"] = hex(input.claimhtlt_order().random_number());
    } else if (input.has_refundhtlt_order()) {
        j["from"] = addressString(input.refundhtlt_order().from());
        j["swap_id"] = hex(input.refundhtlt_order().swap_id());
    } else if (input.has_transfer_out_order()) {
        auto to = input.transfer_out_order().to();
        auto addr = Ethereum::Address(Data(to.begin(), to.end()));
        j["from"] = addressString(input.transfer_out_order().from());
        j["to"] = addr.string();
        j["amount"] = tokenJSON(input.transfer_out_order().amount());
        j["expire_time"] = input.transfer_out_order().expire_time();
    } else if (input.has_side_delegate_order()) {
        j["type"] = "cosmos-sdk/MsgSideChainDelegate";
        j["value"] = {
            {"delegator_addr", addressString(input.side_delegate_order().delegator_addr())},
            {"validator_addr",validatorAddress(input.side_delegate_order().validator_addr())},
            {"delegation", tokenJSON(input.side_delegate_order().delegation(), true)},
            {"side_chain_id", input.side_delegate_order().chain_id()},
        };
    } else if (input.has_side_redelegate_order()) {
        j["type"] = "cosmos-sdk/MsgSideChainRedelegate";
        j["value"] = {
            {"delegator_addr", addressString(input.side_redelegate_order().delegator_addr())},
            {"validator_src_addr", validatorAddress(input.side_redelegate_order().validator_src_addr())},
            {"validator_dst_addr", validatorAddress(input.side_redelegate_order().validator_dst_addr())},
            {"amount", tokenJSON(input.side_redelegate_order().amount(), true)},
            {"side_chain_id", input.side_redelegate_order().chain_id()},
        };
    } else if (input.has_side_undelegate_order()) {
        j["type"] = "cosmos-sdk/MsgSideChainUndelegate";
        j["value"] = {
            {"delegator_addr", addressString(input.side_undelegate_order().delegator_addr())},
            {"validator_addr", validatorAddress(input.side_undelegate_order().validator_addr())},
            {"amount", tokenJSON(input.side_undelegate_order().amount(), true)},
            {"side_chain_id", input.side_undelegate_order().chain_id()},
        };
    } else if (input.has_time_lock_order()) {
        j["from"] = addressString(input.time_lock_order().from_address());
        j["description"] = input.time_lock_order().description();
        j["amount"] = input.time_lock_order().amount();
        j["lock_time"] = input.time_lock_order().lock_time();
    } else if (input.has_time_relock_order()) {
        j["from"] = addressString(input.time_relock_order().from_address());
        j["id"] = input.time_relock_order().id();
        j["description"] = input.time_relock_order().description();
        j["amount"] = input.time_relock_order().amount();
        j["lock_time"] = input.time_relock_order().lock_time();
    } else if (input.has_time_unlock_order()) {
        j["from"] = addressString(input.time_unlock_order().from_address());
        j["id"] = input.time_unlock_order().id();
    }
    return j;
}

json Binance::inputsJSON(const Proto::SendOrder& order) {
    json j = json::array();
    for (auto& input : order.inputs()) {
        j.push_back({
            {"address", addressString(input.address())},
            {"coins", tokensJSON(input.coins())}
        });
    }
    return j;
}

json Binance::outputsJSON(const Proto::SendOrder& order) {
    json j = json::array();
    for (auto& output : order.outputs()) {
        j.push_back({
            {"address", addressString(output.address())},
            {"coins", tokensJSON(output.coins())}
        });
    }
    return j;
}

json Binance::tokenJSON(const Proto::SendOrder_Token& token, bool stringAmount) {
    json j = { {"denom", token.denom()} };
    if (stringAmount) {
        j["amount"] = std::to_string(token.amount());
    } else {
        j["amount"] = token.amount();
    }
    return j;
}

json Binance::tokensJSON(const RepeatedPtrField<Proto::SendOrder_Token>& tokens) {
    json j = json::array();
    for (auto& token : tokens) {
        j.push_back(tokenJSON(token));
    }
    return j;
}
