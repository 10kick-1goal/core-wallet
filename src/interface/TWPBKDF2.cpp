// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrezorCrypto/memzero.h>
#include <TrezorCrypto/pbkdf2.h>
#include <TrustWalletCore/TWPBKDF2.h>

#include "Data.h"

using namespace TW;

TWData* _Nullable TWPBKDF2HmacSha256(TWData* _Nonnull password, TWData* _Nonnull salt,
                                     uint32_t iterations, uint32_t dkLen) {

    Data key(dkLen);
    int passLen = static_cast<int>(TWDataSize(password));
    int saltLen = static_cast<int>(TWDataSize(salt));
    pbkdf2_hmac_sha256(
        TWDataBytes(password), 
        passLen, 
        TWDataBytes(salt), 
        saltLen,
        iterations, 
        key.data(), 
        dkLen
    );
    return TWDataCreateWithData(&key);
}

TWData* _Nullable TWPBKDF2HmacSha512(TWData* _Nonnull password, TWData* _Nonnull salt,
                                     uint32_t iterations, uint32_t dkLen) {
    Data key(dkLen);
    int passLen = static_cast<int>(TWDataSize(password));
    int saltLen = static_cast<int>(TWDataSize(salt));
    pbkdf2_hmac_sha512(
        TWDataBytes(password), 
        passLen, 
        TWDataBytes(salt), 
        saltLen,
        iterations, 
        key.data(), 
        dkLen
    );
    return TWDataCreateWithData(&key);
}
