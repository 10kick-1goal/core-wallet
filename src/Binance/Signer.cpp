// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Signer.h"
#include "Serialization.h"
#include "../Hash.h"
#include "../HexCoding.h"
#include "../PrivateKey.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <string>

using namespace TW;
using namespace TW::Binance;

// Message prefixes
// see https://docs.binance.org/api-reference/transactions.html#amino-types
static const auto sendOrderPrefix = Data{0x2A, 0x2C, 0x87, 0xFA};
static const auto tradeOrderPrefix = Data{0xCE, 0x6D, 0xC0, 0x43};
static const auto cancelTradeOrderPrefix = Data{0x16, 0x6E, 0x68, 0x1B};
static const auto HTLTOrderPrefix = Data{0xB3, 0x3F, 0x9A, 0x24};
static const auto depositHTLTOrderPrefix = Data{0x63, 0x98, 0x64, 0x96};
static const auto claimHTLTOrderPrefix = Data{0xC1, 0x66, 0x53, 0x00};
static const auto refundHTLTOrderPrefix = Data{0x34, 0x54, 0xA2, 0x7C};
static const auto pubKeyPrefix = Data{0xEB, 0x5A, 0xE9, 0x87};
static const auto transactionPrefix = Data{0xF0, 0x62, 0x5D, 0xEE};
static const auto tokenIssueOrderPrefix = Data{0x17, 0xEF, 0xAB, 0x80};
static const auto tokenMintOrderPrefix = Data{0x46, 0x7E, 0x08, 0x29};
static const auto tokenBurnOrderPrefix = Data{0x7E, 0xD2, 0xD2, 0xA0};
static const auto tokenFreezeOrderPrefix = Data{0xE7, 0x74, 0xB3, 0x2D};
static const auto tokenUnfreezeOrderPrefix = Data{0x65, 0x15, 0xFF, 0x0D};

Data Signer::build() const {
    auto signature = encodeSignature(sign());
    return encodeTransaction(signature);
}

Data Signer::sign() const {
    auto key = PrivateKey(input.private_key());
    auto hash = Hash::sha256(signaturePreimage());
    auto signature = key.sign(hash, TWCurveSECP256k1);
    return Data(signature.begin(), signature.end() - 1);
}

std::string Signer::signaturePreimage() const {
    auto json = signatureJSON(input);
    return json.dump();
}

Data Signer::encodeTransaction(const Data& signature) const {
    auto msg = encodeOrder();
    auto transaction = Binance::Proto::Transaction();
    transaction.add_msgs(msg.data(), msg.size());
    transaction.add_signatures(signature.data(), signature.size());
    transaction.set_memo(input.memo());
    transaction.set_source(input.source());

    auto data = transaction.SerializeAsString();
    return aminoWrap(data, transactionPrefix, true);
}

Data Signer::encodeOrder() const {
    std::string data;
    Data prefix;
    if (input.has_trade_order()) {
        data = input.trade_order().SerializeAsString();
        prefix = tradeOrderPrefix;
    } else if (input.has_cancel_trade_order()) {
        data = input.cancel_trade_order().SerializeAsString();
        prefix = cancelTradeOrderPrefix;
    } else if (input.has_send_order()) {
        data = input.send_order().SerializeAsString();
        prefix = sendOrderPrefix;
    } else if (input.has_issue_order()) {
        data = input.issue_order().SerializeAsString();
        prefix = tokenIssueOrderPrefix;
    } else if (input.has_mint_order()) {
        data = input.mint_order().SerializeAsString();
        prefix = tokenMintOrderPrefix;
    } else if (input.has_burn_order()) {
        data = input.burn_order().SerializeAsString();
        prefix = tokenBurnOrderPrefix;
    } else if (input.has_freeze_order()) {
        data = input.freeze_order().SerializeAsString();
        prefix = tokenFreezeOrderPrefix;
    } else if (input.has_unfreeze_order()) {
        data = input.unfreeze_order().SerializeAsString();
        prefix = tokenUnfreezeOrderPrefix;
    } else if (input.has_htlt_order()) {
        data = input.htlt_order().SerializeAsString();
        prefix = HTLTOrderPrefix;
    } else if (input.has_deposithtlt_order()) {
        data = input.deposithtlt_order().SerializeAsString();
        prefix = depositHTLTOrderPrefix;
    } else if (input.has_claimhtlt_order()) {
        data = input.claimhtlt_order().SerializeAsString();
        prefix = claimHTLTOrderPrefix;
    } else if (input.has_refundhtlt_order()) {
        data = input.refundhtlt_order().SerializeAsString();
        prefix = refundHTLTOrderPrefix;
    } else {
        return {};
    }
    return aminoWrap(data, prefix, false);
}

Data Signer::encodeSignature(const Data& signature) const {
    auto key = PrivateKey(input.private_key());
    auto publicKey = key.getPublicKey(TWPublicKeyTypeSECP256k1);

    auto encodedPublicKey = pubKeyPrefix;
    encodedPublicKey.insert(encodedPublicKey.end(), static_cast<uint8_t>(publicKey.bytes.size()));
    encodedPublicKey.insert(encodedPublicKey.end(), publicKey.bytes.begin(), publicKey.bytes.end());

    auto object = Binance::Proto::Signature();
    object.set_pub_key(encodedPublicKey.data(), encodedPublicKey.size());
    object.set_signature(signature.data(), signature.size());
    object.set_account_number(input.account_number());
    object.set_sequence(input.sequence());

    return aminoWrap(object.SerializeAsString(), {}, false);
}

Data Signer::aminoWrap(const std::string& raw, const Data& typePrefix, bool prefixWithSize) const {
    const auto contentsSize = raw.size() + typePrefix.size();
    auto size = contentsSize;
    if (prefixWithSize) {
        size += 10;
    }

    std::string msg;
    msg.reserve(size);
    {
        google::protobuf::io::StringOutputStream output(&msg);
        google::protobuf::io::CodedOutputStream cos(&output);
        if (prefixWithSize) {
            cos.WriteVarint64(contentsSize);
        }
        cos.WriteRaw(typePrefix.data(), static_cast<int>(typePrefix.size()));
        cos.WriteRaw(raw.data(), static_cast<int>(raw.size()));
    }

    return Data(msg.begin(), msg.end());
}
