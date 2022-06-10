// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import "mocha";
import { assert } from "chai";
import { WalletCore } from "../dist";
import { Buffer } from "buffer";

describe("HDWallet", () => {

  it("test creating 24 words", () => {
    const { HDWallet, Mnemonic } = WalletCore;

    var wallet = HDWallet.create(256, "password");
    const mnemonic = wallet.mnemonic();

    assert.equal(mnemonic.split(" ").length, 24);
    assert.isTrue(Mnemonic.isValid(mnemonic));

    wallet.delete();
  });

  it("test deriving Ethereum address", () => {
    const { HDWallet, CoinType } = WalletCore;

    var wallet = HDWallet.createWithMnemonic("ripple scissors kick mammal hire column oak again sun offer wealth tomorrow wagon turn fatal", "TREZOR");
    const address = wallet.getAddressForCoin(CoinType.ethereum);

    assert.equal(address, "0x27Ef5cDBe01777D62438AfFeb695e33fC2335979");

    wallet.delete();
  });
});
