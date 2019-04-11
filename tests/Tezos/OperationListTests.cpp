// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Tezos/Address.h"
#include "Tezos/BinaryCoding.h"
#include "Tezos/OperationList.h"
#include "proto/Tezos.pb.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

using namespace TW::Tezos;
using namespace TW::Tezos::Proto;

TEST(TezosOperationList, ForgeBranch) {
    auto input = TW::Tezos::OperationList("BMNY6Jkas7BzKb7wDLCFoQ4YxfYoieU7Xmo1ED3Y9Lo3ZvVGdgW");
    auto expected = "da8eb4f57f98a647588b47d29483d1edfdbec1428c11609cee0da6e0f27cfc38";

    ASSERT_EQ(input.forgeBranch(), parse_hex(expected));
}

TEST(TezosOperationList, ForgeOperationList_TransactionOnly) {
    auto branch = "BL8euoCWqNCny9AR3AKjnpi38haYMxjei1ZqNHuXMn19JSQnoWp";
    auto op_list = TW::Tezos::OperationList(branch);

    auto transactionOperationData = new TW::Tezos::Proto::TransactionOperationData();
    transactionOperationData -> set_amount(1);
    transactionOperationData -> set_destination("tz1Yju7jmmsaUiG9qQLoYv35v5pHgnWoLWbt");

    auto transactionOperation = TW::Tezos::Proto::Operation();
    transactionOperation.set_source("tz1XVJ8bZUXs7r5NV8dHvuiBhzECvLRLR3jW");
    transactionOperation.set_fee(1272);
    transactionOperation.set_counter(30738);
    transactionOperation.set_gas_limit(10100);
    transactionOperation.set_storage_limit(257);
    transactionOperation.set_kind(TW::Tezos::Proto::Operation::TRANSACTION);
    transactionOperation.set_allocated_transaction_operation_data(transactionOperationData);

    op_list.addOperation(transactionOperation);

    auto expected = "3756ef37b1be849e3114643f0aa5847cabf9a896d3bfe4dd51448de68e91da0108000081faa75f741ef614b0e35fcc8c90dfa3b0b95721f80992f001f44e81020100008fb5cea62d147c696afd9a93dbce962f4c8a9c9100";

    ASSERT_EQ(op_list.forge(), parse_hex(expected));
}

TEST(TezosOperationList, ForgeOperationList_RevealOnly) {
      auto branch = "BL8euoCWqNCny9AR3AKjnpi38haYMxjei1ZqNHuXMn19JSQnoWp";
      auto op_list = TW::Tezos::OperationList(branch);

      PublicKey publicKey = parsePublicKey("edpku9ZF6UUAEo1AL3NWy1oxHLL6AfQcGYwA5hFKrEKVHMT3Xx889A");

      auto revealOperationData = new TW::Tezos::Proto::RevealOperationData();
      revealOperationData -> set_public_key(publicKey.bytes.data(), publicKey.bytes.size());

      auto revealOperation = TW::Tezos::Proto::Operation();
      revealOperation.set_source("tz1XVJ8bZUXs7r5NV8dHvuiBhzECvLRLR3jW");
      revealOperation.set_fee(1272);
      revealOperation.set_counter(30738);
      revealOperation.set_gas_limit(10100);
      revealOperation.set_storage_limit(257);
      revealOperation.set_kind(TW::Tezos::Proto::Operation::REVEAL);
      revealOperation.set_allocated_reveal_operation_data(revealOperationData);

      op_list.addOperation(revealOperation);
      auto expected = "3756ef37b1be849e3114643f0aa5847cabf9a896d3bfe4dd51448de68e91da0107000081faa75f741ef614b0e35fcc8c90dfa3b0b95721f80992f001f44e810200429a986c8072a40a1f3a3e2ab5a5819bb1b2fb69993c5004837815b9dc55923e";
      ASSERT_EQ(op_list.forge(), parse_hex(expected));
}

TEST(TezosOperationList, ForgeOperationList_OriginationOnly) {
    auto branch = "BLoCyPwBk3XoS5jLbKNzvYPe4RwFbnDdMSLqvsbhMmxopcPfAhP";
    auto op_list = TW::Tezos::OperationList(branch);

    auto originationOperationData = new TW::Tezos::Proto::OriginationOperationData();
    originationOperationData -> set_manager_pubkey("tz1XVJ8bZUXs7r5NV8dHvuiBhzECvLRLR3jW");
    originationOperationData -> set_balance(0);

    auto originationOperation = TW::Tezos::Proto::Operation();
    originationOperation.set_source("tz1XVJ8bZUXs7r5NV8dHvuiBhzECvLRLR3jW");
    originationOperation.set_fee(1285);
    originationOperation.set_counter(30871);
    originationOperation.set_gas_limit(10000);
    originationOperation.set_storage_limit(257);
    originationOperation.set_kind(TW::Tezos::Proto::Operation::ORIGINATION);
    originationOperation.set_allocated_origination_operation_data(originationOperationData);
    
    op_list.addOperation(originationOperation);

    auto expected = "8ee0e04e6e66717f5b580bd494f1c00b73c171f5ebd85e0ef4c1dbc4def1f6f109000081faa75f741ef614b0e35fcc8c90dfa3b0b95721850a97f101904e81020081faa75f741ef614b0e35fcc8c90dfa3b0b9572100ffff0000";
    ASSERT_EQ(hex(op_list.forge()), hex(parse_hex(expected)));
}

TEST(TezosOperationList, ForgeOperationList_Delegation_ClearDelegate) {
    auto branch = "BLGJfQDFEYZBRLj5GSHskj8NPaRYhk7Kx5WAfdcDucD3q98WdeW";
    auto op_list = TW::Tezos::OperationList(branch);

    auto delegationOperationData = new TW::Tezos::Proto::DelegationOperationData();
    delegationOperationData -> set_delegate("");

    auto delegationOperation = TW::Tezos::Proto::Operation();
    delegationOperation.set_source("KT1D5jmrBD7bDa3jCpgzo32FMYmRDdK2ihka");
    delegationOperation.set_fee(1257);
    delegationOperation.set_counter(67);
    delegationOperation.set_gas_limit(10000);
    delegationOperation.set_storage_limit(0);
    delegationOperation.set_kind(TW::Tezos::Proto::Operation::DELEGATION);
    delegationOperation.set_allocated_delegation_operation_data(delegationOperationData);

    op_list.addOperation(delegationOperation);

    auto expected = "48b63d801fa824013a195f7885ba522503c59e0580f7663e15c52f03ccc935e60a01315b1206ec00b1b1e64cc3b8b93059f58fa2fc3900e90943904e0000";
    ASSERT_EQ(hex(op_list.forge()), hex(parse_hex(expected)));
}

TEST(TezosOperationList, ForgeOperationList_Delegation_AddDelegate) {
    auto branch = "BLa4GrVQTxUgQWbHv6cF7RXWSGzHGPbgecpQ795R3cLzw4cGfpD";
    auto op_list = TW::Tezos::OperationList(branch);

    auto delegationOperationData = new TW::Tezos::Proto::DelegationOperationData();
    delegationOperationData -> set_delegate("tz1dYUCcrorfCoaQCtZaxi1ynGrP3prTZcxS");

    auto delegationOperation = TW::Tezos::Proto::Operation();
    delegationOperation.set_source("KT1D5jmrBD7bDa3jCpgzo32FMYmRDdK2ihka");
    delegationOperation.set_fee(1257);
    delegationOperation.set_counter(68);
    delegationOperation.set_gas_limit(10000);
    delegationOperation.set_storage_limit(0);
    delegationOperation.set_kind(TW::Tezos::Proto::Operation::DELEGATION);
    delegationOperation.set_allocated_delegation_operation_data(delegationOperationData);
    
    op_list.addOperation(delegationOperation);

    auto expected = "7105102c032807994dd9b5edf219261896a559876ca16cbf9d31dbe3612b89f20a01315b1206ec00b1b1e64cc3b8b93059f58fa2fc3900e90944904e00ff00c4650fd609f88c67356e5fe01e37cd3ff654b18c";
    ASSERT_EQ(hex(op_list.forge()), hex(parse_hex(expected)));
}

TEST(TezosOperationList, ForgeOperationList_TransactionAndReveal) {
    auto branch = "BL8euoCWqNCny9AR3AKjnpi38haYMxjei1ZqNHuXMn19JSQnoWp";
    auto op_list = TW::Tezos::OperationList(branch);
    
    PublicKey publicKey = parsePublicKey("edpku9ZF6UUAEo1AL3NWy1oxHLL6AfQcGYwA5hFKrEKVHMT3Xx889A");

    auto revealOperationData = new TW::Tezos::Proto::RevealOperationData();
    revealOperationData -> set_public_key(publicKey.bytes.data(), publicKey.bytes.size());

    auto revealOperation = TW::Tezos::Proto::Operation();
    revealOperation.set_source("tz1XVJ8bZUXs7r5NV8dHvuiBhzECvLRLR3jW");
    revealOperation.set_fee(1272);
    revealOperation.set_counter(30738);
    revealOperation.set_gas_limit(10100);
    revealOperation.set_storage_limit(257);
    revealOperation.set_kind(TW::Tezos::Proto::Operation::REVEAL);
    revealOperation.set_allocated_reveal_operation_data(revealOperationData);
    
    auto transactionOperationData = new TW::Tezos::Proto::TransactionOperationData();
    transactionOperationData -> set_amount(1);
    transactionOperationData -> set_destination("tz1XVJ8bZUXs7r5NV8dHvuiBhzECvLRLR3jW");

    auto transactionOperation = TW::Tezos::Proto::Operation();
    transactionOperation.set_source("tz1XVJ8bZUXs7r5NV8dHvuiBhzECvLRLR3jW");
    transactionOperation.set_fee(1272);
    transactionOperation.set_counter(30739);
    transactionOperation.set_gas_limit(10100);
    transactionOperation.set_storage_limit(257);
    transactionOperation.set_kind(TW::Tezos::Proto::Operation::TRANSACTION);
    transactionOperation.set_allocated_transaction_operation_data(transactionOperationData);

    op_list.addOperation(revealOperation);
    op_list.addOperation(transactionOperation);

    auto expected = "3756ef37b1be849e3114643f0aa5847cabf9a896d3bfe4dd51448de68e91da0107000081faa75f741ef614b0e35fcc8c90dfa3b0b95721f80992f001f44e810200429a986c8072a40a1f3a3e2ab5a5819bb1b2fb69993c5004837815b9dc55923e08000081faa75f741ef614b0e35fcc8c90dfa3b0b95721f80993f001f44e810201000081faa75f741ef614b0e35fcc8c90dfa3b0b9572100";
    ASSERT_EQ(op_list.forge(), parse_hex(expected));
}
