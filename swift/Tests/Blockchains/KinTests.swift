// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import XCTest
import WalletCore

class KinTests: XCTestCase {

    func testSigner() {
        let account = "GAE2SZV4VLGBAPRYRFV2VY7YYLYGYIP5I7OU7BSP6DJT7GAZ35OKFDYI"
        let destination = "GDCYBNRRPIHLHG7X7TKPUPAZ7WVUXCN3VO7WCCK64RIFV5XM5V5K4A52"
        let privateKeyData = Data(hexString: "59a313f46ef1c23a9e4f71cea10fc0c56a2a6bb8a4b9ea3d5348823e5a478722")!

        let input = StellarSigningInput.with {
            $0.passphrase = StellarPassphrase.kin.description
            $0.amount = 10_000_000
            $0.fee = 1000
            $0.sequence = 2 // from account info api
            $0.account = account
            $0.destination = destination
            $0.privateKey = privateKeyData
            $0.operationType = .payment
        }

        let output: StellarSigningOutput = AnySigner.sign(input: input, coin: .kin)
        XCTAssertEqual(output.signature, "AAAAAAmpZryqzBA+OIlrquP4wvBsIf1H3U+GT/DTP5gZ31yiAAAD6AAAAAAAAAACAAAAAAAAAAAAAAABAAAAAAAAAAEAAAAAxYC2MXoOs5v3/NT6PBn9q0uJu6u/YQle5FBa9uzteq4AAAAAAAAAAACYloAAAAAAAAAAARnfXKIAAABA9tSWyvGnmF7maZ+en5bRu5MurhQw9yDU/U0bKFZN0Og3Px8UKlQuFIGnddh47/rOM8AYtqpQbcbvWgKhS31DAw==")
    }
}
