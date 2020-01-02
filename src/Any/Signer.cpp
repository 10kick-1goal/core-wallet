// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Signer.h"
#include "Binance/Signer.h"
#include "Cosmos/Signer.h"
#include "Data.h"
#include "Ethereum/Signer.h"
#include "Harmony/Signer.h"
#include "HexCoding.h"
#include "IoTeX/Signer.h"
#include "Nano/Signer.h"
#include "Nebulas/Signer.h"
#include "PrivateKey.h"
#include "Tezos/Signer.h"
#include "Tron/Signer.h"
#include "VeChain/Signer.h"
#include "Wanchain/Signer.h"
#include "Waves/Signer.h"
#include "Stellar/Signer.h"
#include "Bitcoin/Transaction.h"
#include "Bitcoin/TransactionSigner.h"
#include "Solana/Signer.h"

#include <google/protobuf/util/json_util.h>
#include <string>

#include <TrustWalletCore/TWCoinType.h>

using namespace TW;
using namespace google::protobuf;

TW::Any::Proto::SigningOutput TW::Any::Signer::sign() const noexcept {
    const auto coinType = (TWCoinType)input.coin_type();
    const auto transaction = input.transaction();
    const auto privateKey = PrivateKey(parse_hex(input.private_key()));

    auto output = TW::Any::Proto::SigningOutput();

    switch (coinType) {
    case TWCoinTypeCosmos: {
        Cosmos::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signerOutput = Cosmos::Signer(std::move(message)).build();
            output.set_output(signerOutput.json());
        }
        break;
    }
    case TWCoinTypeBinance: {
        Binance::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signerOutput = Binance::Signer(std::move(message)).build();
            output.set_output(hex(signerOutput.begin(), signerOutput.end()));
        }
        break;
    }
    case TWCoinTypeTomoChain:
    case TWCoinTypeCallisto:
    case TWCoinTypeThunderToken:
    case TWCoinTypePOANetwork:
    case TWCoinTypeEthereumClassic:
    case TWCoinTypeEthereum: {
        Ethereum::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signerOutput = Ethereum::Signer(load(message.chain_id())).sign(message);
            auto encoded = signerOutput.encoded();
            output.set_output(hex(encoded.begin(), encoded.end()));
        }
        break;
    }
    case TWCoinTypeTezos: {
        Tezos::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto operations = message.operation_list().operations();
            auto operation_list = Tezos::OperationList(message.operation_list().branch());
            operation_list.operation_list.assign(operations.begin(), operations.end());
            auto signerOutput = Tezos::Signer().signOperationList(privateKey, operation_list);
            output.set_output(hex(signerOutput.begin(), signerOutput.end()));
        }
        break;
    }
    case TWCoinTypeIoTeX: {
        IoTeX::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_privatekey(privateKey.bytes.data(), privateKey.bytes.size());
            auto signerOutput = IoTeX::Signer(std::move(message)).build();
            auto encoded = signerOutput.encoded();
            output.set_output(hex(encoded.begin(), encoded.end()));
        }
        break;
    }
    case TWCoinTypeWanchain: {
        Ethereum::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signerOutput = Wanchain::Signer(load(message.chain_id())).sign(message);
            auto encoded = signerOutput.encoded();
            output.set_output(hex(encoded.begin(), encoded.end()));
        }
        break;
    }
    case TWCoinTypeWaves: {
        Waves::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            auto publicKey = privateKey.getPublicKey(TWPublicKeyTypeCURVE25519);
            auto wavesTransaction = Waves::Transaction(std::move(message), publicKey.bytes);
            auto signature = Waves::Signer::sign(privateKey, wavesTransaction);
            auto jsonOutput = wavesTransaction.buildJson(signature).dump();
            output.set_output(jsonOutput);
        }
        break;
    }
    case TWCoinTypeNebulas: {
        Nebulas::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signerOutput = Nebulas::Signer(load(message.chain_id())).sign(message);
            auto signature = signerOutput.signature();
            output.set_output(hex(signature.begin(), signature.end()));
        }
        break;
    }
    case TWCoinTypeTron: {
        Tron::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signerOutput = Tron::Signer::sign(message);
            output.set_output(signerOutput.json());
        }
        break;
    }
    case TWCoinTypeVeChain: {
        VeChain::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signerOutput = VeChain::Signer::sign(message);
            auto encoded = signerOutput.encoded();
            output.set_output(hex(encoded.begin(), encoded.end()));
        }
        break;
    }
    case TWCoinTypeHarmony: {
        Harmony::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signerOutput = Harmony::Signer::sign(message);
            auto encoded = signerOutput.encoded();
            output.set_output(hex(encoded.begin(), encoded.end()));
        }
        break;
    }
    case TWCoinTypeNano: {
        Nano::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signer = Nano::Signer(message);
            // only signature is included (signer.blockHash not)
            auto signature = signer.sign();
            output.set_output(hex(signature.begin(), signature.end()));
        }
        break;
    }
    case TWCoinTypeStellar: {
        Stellar::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
                message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
                auto signer = Stellar::Signer(message);
                auto signerOutput = signer.sign();
                output.set_output(signerOutput);
        }
        break;
    }
    case TWCoinTypeSolana: {
        Solana::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
                message.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
                auto signerOutput = Solana::Signer::signProtobuf(message);
                auto encoded = signerOutput.encoded();
                output.set_output(hex(encoded.begin(), encoded.end()));
        }
        break;
    }
    case TWCoinTypeBitcoinCash:
    case TWCoinTypeLitecoin:
    case TWCoinTypeBitcoin: {
        Bitcoin::Proto::SigningInput message;
        parse(transaction, &message, output);
        if (output.success()) {
            message.add_private_key(privateKey.bytes.data(), privateKey.bytes.size());
            auto signer =
                    Bitcoin::TransactionSigner<Bitcoin::Transaction, Bitcoin::TransactionBuilder>(std::move(message));
            auto signerOutput = signer.sign();
            auto signedTx = signerOutput.payload();
            Data serialized;
            signedTx.encode(true, serialized);
            output.set_output(hex(serialized));
        }
        break;
    }
    default:
        auto error = new Proto::SigningOutput_Error();
        error->set_code(SignerErrorCodeNotSupported);
        error->set_description("Network not supported");
        output.set_allocated_error(error);
    }

    return output;
}

void TW::Any::Signer::parse(const std::string &transaction, Message *message,
                        TW::Any::Proto::SigningOutput &output) const noexcept {
    util::JsonParseOptions options;
    options.case_insensitive_enum_parsing = true;
    options.ignore_unknown_fields = false;

    auto result = JsonStringToMessage(transaction, message, options);

    if (result.ok()) {
        output.set_success(true);
        return;
    }

    auto error = new TW::Any::Proto::SigningOutput_Error();
    error->set_code(SignerErrorCodeInvalidJson);
    error->set_description(result.error_message());
    output.set_allocated_error(error);
}

void TW::Any::Signer::toJson(const google::protobuf::Message &message, std::string *json_string) const
    noexcept {
    util::JsonPrintOptions options;
    options.preserve_proto_field_names = true;

    MessageToJsonString(message, json_string, options);
}
