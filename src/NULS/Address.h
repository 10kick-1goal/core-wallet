// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.
#pragma once
#include "../Base58Address.h"
#include "../PrivateKey.h"
#include "../PublicKey.h"

#include <string>

namespace TW::NULS {

class Address : public Base58Address<24> {
  public:
    /// NULS Main Net Chain ID = 1
    static const std::array<byte, 2> mainnetId;

    /// NULS address prefix
    static const std::string prefix;

    /// NULS address type
    static const byte addressType = 0x01;

    /// Determines whether a string makes a valid address.
    static bool isValid(const std::string& string);

    /// Initializes an address from a string representation.
    explicit Address(const std::string& string);

    /// Initializes an address from a public key.
    explicit Address(const PublicKey& publicKey);

    /// Initializes an address with a collection of bytes.
    explicit Address(const Data& data) : TW::Base58Address<24>(data) {}

    /// Determines is a valid address.
    bool isValid() const;

    /// Mainnet chain id
    uint16_t chainID() const;

    /// TX type
    uint8_t type() const;

    /// Returns a string representation of the address.
    std::string string() const;

    /// calculate checksum
    uint8_t checksum(std::array<byte, size>& byteArray) const;
};

static inline bool operator==(const Address& lhs, const Address& rhs) {
    return lhs.bytes == rhs.bytes;
}

} // namespace TW::NULS

/// Wrapper for C interface.
struct TWNULSAddress {
    TW::NULS::Address impl;
};