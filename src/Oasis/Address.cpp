// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"

#include <TrustWalletCore/TWHRP.h>

#define COIN_ADDRESS_CONTEXT "oasis-core/address: staking"
#define COIN_ADDRESS_VERSION  0

using namespace TW::Oasis;

const std::string Address::hrp = HRP_OASIS;

Address::Address(const Data& keyHash) : Bech32Address(hrp, keyHash) {
    if (getKeyHash().size() != Address::size) {
        throw std::invalid_argument("invalid address data");
    }
}

Address::Address(const TW::PublicKey& publicKey) : Bech32Address(hrp){
    if (publicKey.type != TWPublicKeyTypeED25519) {
        throw std::invalid_argument("address may only be an extended ED25519 public key");
    }

    std::string context = COIN_ADDRESS_CONTEXT;
    Data toHash(context.begin(), context.end());
    toHash.insert(toHash.end(), COIN_ADDRESS_VERSION);
    toHash.insert(toHash.end(), publicKey.bytes.begin(), publicKey.bytes.end());

    const auto hash = Hash::sha512_256(toHash);
    auto key = Data(21);
    key[0] = COIN_ADDRESS_VERSION;
    std::copy(hash.begin(), hash.begin() + 20, key.begin() + 1);

    setKey(key);
}

Address::Address(const std::string& addr) : Bech32Address(addr) {
    if(!isValid(addr)) {
        throw std::invalid_argument("invalid address string");
    }
}

bool Address::isValid(const std::string& addr) {
    return Bech32Address::isValid(addr, hrp);
}

