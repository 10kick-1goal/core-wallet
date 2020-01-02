// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Action.h"
#include "../HexCoding.h"
#include "../Bravo/Serialization.h"

using namespace TW;
using namespace TW::EOS;
using json = nlohmann::json;

void PermissionLevel::serialize(Data& o) const {
    actor.serialize(o);
    permission.serialize(o);
}

json PermissionLevel::serialize() const noexcept {
    json obj;
    obj["actor"] = actor.string();
    obj["permission"] = permission.string();
    return obj;
}

void Action::serialize(Data& o) const {
    account.serialize(o);
    name.serialize(o);
    Bravo::encodeCollection(authorization, o);
    Bravo::encodeVarInt64(data.size(), o);
    append(o, data);
}

json Action::serialize() const noexcept {
    json obj;
    obj["account"] = account.string();
    obj["name"] = name.string();
    obj["authorizations"] = Bravo::encodeCollection(authorization);
    obj["data"] = hex(data);
    return obj;
}

TransferAction::TransferAction( const std::string& currency,
                                const std::string& from, 
                                const std::string& to, 
                                const Bravo::Asset& asset, 
                                const std::string& memo) {
    account = Name(currency);
    name = Name("transfer");
    authorization.push_back(PermissionLevel(Name(from), Name("active")));

    setData(from, to, asset, memo);
}

void TransferAction::setData(const std::string& from, const std::string& to, const Bravo::Asset& asset, const std::string& memo) {
    if (asset.amount <= 0) {
        throw std::invalid_argument("Amount in a transfer action must be greater than zero.");
    }

    Name(from).serialize(data);
    Name(to).serialize(data);
    asset.serialize(data);
    Bravo::encodeString(memo, data);
}
