// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TransactionOutput.h"

#include "../BinaryCoding.h"

using namespace TW::Decred;

void TransactionOutput::encode(Data& data) const {
    encode64LE(value, data);
    encode16LE(version, data);
    script.encode(data);
}
