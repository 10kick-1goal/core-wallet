// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "PublicKey.h"
#include "Data.h"

#include <TrezorCrypto/ecdsa.h>
#include <TrezorCrypto/ed25519-donna/ed25519-blake2b.h>
#include <TrezorCrypto/nist256p1.h>
#include <TrezorCrypto/secp256k1.h>
#include <TrezorCrypto/sodium/keypair.h>

namespace TW {

/// Determines if a collection of bytes makes a valid public key of the
/// given type.
bool PublicKey::isValid(const Data& data, enum TWPublicKeyType type) {
    const auto size = data.size();
    if (size == 0) {
        return false;
    }
    switch (type) {
    case TWPublicKeyTypeED25519:
        return size == ed25519Size || (size == ed25519Size + 1 && data[0] == 0x01);
    case TWPublicKeyTypeCURVE25519:
    case TWPublicKeyTypeED25519Blake2b:
        return size == ed25519Size;
    case TWPublicKeyTypeED25519Extended:
        return size == ed25519ExtendedSize;
    case TWPublicKeyTypeSECP256k1:
    case TWPublicKeyTypeNIST256p1:
        return size == secp256k1Size && (data[0] == 0x02 || data[0] == 0x03);
    case TWPublicKeyTypeSECP256k1Extended:
    case TWPublicKeyTypeNIST256p1Extended:
        return size == secp256k1ExtendedSize && data[0] == 0x04;
    default:
        return false;
    }
}

/// Initializes a public key with a collection of bytes.
///
/// @throws std::invalid_argument if the data is not a valid public key.
PublicKey::PublicKey(const Data& data, enum TWPublicKeyType type) : type(type) {
    if (!isValid(data, type)) {
        throw std::invalid_argument("Invalid public key data");
    }
    switch (type) {
    case TWPublicKeyTypeSECP256k1:
    case TWPublicKeyTypeNIST256p1:
    case TWPublicKeyTypeSECP256k1Extended:
    case TWPublicKeyTypeNIST256p1Extended:
        bytes.reserve(data.size());
        std::copy(std::begin(data), std::end(data), std::back_inserter(bytes));
        break;

    case TWPublicKeyTypeED25519:
    case TWPublicKeyTypeCURVE25519:
        bytes.reserve(ed25519Size);
        if (data.size() == ed25519Size + 1) {
            std::copy(std::begin(data) + 1, std::end(data), std::back_inserter(bytes));
        } else {
            std::copy(std::begin(data), std::end(data), std::back_inserter(bytes));
        }
        break;
    case TWPublicKeyTypeED25519Blake2b:
        bytes.reserve(ed25519Size);
        if (data.size() == ed25519Size + 1) {
            std::copy(std::begin(data) + 1, std::end(data), std::back_inserter(bytes));
        } else {
            std::copy(std::begin(data), std::end(data), std::back_inserter(bytes));
        }
        break;
    case TWPublicKeyTypeED25519Extended:
        bytes.reserve(ed25519ExtendedSize);
        std::copy(std::begin(data), std::end(data), std::back_inserter(bytes));
    }
}

PublicKey PublicKey::compressed() const {
    if (type != TWPublicKeyTypeSECP256k1Extended && type != TWPublicKeyTypeNIST256p1Extended) {
        return *this;
    }

    Data newBytes(secp256k1Size);
    newBytes[0] = 0x02 | (bytes[64] & 0x01);

    switch (type) {
    case TWPublicKeyTypeSECP256k1Extended:
        std::copy(bytes.begin() + 1, bytes.begin() + secp256k1Size, newBytes.begin() + 1);
        return PublicKey(newBytes, TWPublicKeyTypeSECP256k1);
    case TWPublicKeyTypeNIST256p1Extended:
        std::copy(bytes.begin() + 1, bytes.begin() + secp256k1Size, newBytes.begin() + 1);
        return PublicKey(newBytes, TWPublicKeyTypeNIST256p1);
    default:
        return *this;
    }
}

PublicKey PublicKey::extended() const {
    Data newBytes(secp256k1ExtendedSize);
    switch (type) {
    case TWPublicKeyTypeSECP256k1:
        ecdsa_uncompress_pubkey(&secp256k1, bytes.data(), newBytes.data());
        return PublicKey(newBytes, TWPublicKeyTypeSECP256k1Extended);
    case TWPublicKeyTypeSECP256k1Extended:
        return *this;
    case TWPublicKeyTypeNIST256p1:
        ecdsa_uncompress_pubkey(&nist256p1, bytes.data(), newBytes.data());
        return PublicKey(newBytes, TWPublicKeyTypeNIST256p1Extended);
    case TWPublicKeyTypeNIST256p1Extended:
        return *this;
    case TWPublicKeyTypeED25519:
    case TWPublicKeyTypeCURVE25519:
    case TWPublicKeyTypeED25519Blake2b:
    case TWPublicKeyTypeED25519Extended:
       return *this;
    }
}

bool PublicKey::verify(const Data& signature, const Data& message) const {
    switch (type) {
    case TWPublicKeyTypeSECP256k1:
    case TWPublicKeyTypeSECP256k1Extended:
        return ecdsa_verify_digest(&secp256k1, bytes.data(), signature.data(), message.data()) == 0;
    case TWPublicKeyTypeNIST256p1:
    case TWPublicKeyTypeNIST256p1Extended:
        return ecdsa_verify_digest(&nist256p1, bytes.data(), signature.data(), message.data()) == 0;
    case TWPublicKeyTypeED25519:
        return ed25519_sign_open(message.data(), message.size(), bytes.data(), signature.data()) == 0;
    case TWPublicKeyTypeED25519Blake2b:
        return ed25519_sign_open_blake2b(message.data(), message.size(), bytes.data(), signature.data()) == 0;
    case TWPublicKeyTypeED25519Extended:
        throw std::logic_error("Not yet implemented");
        //ed25519_sign_open(message.data(), message.size(), bytes.data(), signature.data()) == 0;
    case TWPublicKeyTypeCURVE25519:
        auto ed25519PublicKey = Data();
        ed25519PublicKey.resize(PublicKey::ed25519Size);
        curve25519_pk_to_ed25519(ed25519PublicKey.data(), bytes.data());

        ed25519PublicKey[31] &= 0x7F;
        ed25519PublicKey[31] |= signature[63] & 0x80;

        // remove sign bit
        auto verifyBuffer = Data();
        append(verifyBuffer, signature);
        verifyBuffer[63] &= 127;
        return ed25519_sign_open(message.data(), message.size(), ed25519PublicKey.data(),
                                 verifyBuffer.data()) == 0;
    }
}

bool PublicKey::verifySchnorr(const Data& signature, const Data& message) const {
    switch (type) {
    case TWPublicKeyTypeSECP256k1:
    case TWPublicKeyTypeSECP256k1Extended:
        return zil_schnorr_verify(&secp256k1, bytes.data(), signature.data(), message.data(), static_cast<uint32_t>(message.size())) == 0;
    case TWPublicKeyTypeNIST256p1:
    case TWPublicKeyTypeNIST256p1Extended:
        return false;
    case TWPublicKeyTypeED25519:
    case TWPublicKeyTypeED25519Blake2b:
    case TWPublicKeyTypeED25519Extended:
    case TWPublicKeyTypeCURVE25519:
    default:
        return false;
    }
}

Data PublicKey::hash(const Data& prefix, Hash::Hasher hasher, bool skipTypeByte) const {
    const auto offset = std::size_t(skipTypeByte ? 1 : 0);
    const auto hash = hasher(bytes.data() + offset, bytes.data() + bytes.size());

    auto result = Data();
    result.reserve(prefix.size() + hash.size());
    append(result, prefix);
    append(result, hash);
    return result;
}

} // namespace TW
