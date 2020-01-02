// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Staking.h"
#include "Transaction.h"
#include "../Data.h"
#include "../Hash.h"
#include "../PrivateKey.h"
#include "../proto/Harmony.pb.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <tuple>
#include <vector>

namespace TW::Harmony {

/// Helper class that performs Harmony transaction signing.
class Signer {
  private:
    static Proto::SigningOutput
    signTransaction(const Proto::SigningInput &input) noexcept;

    static Proto::SigningOutput
    signCreateValidator(const Proto::SigningInput &input) noexcept;

    static Proto::SigningOutput
    signEditValidator(const Proto::SigningInput &input) noexcept;

    static Proto::SigningOutput
    signDelegate(const Proto::SigningInput &input) noexcept;

    static Proto::SigningOutput
    signUndelegate(const Proto::SigningInput &input) noexcept;

    static Proto::SigningOutput
    signCollectRewards(const Proto::SigningInput &input) noexcept;

  public:
    uint256_t chainID;

    /// Initializes a signer with a chain identifier.
    explicit Signer(uint256_t chainID) : chainID(std::move(chainID)) {}

    template <typename T>
    static Proto::SigningOutput prepareOutput(const Data &encoded, const T &transaction) noexcept;

    /// Signs a Proto::SigningInput transaction or staking
    static Proto::SigningOutput sign(const Proto::SigningInput &input) noexcept;

    /// Signs the given transaction.
    template <typename T>
    void sign(const PrivateKey &privateKey, const Data &hash, T &transaction) const noexcept;

    /// Signs a hash with the given private key for the given chain identifier.
    ///
    /// @returns the r, s, and v values of the transaction signature
    static std::tuple<uint256_t, uint256_t, uint256_t>
    sign(const uint256_t &chainID, const PrivateKey &privateKey, const Data &hash) noexcept;

    /// R, S, and V values for the given chain identifier and signature.
    ///
    /// @returns the r, s, and v values of the transaction signature
    static std::tuple<uint256_t, uint256_t, uint256_t> values(const uint256_t &chainID,
                                                              const Data &signature) noexcept;

    std::string txnAsRLPHex(Transaction &transaction) const noexcept;

    template <typename Directive>
    std::string txnAsRLPHex(Staking<Directive> &transaction) const noexcept;

  protected:
    /// Computes the transaction hash.
    Data hash(const Transaction &transaction) const noexcept;
    /// Computes the staking transaction hash.
    template <typename Directive>
    Data hash(const Staking<Directive> &transaction) const noexcept;
    // Plain rlp encoding of transaction before hashing
    Data rlpNoHash(const Transaction &transaction, const bool) const noexcept;
    // Plain rlp encoding of staking transaction before hashing
    template <typename Directive>
    Data rlpNoHash(const Staking<Directive> &transaction, const bool) const noexcept;

    Data rlpNoHashDirective(const Staking<CreateValidator> &transaction) const noexcept;
    Data rlpNoHashDirective(const Staking<EditValidator> &transaction) const noexcept;
    Data rlpNoHashDirective(const Staking<Delegate> &transaction) const noexcept;
    Data rlpNoHashDirective(const Staking<Undelegate> &transaction) const noexcept;
    Data rlpNoHashDirective(const Staking<CollectRewards> &transaction) const noexcept;
};

} // namespace TW::Harmony

/// Wrapper for C interface.
struct TWHarmonySigner {
    TW::Harmony::Signer impl;
};
