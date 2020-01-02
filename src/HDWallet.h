// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Data.h"
#include "DerivationPath.h"
#include "Hash.h"
#include "PrivateKey.h"
#include "PublicKey.h"

#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWCurve.h>
#include <TrustWalletCore/TWHDVersion.h>
#include <TrustWalletCore/TWPurpose.h>

#include <array>
#include <optional>
#include <string>

namespace TW {

class HDWallet {
  public:
    static constexpr size_t seedSize = 64;
    static constexpr size_t maxMnemomincSize = 240;
    static constexpr size_t maxExtendedKeySize = 128;

  public:
    /// Wallet seed.
    std::array<byte, seedSize> seed;

    /// Mnemonic word list.
    std::string mnemonic;

    /// Mnemonic passphrase.
    std::string passphrase;

    /// Entropy bytes (11 bits from each word)
    TW::Data entropy;

  public:
    /// Determines if a mnemonic phrase is valid.
    static bool isValid(const std::string& mnemonic);

    /// Initializes a new random HDWallet with the provided strength in bits.
    HDWallet(int strength, const std::string& passphrase);

    /// Initializes an HDWallet from a mnemonic seed.
    HDWallet(const std::string& mnemonic, const std::string& passphrase);

    /// Initializes an HDWallet from a seed.
    HDWallet(const Data& data, const std::string& passphrase);

    HDWallet(const HDWallet& other) = default;
    HDWallet(HDWallet&& other) = default;
    HDWallet& operator=(const HDWallet& other) = default;
    HDWallet& operator=(HDWallet&& other) = default;

    virtual ~HDWallet();

    void updateEntropy();

    /// Returns master key.
    PrivateKey getMasterKey(TWCurve curve) const;

    /// Returns the master private key extension (32 byte).
    PrivateKey getMasterKeyExtension(TWCurve curve) const;

    /// Returns the private key at the given derivation path.
    PrivateKey getKey(const DerivationPath& derivationPath) const;

    /// Derives the address for a coin.
    std::string deriveAddress(TWCoinType coin) const;

    /// Returns the extended private key.
    std::string getExtendedPrivateKey(TWPurpose purpose, TWCoinType coin, TWHDVersion version) const;

    /// Returns the exteded public key.
    std::string getExtendedPublicKey(TWPurpose purpose, TWCoinType coin, TWHDVersion version) const;

    /// Computes the public key from an exteded public key representation.
    static std::optional<PublicKey> getPublicKeyFromExtended(const std::string &extended, const DerivationPath& path);

    /// Computes the private key from an exteded private key representation.
    static std::optional<PrivateKey> getPrivateKeyFromExtended(const std::string &extended, const DerivationPath& path);

  public:
    // Private key type (later could be moved out of HDWallet)
    enum PrivateKeyType {
      PrivateKeyTypeDefault32 = 0, // 32-byte private key
      PrivateKeyTypeExtended96 = 1, // 3*32-byte extended private key
    };
    
    // obtain privateKeyType used by the coin/curve
    static PrivateKeyType getPrivateKeyType(TWCurve curve);
};

} // namespace TW

/// Wrapper for C interface.
struct TWHDWallet {
    TW::HDWallet impl;
};
