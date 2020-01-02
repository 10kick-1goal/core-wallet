// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../proto/Cosmos.pb.h"
#include "../Data.h"

namespace TW::Cosmos {

/// Helper class that performs Cosmos transaction signing.
class Signer {
public:
    Proto::SigningInput input;

    /// Initializes a transaction signer.
    Signer(Proto::SigningInput&& input) :input(input) {}

    /// Signs the transaction.
    ///
    /// \returns the transaction signature or an empty vector if there is an error.
    Data sign() const;

    /// Builds the signed transaction.
    ///
    /// \returns the signed transaction.
    Proto::SigningOutput build() const;
};

} // namespace

/// Wrapper for C interface.
struct TWCosmosSigner {
    TW::Cosmos::Signer impl;
};
