// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Bitcoin/Script.h"
#include "../Bitcoin/Transaction.h"
#include "../Bitcoin/TransactionInput.h"
#include "../Bitcoin/TransactionOutput.h"
#include "../proto/Bitcoin.pb.h"

#include <array>
#include <vector>

namespace TW::Zcash {

extern const std::array<byte, 4> SaplingBranchID;
extern const std::array<byte, 4> BlossomBranchID;

/// Only supports transparent transaction right now
/// See also https://github.com/zcash/zips/blob/master/zip-0243.rst
struct Transaction {
    uint32_t version = 0x80000004;
    uint32_t versionGroupId = 0x892F2085;
    uint32_t lockTime = 0;
    uint32_t expiryHeight = 0;
    uint64_t valueBalance = 0;

    std::vector<Bitcoin::TransactionInput> inputs;
    std::vector<Bitcoin::TransactionOutput> outputs;
    std::array<byte, 4> branchId;

    Transaction() = default;

    Transaction(uint32_t version, uint32_t versionGroupId, uint32_t lockTime, uint32_t expiryHeight,
                uint64_t valueBalance, std::array<byte, 4> branchId)
        : version(version)
        , versionGroupId(versionGroupId)
        , lockTime(lockTime)
        , expiryHeight(expiryHeight)
        , valueBalance(valueBalance)
        , branchId(branchId) {}

    /// Whether the transaction is empty.
    bool empty() const { return inputs.empty() && outputs.empty(); }

    /// Generates the signature pre-image.
    Data getPreImage(const Bitcoin::Script& scriptCode, size_t index,
                     enum TWBitcoinSigHashType hashType, uint64_t amount) const;
    Data getPrevoutHash() const;
    Data getSequenceHash() const;
    Data getOutputsHash() const;

    Data getJoinSplitsHash() const;
    Data getShieldedSpendsHash() const;
    Data getShieldedOutputsHash() const;

    /// Encodes the rawtx into the provided buffer.
    void encode(Data& data) const;

    Data getSignatureHash(const Bitcoin::Script& scriptCode, size_t index,
                          enum TWBitcoinSigHashType hashType, uint64_t amount,
                          TWBitcoinSignatureVersion version) const;

    /// Converts to Protobuf model
    Bitcoin::Proto::Transaction proto() const;
};

} // namespace TW::Zcash
