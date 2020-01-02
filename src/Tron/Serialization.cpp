// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Serialization.h"
#include "../HexCoding.h"
#include <sstream>
#include <vector>
#include <algorithm>

using namespace TW;
using namespace TW::Tron;
using namespace std;

using json = nlohmann::json;

string typeName(const protocol::Transaction::Contract::ContractType type) {
    return protocol::Transaction::Contract::ContractType_Name(type);
}

string typeUrl(const protocol::Transaction::Contract::ContractType type) {
    std::ostringstream stringStream;
    stringStream << "type.googleapis.com/protocol." << typeName(type);
    return stringStream.str();
}

json valueJSON(const protocol::TransferContract& contract) {
    json valueJSON;
    valueJSON["owner_address"] = hex(contract.owner_address());
    valueJSON["to_address"] = hex(contract.to_address());
    valueJSON["amount"] = contract.amount();

    return valueJSON;
}

json valueJSON(const protocol::TransferAssetContract& contract) {
    json valueJSON;
    valueJSON["asset_name"] = hex(contract.asset_name());
    valueJSON["owner_address"] = hex(contract.owner_address());
    valueJSON["to_address"] = hex(contract.to_address());
    valueJSON["amount"] = contract.amount();

    return valueJSON;
}

json valueJSON(const protocol::VoteAssetContract& contract) {
    json valueJSON;
    
    vector<string> vote_address;
    for (const string& addr : contract.vote_address()) {
        vote_address.push_back(hex(addr));
    }

    valueJSON["owner_address"] = hex(contract.owner_address());
    valueJSON["vote_address"] = vote_address;
    valueJSON["support"] = contract.support();
    valueJSON["count"] = contract.count();
    
    return valueJSON;
}

json voteJSON(const protocol::VoteWitnessContract::Vote& vote) {
    json voteJSON;
    voteJSON["vote_address"] = hex(vote.vote_address());
    voteJSON["vote_count"] = vote.vote_count();

    return voteJSON;
}

json valueJSON(const protocol::VoteWitnessContract& contract) {
    json valueJSON;

    vector<json> votes;
    for (const protocol::VoteWitnessContract::Vote& vote : contract.votes()) {
        votes.push_back(voteJSON(vote));
    }

    valueJSON["owner_address"] = hex(contract.owner_address());
    valueJSON["votes"] = votes;
    valueJSON["support"] = contract.support();

    return valueJSON;
}

json valueJSON(const protocol::FreezeBalanceContract& contract) {
    json valueJSON;
    valueJSON["owner_address"] = hex(contract.owner_address());
    valueJSON["frozen_balance"] = contract.frozen_balance();
    valueJSON["frozen_duration"] = contract.frozen_duration();
    valueJSON["resource"] = protocol::ResourceCode_Name(contract.resource());
    valueJSON["receiver_address"] = hex(contract.receiver_address());

    return valueJSON;
}

json valueJSON(const protocol::UnfreezeBalanceContract& contract) {
    json valueJSON;
    valueJSON["owner_address"] = hex(contract.owner_address());
    valueJSON["resource"] = protocol::ResourceCode_Name(contract.resource());
    valueJSON["receiver_address"] = hex(contract.receiver_address());

    return valueJSON;
}

json valueJSON(const protocol::WithdrawBalanceContract& contract) {
    json valueJSON;
    valueJSON["owner_address"] = hex(contract.owner_address());

    return valueJSON;
}

json valueJSON(const protocol::UnfreezeAssetContract& contract) {
    json valueJSON;
    valueJSON["owner_address"] = hex(contract.owner_address());

    return valueJSON;
}

json valueJSON(const protocol::TriggerSmartContract& contract) {
    json valueJSON;
    valueJSON["owner_address"] = hex(contract.owner_address());
    valueJSON["contract_address"] = hex(contract.contract_address());
    valueJSON["data"] = hex(contract.data());
    if (contract.call_value() > 0) {
        valueJSON["call_value"] = contract.call_value();
    }
    if (contract.call_token_value() > 0) {
        valueJSON["call_token_value"] = contract.call_token_value();
    }
    if (contract.token_id() > 0) {
        valueJSON["token_id"] = contract.token_id();
    }

    return valueJSON;
}

json parameterJSON(const google::protobuf::Any &parameter, const protocol::Transaction::Contract::ContractType type) {
    json paramJSON;
    paramJSON["type_url"] = typeUrl(type);

    switch (type) {
        case protocol::Transaction::Contract::TransferContract: {
            protocol::TransferContract contract;
            parameter.UnpackTo(&contract);
            paramJSON["value"] = valueJSON(contract);
            break;
        }
        case protocol::Transaction::Contract::TransferAssetContract: {
            protocol::TransferAssetContract contract;
            parameter.UnpackTo(&contract);
            paramJSON["value"] = valueJSON(contract);
            break;
        }
        case protocol::Transaction::Contract::VoteAssetContract: {
            protocol::VoteAssetContract contract;
            parameter.UnpackTo(&contract);
            paramJSON["value"] = valueJSON(contract);
            break;
        }
        case protocol::Transaction::Contract::VoteWitnessContract: {
            protocol::VoteWitnessContract contract;
            parameter.UnpackTo(&contract);
            paramJSON["value"] = valueJSON(contract);
            break;
        }
        case protocol::Transaction::Contract::FreezeBalanceContract: {
            protocol::FreezeBalanceContract contract;
            parameter.UnpackTo(&contract);
            paramJSON["value"] = valueJSON(contract);
            break;
        }
        case protocol::Transaction::Contract::UnfreezeBalanceContract: {
            protocol::UnfreezeBalanceContract contract;
            parameter.UnpackTo(&contract);
            paramJSON["value"] = valueJSON(contract);
            break;
        }
        case protocol::Transaction::Contract::WithdrawBalanceContract: {
            protocol::WithdrawBalanceContract contract;
            parameter.UnpackTo(&contract);
            paramJSON["value"] = valueJSON(contract);
            break;
        }
        case protocol::Transaction::Contract::UnfreezeAssetContract: {
            protocol::UnfreezeAssetContract contract;
            parameter.UnpackTo(&contract);
            paramJSON["value"] = valueJSON(contract);
            break;
        }
        case protocol::Transaction::Contract::TriggerSmartContract: {
            protocol::TriggerSmartContract contract;
            parameter.UnpackTo(&contract);
            paramJSON["value"] = valueJSON(contract);
            break;
        }
        case protocol::Transaction::Contract::AccountCreateContract:
        default:
            break;
    }

    return paramJSON;
}

json contractJSON(const protocol::Transaction::Contract &contract) {
    json contractJSON;
    contractJSON["type"] = typeName(contract.type());
    contractJSON["parameter"] = parameterJSON(contract.parameter(), contract.type());
    return contractJSON;
}

json raw_dataJSON(const protocol::Transaction::raw &raw) {
    json raw_dataJSON;

    raw_dataJSON["ref_block_bytes"] = hex(raw.ref_block_bytes());
    raw_dataJSON["ref_block_hash"] = hex(raw.ref_block_hash());
    if (raw.ref_block_num() > 0) {
        raw_dataJSON["ref_block_num"] = raw.ref_block_num();
    }
    if (raw.fee_limit() > 0) {
        raw_dataJSON["fee_limit"] = raw.fee_limit();
    }
    raw_dataJSON["timestamp"] = raw.timestamp();
    raw_dataJSON["expiration"] = raw.expiration();
    raw_dataJSON["contract"] = json::array({ contractJSON(raw.contract(0)) });

    return raw_dataJSON;
}

json TW::Tron::transactionJSON(const protocol::Transaction& transaction, const TW::Data& txID, const TW::Data& signature) {
    json transactionJSON;
    transactionJSON["raw_data"] = raw_dataJSON(transaction.raw_data());
    transactionJSON["txID"] = hex(txID);
    transactionJSON["signature"] = json::array({ hex(signature) });

    return transactionJSON;
}
