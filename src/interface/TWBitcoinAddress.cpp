// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../Base58.h"
#include "../Bitcoin/Address.h"

#include <TrezorCrypto/ecdsa.h>
#include <TrustWalletCore/TWBitcoinAddress.h>
#include <TrustWalletCore/TWPublicKey.h>

#include <cstring>

using namespace TW::Bitcoin;

bool TWBitcoinAddressEqual(struct TWBitcoinAddress *_Nonnull lhs, struct TWBitcoinAddress *_Nonnull rhs) {
    return lhs->impl == rhs->impl;
}

bool TWBitcoinAddressIsValid(TWData *_Nonnull data) {
    return TWDataSize(data) == Address::size;
}

bool TWBitcoinAddressIsValidString(TWString *_Nonnull string) {
    auto& s = *reinterpret_cast<const std::string*>(string);
    return Address::isValid(s);
}

struct TWBitcoinAddress *_Nullable TWBitcoinAddressCreateWithString(TWString *_Nonnull string) {
    auto& s = *reinterpret_cast<const std::string*>(string);
    try {
        return new TWBitcoinAddress{ Address(s) };
    } catch (...) {
        return nullptr;
    }
}

struct TWBitcoinAddress *_Nullable TWBitcoinAddressCreateWithData(TWData *_Nonnull data) {
    auto& d = *reinterpret_cast<const TW::Data*>(data);
    try {
        return new TWBitcoinAddress{ Address(d) };
    } catch (...) {
        return nullptr;
    }
}

struct TWBitcoinAddress *_Nullable TWBitcoinAddressCreateWithPublicKey(struct TWPublicKey *_Nonnull publicKey, uint8_t prefix) {
    try {
        return new TWBitcoinAddress{ Address(publicKey->impl, prefix) };
    } catch (...) {
        return nullptr;
    }
}

void TWBitcoinAddressDelete(struct TWBitcoinAddress *_Nonnull address) {
    delete address;
}

TWString *_Nonnull TWBitcoinAddressDescription(struct TWBitcoinAddress *_Nonnull address) {
    return TWStringCreateWithUTF8Bytes(address->impl.string().c_str());
}

uint8_t TWBitcoinAddressPrefix(struct TWBitcoinAddress *_Nonnull address) {
    return address->impl.bytes[0];
}

TWData *_Nonnull TWBitcoinAddressKeyhash(struct TWBitcoinAddress *_Nonnull address) {
    return TWDataCreateWithBytes(address->impl.bytes.data() + 1, Address::size - 1);
}
