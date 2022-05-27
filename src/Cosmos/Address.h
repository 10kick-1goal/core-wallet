// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Bech32Address.h"
#include "../Data.h"
#include "../PublicKey.h"
#include "../Coin.h"
#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWHRP.h>

#include <string>

namespace TW::Cosmos {

/// A Bech32 Cosmos address.  Hrp has to be specified (e.g. "cosmos", "terra"...), hash is coin-specific (from config, usually sha256ripemd).
class Address: public Bech32Address {
public:
    Address() : Bech32Address("") {}

    /// Initializes an address with a key hash, with prefix of the given coin.
    Address(TWCoinType coin, const Data& keyHash) : Bech32Address(stringForHRP(TW::hrp(coin)), keyHash) {}

    /// Initializes an address with a key hash, with given prefix.
    Address(const std::string& hrp, const Data& keyHash) : Bech32Address(hrp, keyHash) {}

    /// Initializes an address with a public key, with prefix of the given coin.
    Address(TWCoinType coin, const PublicKey& publicKey) : Bech32Address(stringForHRP(TW::hrp(coin)), TW::addressHasher(coin), publicKey) {}

    /// Initializes an address with a public key, with given prefix.
    Address(const std::string& hrp, const PublicKey& publicKey, TWCoinType coin = TWCoinTypeCosmos) : Bech32Address(hrp, TW::addressHasher(coin), publicKey) {}

    /// Determines whether a string makes a valid Bech32 address, and the HRP matches to the coin.
    static bool isValid(TWCoinType coin, const std::string& addr) {
        const auto* const hrp = stringForHRP(TW::hrp(coin));
        return Bech32Address::isValid(addr, hrp);
    }

    /// Creates an address object from the given string, if valid.  Returns success.
    static bool decode(const std::string& addr, Address& obj_out) {
        return Bech32Address::decode(addr, obj_out, "");
    }
};

} // namespace TW::Cosmos
