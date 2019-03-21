// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import TrustWalletCore
import XCTest

class PrivateKeyTests: XCTestCase {
    func testCreateNew() {
        let privateKey = PrivateKey()

        XCTAssertEqual(privateKey.data.count, PrivateKey.size)
        XCTAssertTrue(PrivateKey.isValid(data: privateKey.data))
    }

    func testCreateFromInvalid() {
        let privateKey = PrivateKey(data: Data(hexString: "0xdeadbeef")!)
        XCTAssertNil(privateKey)
    }

    func testIsValidString() {
        let valid = PrivateKey.isValid(data: Data(hexString: "afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5")!)

        XCTAssert(valid)
    }

    func testPublicKey() {
        let privateKey = PrivateKey(data: Data(hexString: "afeefca74d9a325cf1d6b6911d61a65c32afa8e02bd5e78e2e4ac2910bab45f5")!)!
        let publicKey = privateKey.getPublicKeySecp256k1(compressed: false)

        XCTAssertEqual(publicKey.data.hexString, "0499c6f51ad6f98c9c583f8e92bb7758ab2ca9a04110c0a1126ec43e5453d196c166b489a4b7c491e7688e6ebea3a71fc3a1a48d60f98d5ce84c93b65e423fde91")
    }
}
