// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import "mocha";
import { assert } from "chai";
import { Buffer } from "buffer";
import { WalletCore } from "../dist";

describe("Hash", () => {
  it("test keccak256", () => {
    const { Hash, HexCoding } = WalletCore;

    const sha3Hash = Hash.keccak256(Buffer.from("Test keccak-256"));

    assert.equal(
      HexCoding.encode(sha3Hash),
      "0x9aeb50f48121c80b2ff73ad48b5f197d940f748d936d35c992367370c1abfb18"
    );
  });

  it("test sha256", () => {
    const { Hash, HexCoding } = WalletCore;

    const sha256Hash = Hash.sha256(Buffer.from("Test hash"));
    assert.equal(
      HexCoding.encode(sha256Hash),
      "0xf250fc8f40aeea3297c0158ec1bfa07b503805f2a822530bd63c50625bb9376b"
    );
  });

  it("test sha512_256", () => {
    const { Hash, HexCoding } = WalletCore;

    const hash = Hash.sha512_256(Buffer.from("hello"));
    assert.equal(
      HexCoding.encode(hash),
      "0xe30d87cfa2a75db545eac4d61baf970366a8357c7f72fa95b52d0accb698f13a"
    );
  });
});
