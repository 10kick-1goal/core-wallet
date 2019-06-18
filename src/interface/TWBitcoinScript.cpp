// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWBitcoinScript.h>

#include "../Bitcoin/Script.h"

using namespace TW::Bitcoin;

struct TWBitcoinScript *_Nonnull TWBitcoinScriptCreate() {
    auto script = new TWBitcoinScript{};
    return script;
}

struct TWBitcoinScript *TWBitcoinScriptCreateWithData(TWData *data) {
    auto script = new TWBitcoinScript{};
    script->impl.bytes.resize(TWDataSize(data));
    TWDataCopyBytes(data, 0, TWDataSize(data), script->impl.bytes.data());
    return script;
}

struct TWBitcoinScript *_Nonnull TWBitcoinScriptCreateWithBytes(uint8_t *_Nonnull bytes, size_t size) {
    auto script = new TWBitcoinScript{};
    std::copy(bytes, bytes + size, std::back_inserter(script->impl.bytes));
    return script;
}

struct TWBitcoinScript *TWBitcoinScriptCreateCopy(const struct TWBitcoinScript *script) {
    auto newScript = new TWBitcoinScript{};
    newScript->impl.bytes = script->impl.bytes;
    return newScript;
}

void TWBitcoinScriptDelete(struct TWBitcoinScript *script) {
    delete script;
}

size_t TWBitcoinScriptSize(const struct TWBitcoinScript *script) {
    return script->impl.bytes.size();
}

TWData *TWBitcoinScriptData(const struct TWBitcoinScript *script) {
    return TWDataCreateWithBytes(&script->impl.bytes[0], script->impl.bytes.size());
}

TWData *TWBitcoinScriptScriptHash(const struct TWBitcoinScript *_Nonnull script) {
    auto result = script->impl.hash();
    return TWDataCreateWithBytes(result.data(), result.size());
}

bool TWBitcoinScriptIsPayToScriptHash(const struct TWBitcoinScript *script) {
    return script->impl.isPayToScriptHash();
}

bool TWBitcoinScriptIsPayToWitnessScriptHash(const struct TWBitcoinScript *script) {
    return script->impl.isPayToWitnessScriptHash();
}

bool TWBitcoinScriptIsWitnessProgram(const struct TWBitcoinScript *script) {
    return script->impl.isWitnessProgram();
}

bool TWBitcoinScriptEqual(const struct TWBitcoinScript *_Nonnull lhs, const struct TWBitcoinScript *_Nonnull rhs) {
    return lhs->impl.bytes == rhs->impl.bytes;
}

TWData *TWBitcoinScriptMatchPayToPubkey(const struct TWBitcoinScript *script) {
    std::vector<uint8_t> data;
    if (script->impl.matchPayToPubkey(data)) {
        return TWDataCreateWithBytes(data.data(), data.size());
    }
    return nullptr;
}

TWData *TWBitcoinScriptMatchPayToPubkeyHash(const struct TWBitcoinScript *script) {
    std::vector<uint8_t> data;
    if (script->impl.matchPayToPubkeyHash(data)) {
        return TWDataCreateWithBytes(data.data(), data.size());
    }
    return nullptr;
}

TWData *_Nullable TWBitcoinScriptMatchPayToScriptHash(const struct TWBitcoinScript *script) {
    std::vector<uint8_t> data;
    if (script->impl.matchPayToScriptHash(data)) {
        return TWDataCreateWithBytes(data.data(), data.size());
    }
    return nullptr;
}

TWData *_Nullable TWBitcoinScriptMatchPayToWitnessPublicKeyHash(const struct TWBitcoinScript *script) {
    std::vector<uint8_t> data;
    if (script->impl.matchPayToWitnessPublicKeyHash(data)) {
        return TWDataCreateWithBytes(data.data(), data.size());
    }
    return nullptr;
}

TWData *_Nullable TWBitcoinScriptMatchPayToWitnessScriptHash(const struct TWBitcoinScript *script) {
    std::vector<uint8_t> data;
    if (script->impl.matchPayToWitnessScriptHash(data)) {
        return TWDataCreateWithBytes(data.data(), data.size());
    }
    return nullptr;
}

TWData *TWBitcoinScriptEncode(const struct TWBitcoinScript *script) {
    auto result = std::vector<uint8_t>{};
    script->impl.encode(result);
    return TWDataCreateWithBytes(result.data(), result.size());
}

struct TWBitcoinScript *TWBitcoinScriptBuildPayToPublicKeyHash(TWData *hash) {
    auto v = reinterpret_cast<const std::vector<uint8_t>*>(hash);
    auto script = Script::buildPayToPublicKeyHash(*v);
    return new TWBitcoinScript{ .impl = script };
}

struct TWBitcoinScript *TWBitcoinScriptBuildPayToScriptHash(TWData *scriptHash) {
    auto v = reinterpret_cast<const std::vector<uint8_t>*>(scriptHash);
    auto script = Script::buildPayToScriptHash(*v);
    return new TWBitcoinScript{ .impl = script };
}

struct TWBitcoinScript *TWBitcoinScriptBuildPayToWitnessPubkeyHash(TWData *hash) {
    auto v = reinterpret_cast<const std::vector<uint8_t>*>(hash);
    auto script = Script::buildPayToWitnessPubkeyHash(*v);
    return new TWBitcoinScript{ .impl = script };
}

struct TWBitcoinScript *TWBitcoinScriptBuildPayToWitnessScriptHash(TWData *scriptHash) {
    auto v = reinterpret_cast<const std::vector<uint8_t>*>(scriptHash);
    auto script = Script::buildPayToWitnessScriptHash(*v);
    return new TWBitcoinScript{ .impl = script };
}

struct TWBitcoinScript *_Nonnull TWBitcoinScriptBuildForAddress(TWString *_Nonnull address, enum TWCoinType coin) {
    auto s = reinterpret_cast<const std::string*>(address);
    auto script = Script::buildForAddress(*s, coin);
    return new TWBitcoinScript{ .impl = script };
}
