// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "PrivateKey.h"

#include "PublicKey.h"

#include <TrezorCrypto/bignum.h>
#include <TrezorCrypto/ecdsa.h>
#include <TrezorCrypto/ed25519-donna/ed25519-blake2b.h>
#include <TrezorCrypto/memzero.h>
#include <TrezorCrypto/nist256p1.h>
#include <TrezorCrypto/rand.h>
#include <TrezorCrypto/schnorr.h>
#include <TrezorCrypto/secp256k1.h>
#include <TrezorCrypto/sodium/keypair.h>

using namespace TW;

bool PrivateKey::isValid(const Data& data, TWCurve curve)
{
    // check size
    bool valid = isValid(data);
    if (!valid) {
        return false;
    }

    const ecdsa_curve *ec_curve = nullptr;
    switch (curve)
    {
    case TWCurveSECP256k1:
        ec_curve = &secp256k1;
        break;
    case TWCurveNIST256p1:
        ec_curve = &nist256p1;
        break;
    case TWCurveED25519:
    case TWCurveED25519Blake2bNano:
    case TWCurveCurve25519:
        break;
    }

    if (ec_curve != nullptr) {
        bignum256 k;
        bn_read_be(data.data(), &k);
        if (!bn_is_less(&k, &ec_curve->order)) {
            memzero(&k, sizeof(k));
            return false;
        };
    }

    return true;
}

PrivateKey::~PrivateKey() {
    std::fill(bytes.begin(), bytes.end(), 0);
}

PublicKey PrivateKey::getPublicKey(TWPublicKeyType type) const {
    Data result;
    switch (type) {
    case TWPublicKeyTypeSECP256k1:
        result.resize(PublicKey::secp256k1Size);
        ecdsa_get_public_key33(&secp256k1, bytes.data(), result.data());
        break;
    case TWPublicKeyTypeSECP256k1Extended:
        result.resize(PublicKey::secp256k1ExtendedSize);
        ecdsa_get_public_key65(&secp256k1, bytes.data(), result.data());
        break;
    case TWPublicKeyTypeNIST256p1:
        result.resize(PublicKey::secp256k1Size);
        ecdsa_get_public_key33(&nist256p1, bytes.data(), result.data());
        break;
    case TWPublicKeyTypeNIST256p1Extended:
        result.resize(PublicKey::secp256k1ExtendedSize);
        ecdsa_get_public_key65(&nist256p1, bytes.data(), result.data());
        break;
    case TWPublicKeyTypeED25519:
        result.resize(PublicKey::ed25519Size);
        ed25519_publickey(bytes.data(), result.data());
        break;
    case TWPublicKeyTypeED25519Blake2b:
        result.resize(PublicKey::ed25519Size);
        ed25519_publickey_blake2b(bytes.data(), result.data());
        break;
    case TWPublicKeyTypeCURVE25519:
        result.resize(PublicKey::ed25519Size);
        PublicKey ed25519PublicKey = getPublicKey(TWPublicKeyTypeED25519);
        ed25519_pk_to_curve25519(result.data(), ed25519PublicKey.bytes.data());
        break;
    }
    return PublicKey(result, type);
}

Data PrivateKey::sign(const Data& digest, TWCurve curve) const {
    Data result;
    bool success = true;
    switch (curve) {
    case TWCurveSECP256k1: {
        result.resize(65);
        success = ecdsa_sign_digest(&secp256k1, bytes.data(), digest.data(), result.data(),
                                    result.data() + 64, nullptr) == 0;
    } break;
    case TWCurveED25519: {
        result.resize(64);
        const auto publicKey = getPublicKey(TWPublicKeyTypeED25519);
        ed25519_sign(digest.data(), digest.size(), bytes.data(), publicKey.bytes.data(), result.data());
    } break;
    case TWCurveED25519Blake2bNano: {
        result.resize(64);
        const auto publicKey = getPublicKey(TWPublicKeyTypeED25519Blake2b);
        ed25519_sign_blake2b(digest.data(), digest.size(), bytes.data(),
                             publicKey.bytes.data(), result.data());
    } break;
    case TWCurveCurve25519: {
        result.resize(64);
        const auto publicKey = getPublicKey(TWPublicKeyTypeED25519);
        ed25519_sign(digest.data(), digest.size(), bytes.data(), publicKey.bytes.data(),
                     result.data());
        const auto sign_bit = publicKey.bytes[31] & 0x80;
        result[63] = result[63] & 127;
        result[63] |= sign_bit;
    } break;
    case TWCurveNIST256p1: {
        result.resize(65);
        success = ecdsa_sign_digest(&nist256p1, bytes.data(), digest.data(), result.data(),
                                    result.data() + 64, nullptr) == 0;
    } break;
    }

    if (!success) {
        return {};
    }
    return result;
}

Data PrivateKey::sign(const Data& digest, TWCurve curve, int(*canonicalChecker)(uint8_t by, uint8_t sig[64])) const {
    Data result;
    bool success = false;
    switch (curve) {
    case TWCurveSECP256k1: {
        result.resize(65);
        success = ecdsa_sign_digest(&secp256k1, bytes.data(), digest.data(), result.data() + 1,
                                    result.data(), canonicalChecker) == 0;
    } break;
    case TWCurveED25519: // not supported
    case TWCurveED25519Blake2bNano: // not supported
    case TWCurveCurve25519:         // not supported
        break;
    case TWCurveNIST256p1: {
        result.resize(65);
        success = ecdsa_sign_digest(&nist256p1, bytes.data(), digest.data(), result.data() + 1,
                                    result.data(), canonicalChecker) == 0;
    } break;
    }

    if (!success) {
        return {};
    }

    // graphene adds 31 to the recovery id
    result[0] += 31;
    return result;
}

Data PrivateKey::signAsDER(const Data& digest, TWCurve curve) const {
    Data sig(64);
    bool success =
        ecdsa_sign_digest(&secp256k1, bytes.data(), digest.data(), sig.data(), nullptr, nullptr) == 0;
    if (!success) {
        return {};
    }

    std::array<uint8_t, 72> resultBytes;
    size_t size = ecdsa_sig_to_der(sig.data(), resultBytes.data());

    auto result = Data{};
    std::copy(resultBytes.begin(), resultBytes.begin() + size, std::back_inserter(result));
    return result;
}

Data PrivateKey::signSchnorr(const Data& message, TWCurve curve) const {
    bool success = false;
    Data sig(64);
    switch (curve) {
    case TWCurveSECP256k1: {
        success = zil_schnorr_sign(&secp256k1, bytes.data(), message.data(), static_cast<uint32_t>(message.size()), sig.data()) == 0;
    } break;

    case TWCurveNIST256p1:
    case TWCurveED25519:
    case TWCurveED25519Blake2bNano:
    case TWCurveCurve25519: {
        // not support
    } break;
    }
    if (!success) {
        return {};
    }
    return sig;
}
