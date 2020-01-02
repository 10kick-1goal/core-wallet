// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../PublicKey.h"

#include <TrustWalletCore/TWIconAddressType.h>

#include <array>
#include <cstdint>
#include <vector>

namespace TW::Icon {

class Address {
  public:
    /// Number of bytes in an address.
    static const size_t size = 20;

    /// Address data consisting of a prefix byte followed by the public key
    /// hash.
    std::array<uint8_t, size> bytes;

    /// Address type.
    TWIconAddressType type;

    /// Determines whether a collection of bytes makes a valid  address.
    static bool isValid(const std::vector<uint8_t>& data) { return data.size() == size; }

    /// Determines whether a string makes a valid  address.
    static bool isValid(const std::string& string);

    /// Initializes an address with a string representation.
    explicit Address(const std::string& string);

    /// Initializes an address with a collection of bytes.
    Address(const std::vector<uint8_t>& data, TWIconAddressType type);

    /// Initializes an address with a public key.
    Address(const PublicKey& publicKey, TWIconAddressType type);

    /// Returns a string representation of the address.
    std::string string() const;
};

inline bool operator==(const Address& lhs, const Address& rhs) {
    return lhs.bytes == rhs.bytes;
}

} // namespace TW::Icon

/// Wrapper for C interface.
struct TWIconAddress {
    TW::Icon::Address impl;
};
