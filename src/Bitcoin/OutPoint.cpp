// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "OutPoint.h"

#include "../BinaryCoding.h"

using namespace TW::Bitcoin;

void OutPoint::encode(std::vector<uint8_t>& data) const {
    std::copy(std::begin(hash), std::end(hash), std::back_inserter(data));
    encode32LE(index, data);
}
