// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "BinaryCoding.h"
#include "Forging.h"
#include "HexCoding.h"
#include "OperationList.h"
#include "../Base58.h"
#include "../proto/Tezos.pb.h"

using namespace TW;
using namespace TW::Tezos;
using namespace TW::Tezos::Proto;

TW::Tezos::OperationList::OperationList(const std::string& str) {
    branch = str;
}

void TW::Tezos::OperationList::addOperation(const Operation& operation) {
    operation_list.push_back(operation);
}

// Forge the given branch to a hex encoded string.
Data TW::Tezos::OperationList::forgeBranch() const {
    std::array<byte, 2> prefix = {1, 52};
    const auto decoded = Base58::bitcoin.decodeCheck(branch);
    if (decoded.size() != 34 || !std::equal(prefix.begin(), prefix.end(), decoded.begin())) {
        throw std::invalid_argument("Invalid branch for forge");
    }
    auto forged = Data();
    forged.insert(forged.end(), decoded.begin() + prefix.size(), decoded.end());
    return forged;
}

Data TW::Tezos::OperationList::forge() const {
    auto forged = forgeBranch();

    for (auto operation : operation_list) {
        append(forged, forgeOperation(operation));
    }
    return forged;
}
