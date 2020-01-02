// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Address.h"
#include "../Data.h"
#include "../proto/Polkadot.pb.h"
#include "../uint256.h"
#include  "ScaleCodec.h"

namespace TW::Polkadot {

class Extrinsic {
  public:
    Data blockHash;
    Data genesisHash;
    uint64_t nonce;
    // Runtime spec version
    uint32_t specVersion;
    // Extrinsic version
    uint32_t version;
    // balances::TakeFees
    uint256_t tip;
    // encoded Era data
    Data era;
    // encoded Call data
    Data call;

    Extrinsic(const Proto::SigningInput& input)
        : blockHash(input.block_hash().begin(), input.block_hash().end())
        , genesisHash(input.genesis_hash().begin(), input.genesis_hash().end())
        , nonce(input.nonce())
        , specVersion(input.spec_version())
        , version(input.extrinsic_version())
        , tip(load(input.tip())) {
        if (input.has_era()) {
            era = encodeEra(input.era().phase(), input.era().period());
        } else {
            era = encodeCompact(0);
        }
        call = encodeCall(input);
    }

    static Data encodeCall(const Proto::SigningInput& input);
    // Payload to sign.
    Data encodePayload() const;
    // Encode final data with signer public key and signature.
    Data encodeSignature(const PublicKey& signer, const Data& signature) const;

  protected:
    Data encodeEraNonceTip() const;
};

} // namespace TW::Polkadot
