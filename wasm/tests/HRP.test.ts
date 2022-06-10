// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import "mocha";
import { assert } from "chai";
import { WalletCore } from "../dist";

describe("HRP", () => {
  it("test string value", () => {
    const { HRP, describeHRP } = WalletCore;

    assert.equal(describeHRP(HRP.bitcoin), "bc");
    assert.equal(describeHRP(HRP.binance), "bnb");

  });
});
