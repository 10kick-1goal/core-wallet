// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWBitcoinSigHashType.h>

bool TWBitcoinSigHashTypeIsSingle(enum TWBitcoinSigHashType type) {
    return (type & 0x1f) == TWBitcoinSigHashTypeSingle;
}

bool TWBitcoinSigHashTypeIsNone(enum TWBitcoinSigHashType type) {
    return (type & 0x1f) == TWBitcoinSigHashTypeNone;
}
