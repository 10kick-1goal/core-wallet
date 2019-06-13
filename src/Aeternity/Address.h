// Copyright © 2017-2019 Trust.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <PublicKey.h>
#include <string>

namespace TW::Aeternity {

class Address {
  public:
    static const size_t size = 32;
    Data bytes;

    /// Determines whether a string makes a valid address.
    static bool isValid(const std::string &string);

    /// Initializes an address from a string representation.
    explicit Address(const std::string &string);

    /// Initializes an address from a public key.
    explicit Address(const PublicKey &publicKey);

    /// Returns a string representation of the Aeternity address.
    std::string string() const;

  private:

    static bool checkType(const std::string &type);
    static bool checkPayload(const std::string &payload);
};

inline bool operator==(const Address& lhs, const Address& rhs) {
    return lhs.bytes == rhs.bytes;
}

} // namespace TW::Aeternity


// Wrapper for C interface.
struct TWAeternityAddress {
    TW::Aeternity::Address impl;
};