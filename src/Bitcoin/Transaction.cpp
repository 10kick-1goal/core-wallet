// Copyright © 2017-2019 Trust.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Transaction.h"

#include "BinaryCoding.h"
#include "Bech32Address.h"
#include "../Hash.h"

#include <TrustWalletCore/TWBitcoin.h>

#include <cassert>

using namespace TW::Bitcoin;

std::vector<uint8_t> Transaction::getPreImage(const Script& scriptCode, int index, uint32_t hashType, uint64_t amount) const {
    assert(index < inputs.size());

    auto data = std::vector<uint8_t>{};

    // Version
    encode32(version, data);

    // Input prevouts (none/all, depending on flags)
    if ((hashType & TWSignatureHashTypeAnyoneCanPay) == 0) {
        auto hashPrevouts = getPrevoutHash();
        std::copy(std::begin(hashPrevouts), std::end(hashPrevouts), std::back_inserter(data));
    } else {
        std::fill_n(back_inserter(data), 32, 0);
    }

    // Input nSequence (none/all, depending on flags)
    if ((hashType & TWSignatureHashTypeAnyoneCanPay) == 0 && !TWSignatureHashTypeIsSingle(hashType) && !TWSignatureHashTypeIsNone(hashType)) {
        auto hashSequence = getSequenceHash();
        std::copy(std::begin(hashSequence), std::end(hashSequence), std::back_inserter(data));
    } else {
        std::fill_n(back_inserter(data), 32, 0);
    }

    // The input being signed (replacing the scriptSig with scriptCode + amount)
    // The prevout may already be contained in hashPrevout, and the nSequence
    // may already be contain in hashSequence.
    reinterpret_cast<const TW::Bitcoin::OutPoint&>(inputs[index].previousOutput).encode(data);
    scriptCode.encode(data);

    encode64(amount, data);
    encode32(inputs[index].sequence, data);

    // Outputs (none/one/all, depending on flags)
    if (!TWSignatureHashTypeIsSingle(hashType) && !TWSignatureHashTypeIsNone(hashType)) {
        auto hashOutputs = getOutputsHash();
        copy(begin(hashOutputs), end(hashOutputs), back_inserter(data));
    } else if (TWSignatureHashTypeIsSingle(hashType) && index < outputs.size()) {
        auto outputData = std::vector<uint8_t>{};
        outputs[index].encode(outputData);
        auto hashOutputs = TW::Hash::sha256(TW::Hash::sha256(outputData));
        copy(begin(hashOutputs), end(hashOutputs), back_inserter(data));
    } else {
        fill_n(back_inserter(data), 32, 0);
    }

    // Locktime
    encode32(lockTime, data);

    // Sighash type
    encode32(hashType, data);

    return data;
}

std::vector<uint8_t> Transaction::getPrevoutHash() const {
    auto data = std::vector<uint8_t>{};
    for (auto& input : inputs) {
        auto& outpoint = reinterpret_cast<const TW::Bitcoin::OutPoint&>(input.previousOutput);
        outpoint.encode(data);
    }
    auto hash = TW::Hash::sha256(TW::Hash::sha256(data));
    return hash;
}

std::vector<uint8_t> Transaction::getSequenceHash() const {
    auto data = std::vector<uint8_t>{};
    for (auto& input : inputs) {
        encode32(input.sequence, data);
    }
    auto hash = TW::Hash::sha256(TW::Hash::sha256(data));
    return hash;
}

std::vector<uint8_t> Transaction::getOutputsHash() const {
    auto data = std::vector<uint8_t>{};
    for (auto& output : outputs) {
        output.encode(data);
    }
    auto hash = TW::Hash::sha256(TW::Hash::sha256(data));
    return hash;
}


void Transaction::encode(bool witness, std::vector<uint8_t>& data) const {
    encode32(version, data);

    if (witness) {
        // Use extended format in case witnesses are to be serialized.
        data.push_back(0);
        data.push_back(1);
    }

    Bitcoin::writeCompactSize(inputs.size(), data);
    for (auto& input : inputs) {
        input.encode(data);
    }

    Bitcoin::writeCompactSize(outputs.size(), data);
    for (auto& output : outputs) {
        output.encode(data);
    }

    if (witness) {
        for (auto& input : inputs) {
            input.encodeWitness(data);
        }
    }

    encode32(lockTime, data);
}

std::vector<uint8_t> Transaction::getSignatureHash(const Script& scriptCode, size_t index, uint32_t hashType, uint64_t amount, TWBitcoinSignatureVersion version) const {
    switch (version) {
    case BASE:
        return getSignatureHashBase(scriptCode, index, hashType);
    case WITNESS_V0:
        return getSignatureHashWitnessV0(scriptCode, index, hashType, amount);
    }
}

/// Generates the signature hash for Witness version 0 scripts.
std::vector<uint8_t> Transaction::getSignatureHashWitnessV0(const Script& scriptCode, size_t index, uint32_t hashType, uint64_t amount) const {
    auto preimage = getPreImage(scriptCode, index, hashType, amount);
    auto hash = TW::Hash::sha256(TW::Hash::sha256(preimage));
    return hash;
}

/// Generates the signature hash for for scripts other than witness scripts.
std::vector<uint8_t> Transaction::getSignatureHashBase(const Script& scriptCode, size_t index, uint32_t hashType) const {
    assert(index < inputs.size());

    auto data = std::vector<uint8_t>{};

    encode32(version, data);

    auto serializedInputCount = (hashType & TWSignatureHashTypeAnyoneCanPay) != 0 ? 1 : inputs.size();
    Bitcoin::writeCompactSize(serializedInputCount, data);
    for (auto subindex = 0; subindex < serializedInputCount; subindex += 1) {
        serializeInput(subindex, scriptCode, index, hashType, data);
    }

    auto hashNone = (hashType & 0x1f) == TWSignatureHashTypeNone;
    auto hashSingle = (hashType & 0x1f) == TWSignatureHashTypeSingle;
    auto serializedOutputCount = hashNone ? 0 : (hashSingle ? index+1 : outputs.size());
    Bitcoin::writeCompactSize(serializedOutputCount, data);
    for (auto subindex = 0; subindex < serializedOutputCount; subindex += 1) {
        if (hashSingle && subindex != index) {
            auto output = TransactionOutput(-1, {});
            output.encode(data);
        } else {
            outputs[subindex].encode(data);
        }
    }

    // Locktime
    encode32(lockTime, data);

    // Sighash type
    encode32(hashType, data);

    auto hash = TW::Hash::sha256(TW::Hash::sha256(data));
    return hash;
}

void Transaction::serializeInput(size_t subindex, const Script& scriptCode, size_t index, uint32_t hashType, std::vector<uint8_t>& data) const {
    // In case of SIGHASH_ANYONECANPAY, only the input being signed is serialized
    if ((hashType & TWSignatureHashTypeAnyoneCanPay) != 0) {
        subindex = index;
    }

    reinterpret_cast<const TW::Bitcoin::OutPoint&>(inputs[subindex].previousOutput).encode(data);

    // Serialize the script
    if (subindex != index) {
        Bitcoin::writeCompactSize(0, data);
    } else {
        scriptCode.encode(data);
    }

    // Serialize the nSequence
    auto hashNone = (hashType & 0x1f) == TWSignatureHashTypeNone;
    auto hashSingle = (hashType & 0x1f) == TWSignatureHashTypeSingle;
    if (subindex != index && (hashSingle || hashNone)) {
        encode32(0, data);
    } else {
        encode32(inputs[subindex].sequence, data);
    }
}

Proto::Transaction Transaction::proto() const {
    auto protoTx = Proto::Transaction();
    protoTx.set_version(version);
    protoTx.set_locktime(lockTime);

    for (const auto& input : inputs) {
        auto protoInput = protoTx.add_inputs();
        protoInput->mutable_previousoutput()->set_hash(input.previousOutput.hash.data(), input.previousOutput.hash.size());
        protoInput->mutable_previousoutput()->set_index(input.previousOutput.index);
        protoInput->set_sequence(input.sequence);
        protoInput->set_script(input.script.bytes.data(), input.script.bytes.size());
    }

    for (const auto& output : outputs) {
        auto protoOutput = protoTx.add_outputs();
        protoOutput->set_value(output.value);
        protoOutput->set_script(output.script.bytes.data(), output.script.bytes.size());
    }

    return protoTx;
}
