// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <gtest/gtest.h>
#include <vector>
#include "boost/format.hpp"

#include "Elrond/TransactionFactory.h"
#include "TestAccounts.h"

using namespace TW;
using namespace TW::Elrond;

TEST(ElrondTransactionFactory, fromEGLDTransfer) {
    auto input = Proto::SigningInput();
    input.mutable_egld_transfer()->mutable_accounts()->set_sender(ALICE_BECH32);
    input.mutable_egld_transfer()->mutable_accounts()->set_receiver(BOB_BECH32);
    input.mutable_egld_transfer()->set_amount("1000000000000000000");

    TransactionFactory factory;
    Transaction transaction = factory.fromEGLDTransfer(input);

    ASSERT_EQ(ALICE_BECH32, transaction.sender);
    ASSERT_EQ(BOB_BECH32, transaction.receiver);
    ASSERT_EQ("", transaction.data);
    ASSERT_EQ("1000000000000000000", transaction.value);
    ASSERT_EQ(50000, transaction.gasLimit);
    ASSERT_EQ(1000000000, transaction.gasPrice);
    ASSERT_EQ("1", transaction.chainID);
    ASSERT_EQ(1, transaction.version);
}

TEST(ElrondTransactionFactory, fromESDTTransfer) {
    auto input = Proto::SigningInput();
    input.mutable_esdt_transfer()->mutable_accounts()->set_sender_nonce(7);
    input.mutable_esdt_transfer()->mutable_accounts()->set_sender(ALICE_BECH32);
    input.mutable_esdt_transfer()->mutable_accounts()->set_receiver(BOB_BECH32);
    input.mutable_esdt_transfer()->set_token_identifier("MYTOKEN-1234");
    input.mutable_esdt_transfer()->set_amount("10000000000000");

    TransactionFactory factory;
    Transaction transaction = factory.fromESDTTransfer(input);

    ASSERT_EQ(ALICE_BECH32, transaction.sender);
    ASSERT_EQ(BOB_BECH32, transaction.receiver);
    ASSERT_EQ("ESDTTransfer@4d59544f4b454e2d31323334@09184e72a000", transaction.data);
    ASSERT_EQ("0", transaction.value);
    ASSERT_EQ(425000, transaction.gasLimit);
    ASSERT_EQ(1000000000, transaction.gasPrice);
    ASSERT_EQ("1", transaction.chainID);
    ASSERT_EQ(1, transaction.version);
}

TEST(ElrondTransactionFactory, fromESDTNFTTransfer) {
    auto input = Proto::SigningInput();
    input.mutable_esdtnft_transfer()->mutable_accounts()->set_sender_nonce(7);
    input.mutable_esdtnft_transfer()->mutable_accounts()->set_sender(ALICE_BECH32);
    input.mutable_esdtnft_transfer()->mutable_accounts()->set_receiver(BOB_BECH32);
    input.mutable_esdtnft_transfer()->set_token_collection("LKMEX-aab910");
    input.mutable_esdtnft_transfer()->set_token_nonce(4);
    input.mutable_esdtnft_transfer()->set_amount("184300000000000000");

    TransactionFactory factory;
    Transaction transaction = factory.fromESDTNFTTransfer(input);

    ASSERT_EQ(ALICE_BECH32, transaction.sender);
    ASSERT_EQ(ALICE_BECH32, transaction.receiver);
    ASSERT_EQ("ESDTNFTTransfer@4c4b4d45582d616162393130@04@028ec3dfa01ac000@8049d639e5a6980d1cd2392abcce41029cda74a1563523a202f09641cc2618f8", transaction.data);
    ASSERT_EQ("0", transaction.value);
    ASSERT_EQ(937500, transaction.gasLimit);
    ASSERT_EQ(1000000000, transaction.gasPrice);
    ASSERT_EQ("1", transaction.chainID);
    ASSERT_EQ(1, transaction.version);
}

TEST(ElrondTransactionFactory, createTransfersWithProvidedNetworkConfig) {
    NetworkConfig networkConfig;

    // Set dummy values:
    networkConfig.setChainId("T");
    networkConfig.setMinGasPrice(1500000000);
    networkConfig.setMinGasLimit(60000);
    networkConfig.setGasPerDataByte(2000);
    networkConfig.setGasCostESDTTransfer(300000);
    networkConfig.setGasCostESDTNFTTransfer(300000);

    Proto::SigningInput signingInputWithEGLDTransfer;
    signingInputWithEGLDTransfer.mutable_egld_transfer()->mutable_accounts()->set_sender(ALICE_BECH32);
    signingInputWithEGLDTransfer.mutable_egld_transfer()->mutable_accounts()->set_receiver(BOB_BECH32);
    signingInputWithEGLDTransfer.mutable_egld_transfer()->set_amount("0");

    Proto::SigningInput signingInputWithESDTTransfer;
    signingInputWithESDTTransfer.mutable_esdt_transfer()->mutable_accounts()->set_sender(ALICE_BECH32);
    signingInputWithESDTTransfer.mutable_esdt_transfer()->mutable_accounts()->set_receiver(BOB_BECH32);
    signingInputWithESDTTransfer.mutable_esdt_transfer()->set_token_identifier("MYTOKEN-1234");
    signingInputWithESDTTransfer.mutable_esdt_transfer()->set_amount("10000000000000");

    Proto::SigningInput signingInputWithESDTNFTTransfer;
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->mutable_accounts()->set_sender(ALICE_BECH32);
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->mutable_accounts()->set_receiver(BOB_BECH32);
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->set_token_collection("LKMEX-aab910");
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->set_token_nonce(4);
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->set_amount("184300000000000000");

    TransactionFactory factory(networkConfig);
    Transaction tx1 = factory.fromEGLDTransfer(signingInputWithEGLDTransfer);
    Transaction tx2 = factory.fromESDTTransfer(signingInputWithESDTTransfer);
    Transaction tx3 = factory.fromESDTNFTTransfer(signingInputWithESDTNFTTransfer);
    
    ASSERT_EQ(60000, tx1.gasLimit);
    ASSERT_EQ(1500000000, tx1.gasPrice);
    ASSERT_EQ("T", tx1.chainID);

    ASSERT_EQ(560000, tx2.gasLimit);
    ASSERT_EQ(1500000000, tx2.gasPrice);
    ASSERT_EQ("T", tx2.chainID);

    ASSERT_EQ(1110000, tx3.gasLimit);
    ASSERT_EQ(1500000000, tx3.gasPrice);
    ASSERT_EQ("T", tx3.chainID);
}

TEST(ElrondTransactionFactory, createTransfersWithOverriddenNetworkParameters) {
    Proto::SigningInput signingInputWithEGLDTransfer;
    signingInputWithEGLDTransfer.set_gas_limit(50500);
    signingInputWithEGLDTransfer.set_gas_price(1000000001);
    signingInputWithEGLDTransfer.set_chain_id("A");

    Proto::SigningInput signingInputWithESDTTransfer;
    signingInputWithESDTTransfer.set_gas_limit(5000000);
    signingInputWithESDTTransfer.set_gas_price(1000000002);
    signingInputWithESDTTransfer.set_chain_id("B");

    Proto::SigningInput signingInputWithESDTNFTTransfer;
    signingInputWithESDTNFTTransfer.set_gas_limit(10000000);
    signingInputWithESDTNFTTransfer.set_gas_price(1000000003);
    signingInputWithESDTNFTTransfer.set_chain_id("C");

    TransactionFactory factory;
    Transaction tx1 = factory.fromEGLDTransfer(signingInputWithEGLDTransfer);
    Transaction tx2 = factory.fromESDTTransfer(signingInputWithESDTTransfer);
    Transaction tx3 = factory.fromESDTNFTTransfer(signingInputWithESDTNFTTransfer);
    
    ASSERT_EQ(50500, tx1.gasLimit);
    ASSERT_EQ(1000000001, tx1.gasPrice);
    ASSERT_EQ("A", tx1.chainID);

    ASSERT_EQ(5000000, tx2.gasLimit);
    ASSERT_EQ(1000000002, tx2.gasPrice);
    ASSERT_EQ("B", tx2.chainID);

    ASSERT_EQ(10000000, tx3.gasLimit);
    ASSERT_EQ(1000000003, tx3.gasPrice);
    ASSERT_EQ("C", tx3.chainID);
}

TEST(ElrondTransactionFactory, create) {
    Proto::SigningInput signingInputWithGenericAction;
    signingInputWithGenericAction.mutable_generic_action()->set_data("hello");

    Proto::SigningInput signingInputWithEGLDTransfer;
    signingInputWithEGLDTransfer.mutable_egld_transfer()->mutable_accounts()->set_sender(ALICE_BECH32);
    signingInputWithEGLDTransfer.mutable_egld_transfer()->mutable_accounts()->set_receiver(BOB_BECH32);
    signingInputWithEGLDTransfer.mutable_egld_transfer()->set_amount("1");

    Proto::SigningInput signingInputWithESDTTransfer;
    signingInputWithESDTTransfer.mutable_esdt_transfer()->mutable_accounts()->set_sender(ALICE_BECH32);
    signingInputWithESDTTransfer.mutable_esdt_transfer()->mutable_accounts()->set_receiver(BOB_BECH32);
    signingInputWithESDTTransfer.mutable_esdt_transfer()->set_token_identifier("MYTOKEN-1234");
    signingInputWithESDTTransfer.mutable_esdt_transfer()->set_amount("10000000000000");

    Proto::SigningInput signingInputWithESDTNFTTransfer;
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->mutable_accounts()->set_sender(ALICE_BECH32);
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->mutable_accounts()->set_receiver(BOB_BECH32);
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->set_token_collection("LKMEX-aab910");
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->set_token_nonce(4);
    signingInputWithESDTNFTTransfer.mutable_esdtnft_transfer()->set_amount("184300000000000000");

    TransactionFactory factory;
    Transaction tx1 = factory.create(signingInputWithGenericAction);
    Transaction tx2 = factory.create(signingInputWithEGLDTransfer);
    Transaction tx3 = factory.create(signingInputWithESDTTransfer);
    Transaction tx4 = factory.create(signingInputWithESDTNFTTransfer);
    
    ASSERT_EQ("hello", tx1.data);
    ASSERT_EQ("1", tx2.value);
    ASSERT_EQ("ESDTTransfer@4d59544f4b454e2d31323334@09184e72a000", tx3.data);
    ASSERT_EQ("ESDTNFTTransfer@4c4b4d45582d616162393130@04@028ec3dfa01ac000@8049d639e5a6980d1cd2392abcce41029cda74a1563523a202f09641cc2618f8", tx4.data);
}
