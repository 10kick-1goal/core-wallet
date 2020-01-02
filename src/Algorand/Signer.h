// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Transaction.h"

#include "../Data.h"
#include "../PrivateKey.h"

namespace TW::Algorand {

/// Helper class that performs Algorand transaction signing.
class Signer {
  public:
    Signer() = delete;

    /// Signs a Proto::SigningInput transaction
    static Proto::SigningOutput sign(const Proto::SigningInput& input) noexcept;

    /// Signs the given transaction.
    static Data sign(const PrivateKey& privateKey, Transaction& transaction) noexcept;
};

} // namespace TW::Algorand

/// Wrapper for C interface.
struct TWAlgorandSigner {
    TW::Algorand::Signer impl;
};
