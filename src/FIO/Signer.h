// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Address.h"
#include "../Data.h"
#include "../PrivateKey.h"
#include "../PublicKey.h"
#include "../proto/FIO.pb.h"

#include <string>

namespace TW::FIO {

/// FIO Signer, signing primitives.  See also TransactionBuilder for building full signed transaction
class Signer {
  public:
    /// Signs a Proto::SigningInput transaction
    static Proto::SigningOutput sign(const Proto::SigningInput& input) noexcept;

  public:
    static constexpr auto SignatureSuffix = "K1";
    static constexpr auto SignaturePrefix = "SIG_K1_";

    /// Sign the hash of the provided data
    static Data signData(const PrivateKey& privKey, const Data& data);

    /// Used internally, encode signature to base58 with prefix. Ex.: "SIG_K1_K54CA1jmhgWrSdvrNrkokPyvqh7dwsSoQHNU9xgD3Ezf6cJySzhKeUubVRqmpYdnjoP1DM6SorroVAgrCu3qqvJ9coAQ6u"
    static std::string signatureToBsase58(const Data& sig);

    /// Verify a signature, used in testing
    static bool verify(const PublicKey& pubKey, const Data& data, const Data& signature);

    static int isCanonical(uint8_t by, uint8_t sig[64]);
};

} // namespace TW::FIO
