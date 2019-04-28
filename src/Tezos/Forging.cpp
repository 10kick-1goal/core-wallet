// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"
#include "BinaryCoding.h"
#include "../Base58.h"
#include "../Data.h"
#include "../HexCoding.h"
#include "../proto/Tezos.pb.h"

#include <sstream>

using namespace TW;
using namespace TW::Tezos;
using namespace TW::Tezos::Proto;

// Forge the given boolean into a hex encoded string.
Data forgeBool(bool input) {
    unsigned char result = input ? 0xff : 0x00;
    return Data{result};
}

// Forge the given public key hash into a hex encoded string.
// Note: This function supports tz1, tz2 and tz3 addresses.
Data forgePublicKeyHash(const std::string& publicKeyHash) {
    Data forged = Data();
    // Adjust prefix based on tz1, tz2 or tz3.
    switch ((char)publicKeyHash[2]) {
    case '1':
        forged.push_back(0x00);
        break;
    case '2':
        forged.push_back(0x01);
        break;
    case '3':
        forged.push_back(0x02);
        break;
    default:
        throw std::invalid_argument("Invalid Prefix");
    }
    const auto decoded = Base58::bitcoin.decodeCheck(publicKeyHash);
    const auto prefixSize = 3;
    forged.insert(forged.end(), decoded.begin() + prefixSize, decoded.end());
    return forged;
}

// Forge the given public key into a hex encoded string.
Data forgePublicKey(PublicKey publicKey) {
    std::array<uint8_t, 4> prefix = {13, 15, 37, 217};
    auto data = Data(prefix.begin(), prefix.end());
    auto bytes = Data(publicKey.bytes.begin(), publicKey.bytes.end());
    append(data, bytes);

    auto pk = Base58::bitcoin.encodeCheck(data);
    auto decoded = base58ToHex(pk, 4, prefix.data());
    return parse_hex(decoded);
}

// Forge the given zarith hash into a hex encoded string.
Data forgeZarith(uint64_t input) {
    Data forged = Data();
    while (input > 0x80) {
        forged.push_back(static_cast<byte>((input & 0xff) | 0x80));
        input >>= 7;
    }
    forged.push_back(static_cast<byte>(input));
    return forged;
}

// Forge the given operation.
Data forgeOperation(const Operation& operation) {
  auto forged = Data();
  auto source = Address(operation.source());
  auto forgedSource = source.forge();
  auto forgedFee = forgeZarith(operation.fee());
  auto forgedCounter = forgeZarith(operation.counter());
  auto forgedGasLimit = forgeZarith(operation.gas_limit());
  auto forgedStorageLimit = forgeZarith(operation.storage_limit());

  if (operation.kind() == Operation_OperationKind_REVEAL) {
      auto publicKey = PublicKey(operation.reveal_operation_data().public_key(), TWPublicKeyTypeED25519);
      forged.push_back(0x07);
      append(forged, forgedSource);
      append(forged, forgedFee);
      append(forged, forgedCounter);
      append(forged, forgedGasLimit);
      append(forged, forgedStorageLimit);
      append(forged, forgePublicKey(publicKey));
      return forged;
  } else if (operation.kind() == Operation_OperationKind_ORIGINATION) {
      auto managerPublicKey = operation.origination_operation_data().manager_pubkey();
      auto balance = operation.origination_operation_data().balance();
      forged.push_back(0x09);
      append(forged, forgedSource);
      append(forged, forgedFee);
      append(forged, forgedCounter);
      append(forged, forgedGasLimit);
      append(forged, forgedStorageLimit);
      append(forged, forgePublicKeyHash(managerPublicKey));
      append(forged, forgeZarith(balance));
      append(forged, forgeBool(true));
      append(forged, forgeBool(true));
      append(forged, forgeBool(false));
      append(forged, forgeBool(false));
      return forged;
  } else if (operation.kind() == Operation_OperationKind_DELEGATION) {
      auto delegate = operation.delegation_operation_data().delegate();
      forged.push_back(0x0a);
      append(forged, forgedSource);
      append(forged, forgedFee);
      append(forged, forgedCounter);
      append(forged, forgedGasLimit);
      append(forged, forgedStorageLimit);
      if (!delegate.empty()) {
          append(forged, forgeBool(true));
          append(forged, forgePublicKeyHash(delegate));
      } else {
          append(forged, forgeBool(false));
      }
      return forged;
  } else {
      auto forgedAmount = forgeZarith(operation.transaction_operation_data().amount());
      auto forgedDestination = Address(operation.transaction_operation_data().destination()).forge();
      forged.push_back(0x08);
      append(forged, forgedSource);
      append(forged, forgedFee);
      append(forged, forgedCounter);
      append(forged, forgedGasLimit);
      append(forged, forgedStorageLimit);
      append(forged, forgedAmount);
      append(forged, forgedDestination);
      append(forged, forgeBool(false));
      return forged;
  }
  return Data();
}
