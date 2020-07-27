// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <gtest/gtest.h>
#include <vector>
#include "boost/format.hpp"

#include "HexCoding.h"
#include "Elrond/Serialization.h"
#include "TestAccounts.h"

using namespace TW;
using namespace TW::Elrond;

TEST(ElrondSerialization, SignableString) {
    Proto::TransactionMessage message;
    message.set_nonce(42);
    message.set_value("43");
    message.set_sender("alice");
    message.set_receiver("bob");
    message.set_data("foo");
    message.set_chain_id("1");
    message.set_version(1);

    string jsonString = serializeTransaction(message);
    ASSERT_EQ(R"({"nonce":42,"value":"43","receiver":"bob","sender":"alice","gasPrice":0,"gasLimit":0,"data":"Zm9v","chainID":"1","version":1})", jsonString);
}

TEST(ElrondSerialization, SignableStringWithRealData) {
    Proto::TransactionMessage message;
    message.set_nonce(15);
    message.set_value("100");
    message.set_sender(ALICE_BECH32);
    message.set_receiver(BOB_BECH32);
    message.set_gas_price(1000000000);
    message.set_gas_limit(50000);
    message.set_data("foo");
    message.set_chain_id("1");
    message.set_version(1);

    string expected = (boost::format(R"({"nonce":15,"value":"100","receiver":"%1%","sender":"%2%","gasPrice":1000000000,"gasLimit":50000,"data":"Zm9v","chainID":"1","version":1})") % BOB_BECH32 % ALICE_BECH32).str();
    string actual = serializeTransaction(message);
    ASSERT_EQ(expected, actual);
}

TEST(ElrondSerialization, SignableStringWithoutData) {
    Proto::TransactionMessage message;
    message.set_nonce(42);
    message.set_value("43");
    message.set_sender("feed");
    message.set_receiver("abba");
    message.set_chain_id("1");
    message.set_version(1);

    string jsonString = serializeTransaction(message);
    ASSERT_EQ(R"({"nonce":42,"value":"43","receiver":"abba","sender":"feed","gasPrice":0,"gasLimit":0,"chainID":"1","version":1})", jsonString);
}
