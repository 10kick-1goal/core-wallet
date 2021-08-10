// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "OptInAssetTransaction.h"
#include "BinaryCoding.h"

using namespace TW;
using namespace TW::Algorand;

Data OptInAssetTransaction::serialize() const {
    Data data;

    // encode map length
    uint8_t size = 9;
    if (!note.empty()) {
        // note is optional
        size += 1;
    }
    data.push_back(0x80 + size);

    // encode fields one by one (sorted by name)
    encodeString("arcv", data);
    encodeBytes(Data(address.bytes.begin(), address.bytes.end()), data);

    encodeString("fee", data);
    encodeNumber(fee, data);

    encodeString("fv", data);
    encodeNumber(firstRound, data);

    encodeString("gen", data);
    encodeString(genesisId, data);

    encodeString("gh", data);
    encodeBytes(genesisHash, data);

    encodeString("lv", data);
    encodeNumber(lastRound, data);

    if (!note.empty()) {
        encodeString("note", data);
        encodeBytes(note, data);
    }

    encodeString("snd", data);
    encodeBytes(Data(address.bytes.begin(), address.bytes.end()), data);

    encodeString("type", data);
    encodeString(type, data);

    encodeString("xaid", data);
    encodeNumber(assetId, data);

    return data;
}
