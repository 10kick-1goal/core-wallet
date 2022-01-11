// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "ParamAddress.h"
#include <HexCoding.h>
#include <uint256.h>

using namespace TW::Ethereum::ABI;
using namespace TW;

Data ParamAddress::getData() const {
    Data data = store(getVal(), bytes);
    return data;
}

bool ParamAddress::setValueJson(const std::string& value) {
    setVal(load(parse_hex(value)));
    return true;
}
