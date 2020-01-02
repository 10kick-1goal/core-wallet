// Copyright © 2019 Mart Roosmaa.
// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Signer.h"
#include "../BinaryCoding.h"
#include "../Hash.h"
#include "../HexCoding.h"

#include <boost/multiprecision/cpp_int.hpp>

using namespace TW;

using uint128_t = boost::multiprecision::uint128_t;

namespace TW::Nano {

const std::array<byte, 32> kBlockHashPreamble{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06,
};

std::array<byte, 16> store(const uint128_t& value) {
    using boost::multiprecision::cpp_int;

    Data buf;
    buf.reserve(16);
    export_bits(value, std::back_inserter(buf), 8);

    // Zero-pad the bytes
    while (buf.size() < 16) {
        buf.insert(buf.begin(), 0);
    }

    std::array<byte, 16> arr = {0};
    std::copy_n(buf.begin(), arr.size(), arr.begin());
    return arr;
}

std::array<byte, 32> hashBlockData(const PublicKey& publicKey, const Proto::SigningInput& input) {
    std::array<byte, 32> parentHash = {0};
    if (input.parent_block().size() != 0) {
        if (input.parent_block().size() != parentHash.size()) {
            throw std::invalid_argument("Invalid parent block hash");
        }
        std::copy_n(input.parent_block().begin(), parentHash.size(), parentHash.begin());
    }
    bool emptyParentHash = std::all_of(parentHash.begin(), parentHash.end(), [](auto b) { return b == 0; });

    std::array<byte, 32> repPublicKey = {0};
    auto repAddress = Address(input.representative());
    std::copy_n(repAddress.bytes.begin(), repPublicKey.size(), repPublicKey.begin());

    uint128_t balance_uint;
    try {
        balance_uint = uint128_t(input.balance());
    } catch (const std::runtime_error&) {
        throw std::invalid_argument("Invalid balance");
    }
    bool zeroBalance = balance_uint == uint128_t(0);
    std::array<byte, 16> balance = store(balance_uint);
    if (emptyParentHash && zeroBalance) {
        throw std::invalid_argument("Invalid balance");
    }

    std::array<byte, 32> link = {0};
    switch (input.link_oneof_case()) {
        case Proto::SigningInput::kLinkBlock: {
            if (input.link_block().size() != link.size()) {
                throw std::invalid_argument("Invalid link block hash");
            }
            std::copy_n(input.link_block().begin(), link.size(), link.begin());
            break;
        }
        case Proto::SigningInput::kLinkRecipient: {
            if (!emptyParentHash) {
                auto toAddress = Address(input.link_recipient());
                std::copy_n(toAddress.bytes.begin(), link.size(), link.begin());
            }
            break;
        }
        case Proto::SigningInput::LINK_ONEOF_NOT_SET: break;
    }
    bool emptyLink = std::all_of(link.begin(), link.end(), [](auto b) { return b == 0; });
    if (emptyParentHash && emptyLink) {
        throw std::invalid_argument("Missing link block hash");
    }

    auto msg = Data();
    msg.insert(msg.end(), kBlockHashPreamble.begin(), kBlockHashPreamble.end());
    msg.insert(msg.end(), publicKey.bytes.begin(), publicKey.bytes.end());
    msg.insert(msg.end(), parentHash.begin(), parentHash.end());
    msg.insert(msg.end(), repPublicKey.begin(), repPublicKey.end());
    msg.insert(msg.end(), balance.begin(), balance.end());
    msg.insert(msg.end(), link.begin(), link.end());

    std::array<byte, 32> blockHash = {0};
    auto digest = Hash::blake2b(msg, blockHash.size());
    std::copy_n(digest.begin(), blockHash.size(), blockHash.begin());

    return blockHash;
}

Signer::Signer(const Proto::SigningInput& input)
  : privateKey(Data(input.private_key().begin(), input.private_key().end())),
    publicKey(privateKey.getPublicKey(TWPublicKeyTypeED25519Blake2b)),
    blockHash(hashBlockData(publicKey, input))
{}

std::array<byte, 64> Signer::sign() const noexcept {
    auto digest = Data(blockHash.begin(), blockHash.end());
    auto sig = privateKey.sign(digest, TWCurveED25519Blake2bNano);

    std::array<byte, 64> signature = {0};
    std::copy_n(sig.begin(), signature.size(), signature.begin());
    return signature;
}

}
