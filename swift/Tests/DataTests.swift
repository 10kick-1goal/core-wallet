// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import WalletCore
import XCTest

class DataTests: XCTestCase {
    func testCreateWithBytes() {
        let bytes = [0x1, 0x2, 0x3, 0x4] as [UInt8]
        let twdata = bytes.withUnsafeBufferPointer({ ptr in TWDataCreateWithBytes(ptr.baseAddress!, 4) })
        let data = Data.fromTWData(twdata)

        XCTAssertEqual(Array(data), bytes)
    }

    func testOddLength() {
        XCTAssertNil(Data(hexString: "0x0"))
        XCTAssertNil(Data(hexString: "0x28fa6ae00"))
        XCTAssertNil(Data(hexString: "28fa6ae00"))
    }
}
