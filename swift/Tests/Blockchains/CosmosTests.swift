// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

import XCTest
import WalletCore

class CosmosAddressTests: XCTestCase {
    func testAddressValidation() {
        let cosmos = CoinType.cosmos
        for address in [
            "cosmos1hsk6jryyqjfhp5dhc55tc9jtckygx0eph6dd02",
            "cosmospub1addwnpepqftjsmkr7d7nx4tmhw4qqze8w39vjq364xt8etn45xqarlu3l2wu2n7pgrq",
            "cosmosvaloper1sxx9mszve0gaedz5ld7qdkjkfv8z992ax69k08",
            "cosmosvalconspub1zcjduepqjnnwe2jsywv0kfc97pz04zkm7tc9k2437cde2my3y5js9t7cw9mstfg3sa",
        ] {
            XCTAssertTrue(cosmos.validate(address: address))
            XCTAssertEqual(cosmos.address(string: address)?.description, address)
        }
    }
}

class CosmosSignerTests: XCTestCase {

    let privateKey = PrivateKey(data: Data(hexString: "80e81ea269e66a0a05b11236df7919fb7fbeedba87452d667489d7403a02f005")!)!

    func testSigningTransaction() {
        let publicKey = privateKey.getPublicKeySecp256k1(compressed: true)
        let fromAddress = AnyAddress(publicKey: publicKey, coin: .cosmos)

        let sendCoinsMessage = CosmosMessage.Send.with {
            $0.fromAddress = fromAddress.description
            $0.toAddress = "cosmos1zt50azupanqlfam5afhv3hexwyutnukeh4c573"
            $0.amounts = [CosmosAmount.with {
                $0.amount = "1"
                $0.denom = "muon"
            }]
        }

        let message = CosmosMessage.with {
            $0.sendCoinsMessage = sendCoinsMessage
        }

        let fee = CosmosFee.with {
            $0.gas = 200000
            $0.amounts = [CosmosAmount.with {
                $0.amount = "200"
                $0.denom = "muon"
            }]
        }

        let input = CosmosSigningInput.with {
            $0.signingMode = .protobuf;
            $0.accountNumber = 1037
            $0.chainID = "gaia-13003"
            $0.memo = ""
            $0.sequence = 8
            $0.messages = [message]
            $0.fee = fee
            $0.privateKey = privateKey.data
        }

        let output: CosmosSigningOutput = AnySigner.sign(input: input, coin: .cosmos)

        XCTAssertJSONEqual(output.serialized, "{\"tx_bytes\": \"CowBCokBChwvY29zbW9zLmJhbmsudjFiZXRhMS5Nc2dTZW5kEmkKLWNvc21vczFoc2s2anJ5eXFqZmhwNWRoYzU1dGM5anRja3lneDBlcGg2ZGQwMhItY29zbW9zMXp0NTBhenVwYW5xbGZhbTVhZmh2M2hleHd5dXRudWtlaDRjNTczGgkKBG11b24SATESZQpQCkYKHy9jb3Ntb3MuY3J5cHRvLnNlY3AyNTZrMS5QdWJLZXkSIwohAlcobsPzfTNVe7uqAAsndErJAjqplnyudaGB0f+R+p3FEgQKAggBGAgSEQoLCgRtdW9uEgMyMDAQwJoMGkD54fQAFlekIAnE62hZYl0uQelh/HLv0oQpCciY5Dn8H1SZFuTsrGdu41PH1Uxa4woptCELi/8Ov9yzdeEFAC9H\", \"mode\": \"BROADCAST_MODE_BLOCK\"}")
        XCTAssertEqual(output.error, "")
    }

    func testStaking() {
        let stakeMessage = CosmosMessage.Delegate.with {
            $0.delegatorAddress = "cosmos1hsk6jryyqjfhp5dhc55tc9jtckygx0eph6dd02"
            $0.validatorAddress = "cosmosvaloper1zkupr83hrzkn3up5elktzcq3tuft8nxsmwdqgp"
            $0.amount = CosmosAmount.with {
                $0.amount = "10"
                $0.denom = "muon"
            }
        }

        let message = CosmosMessage.with {
            $0.stakeMessage = stakeMessage
        }

        let fee = CosmosFee.with {
            $0.gas = 101721
            $0.amounts = [CosmosAmount.with {
                $0.amount = "1018"
                $0.denom = "muon"
            }]
        }

        let input = CosmosSigningInput.with {
            $0.signingMode = .protobuf;
            $0.accountNumber = 1037
            $0.chainID = "gaia-13003"
            $0.memo = ""
            $0.sequence = 7
            $0.messages = [message]
            $0.fee = fee
            $0.privateKey = privateKey.data
        }

        let output: CosmosSigningOutput = AnySigner.sign(input: input, coin: .cosmos)

        XCTAssertJSONEqual(output.serialized, "{\"mode\":\"BROADCAST_MODE_BLOCK\",\"tx_bytes\":\"CpsBCpgBCiMvY29zbW9zLnN0YWtpbmcudjFiZXRhMS5Nc2dEZWxlZ2F0ZRJxCi1jb3Ntb3MxaHNrNmpyeXlxamZocDVkaGM1NXRjOWp0Y2t5Z3gwZXBoNmRkMDISNGNvc21vc3ZhbG9wZXIxemt1cHI4M2hyemtuM3VwNWVsa3R6Y3EzdHVmdDhueHNtd2RxZ3AaCgoEbXVvbhICMTASZgpQCkYKHy9jb3Ntb3MuY3J5cHRvLnNlY3AyNTZrMS5QdWJLZXkSIwohAlcobsPzfTNVe7uqAAsndErJAjqplnyudaGB0f+R+p3FEgQKAggBGAcSEgoMCgRtdW9uEgQxMDE4ENmaBhpA8O9Jm/kL6Za2I3poDs5vpMowYJgNvYCJBRU/vxAjs0lNZYsq40qpTbwOTbORjJA5UjQ6auc40v6uCFT4q4z+uA==\"}")
        XCTAssertEqual(output.error, "")
    }

    func testWithdraw() {
        // https://stargate.cosmos.network/txs/DE2FF0BC39E70DB576DF86C263A5BDB42FF3D5D5B914A4A30E8C13B14A950FFF
        let delegator = "cosmos100rhxclqasy6vnrcervgh99alx5xw7lkfp4u54"
        let validators = [
            "cosmosvaloper1ey69r37gfxvxg62sh4r0ktpuc46pzjrm873ae8",
            "cosmosvaloper1sjllsnramtg3ewxqwwrwjxfgc4n4ef9u2lcnj0",
            "cosmosvaloper1648ynlpdw7fqa2axt0w2yp3fk542junl7rsvq6"
        ]

        let withdrawals = validators.map { validator in
            CosmosMessage.WithdrawDelegationReward.with {
                $0.delegatorAddress = delegator
                $0.validatorAddress = validator
            }
        }.map { withdraw in
            CosmosMessage.with {
                $0.withdrawStakeRewardMessage = withdraw
            }
        }

        let fee = CosmosFee.with {
            $0.amounts = [CosmosAmount.with {
                $0.denom = "uatom"
                $0.amount = "1"
            }]
            $0.gas = 220000
        }

        let input = CosmosSigningInput.with {
            $0.signingMode = .protobuf;
            $0.fee = fee
            $0.accountNumber = 8698
            $0.chainID = "cosmoshub-2"
            $0.sequence = 318
            $0.messages = withdrawals
            $0.fee = fee
            $0.privateKey = privateKey.data
        }

        let output: CosmosSigningOutput = AnySigner.sign(input: input, coin: .cosmos)

        XCTAssertJSONEqual(output.serialized, "{\"mode\":\"BROADCAST_MODE_BLOCK\",\"tx_bytes\":\"CukDCqABCjcvY29zbW9zLmRpc3RyaWJ1dGlvbi52MWJldGExLk1zZ1dpdGhkcmF3RGVsZWdhdG9yUmV3YXJkEmUKLWNvc21vczEwMHJoeGNscWFzeTZ2bnJjZXJ2Z2g5OWFseDV4dzdsa2ZwNHU1NBI0Y29zbW9zdmFsb3BlcjFleTY5cjM3Z2Z4dnhnNjJzaDRyMGt0cHVjNDZwempybTg3M2FlOAqgAQo3L2Nvc21vcy5kaXN0cmlidXRpb24udjFiZXRhMS5Nc2dXaXRoZHJhd0RlbGVnYXRvclJld2FyZBJlCi1jb3Ntb3MxMDByaHhjbHFhc3k2dm5yY2VydmdoOTlhbHg1eHc3bGtmcDR1NTQSNGNvc21vc3ZhbG9wZXIxc2psbHNucmFtdGczZXd4cXd3cndqeGZnYzRuNGVmOXUybGNuajAKoAEKNy9jb3Ntb3MuZGlzdHJpYnV0aW9uLnYxYmV0YTEuTXNnV2l0aGRyYXdEZWxlZ2F0b3JSZXdhcmQSZQotY29zbW9zMTAwcmh4Y2xxYXN5NnZucmNlcnZnaDk5YWx4NXh3N2xrZnA0dTU0EjRjb3Ntb3N2YWxvcGVyMTY0OHlubHBkdzdmcWEyYXh0MHcyeXAzZms1NDJqdW5sN3JzdnE2EmUKUQpGCh8vY29zbW9zLmNyeXB0by5zZWNwMjU2azEuUHViS2V5EiMKIQJXKG7D830zVXu7qgALJ3RKyQI6qZZ8rnWhgdH/kfqdxRIECgIIARi+AhIQCgoKBXVhdG9tEgExEOC2DRpAXLgJ+8xEMUn7nkFj3ukg2V65Vh5ob7HKeCaNpMM6OPQrpW2r6askfssIFcOd8ThiBEz65bJz81Fmb5MtDTGv4g==\"}")
        XCTAssertEqual(output.error, "")
    }

    func testIbcTransfer() {
        let privateKey = PrivateKey(data: Data(hexString: "8bbec3772ddb4df68f3186440380c301af116d1422001c1877d6f5e4dba8c8af")!)!
        let publicKey = privateKey.getPublicKeySecp256k1(compressed: true)
        let fromAddress = AnyAddress(publicKey: publicKey, coin: .cosmos)

        let transferMessage = CosmosMessage.Transfer.with {
            $0.sourcePort = "transfer"
            $0.sourceChannel = "channel-141"
            $0.sender = fromAddress.description
            $0.receiver = "osmo18s0hdnsllgcclweu9aymw4ngktr2k0rkvn7jmn"
            $0.token = CosmosAmount.with {
                $0.amount = "100000"
                $0.denom = "uatom"
            }
            $0.timeoutHeight = CosmosHeight.with {
                $0.revisionNumber = 1
                $0.revisionHeight = 8800000
            }
        }

        let message = CosmosMessage.with {
            $0.transferTokensMessage = transferMessage
        }

        let fee = CosmosFee.with {
            $0.gas = 500000
            $0.amounts = [CosmosAmount.with {
                $0.amount = "12500"
                $0.denom = "uatom"
            }]
        }

        let input = CosmosSigningInput.with {
            $0.signingMode = .protobuf;
            $0.accountNumber = 546179
            $0.chainID = "cosmoshub-4"
            $0.sequence = 2
            $0.messages = [message]
            $0.fee = fee
            $0.privateKey = privateKey.data
        }

        let output: CosmosSigningOutput = AnySigner.sign(input: input, coin: .cosmos)

        // https://www.mintscan.io/cosmos/txs/817101F3D96314AD028733248B28BAFAD535024D7D2C8875D3FE31DC159F096B
        XCTAssertJSONEqual(output.serialized, "{\"mode\":\"BROADCAST_MODE_BLOCK\",\"tx_bytes\":\"Cr4BCrsBCikvaWJjLmFwcGxpY2F0aW9ucy50cmFuc2Zlci52MS5Nc2dUcmFuc2ZlchKNAQoIdHJhbnNmZXISC2NoYW5uZWwtMTQxGg8KBXVhdG9tEgYxMDAwMDAiLWNvc21vczFta3k2OWNuOGVrdHd5MDg0NXZlYzl1cHNkcGhrdHh0MDNna3dseCorb3NtbzE4czBoZG5zbGxnY2Nsd2V1OWF5bXc0bmdrdHIyazBya3ZuN2ptbjIHCAEQgI6ZBBJoClAKRgofL2Nvc21vcy5jcnlwdG8uc2VjcDI1NmsxLlB1YktleRIjCiEC7O9c5DejAsZ/lUaN5LMfNukR9GfX5qUrQcHhPh1WNkkSBAoCCAEYAhIUCg4KBXVhdG9tEgUxMjUwMBCgwh4aQK0HIWdFMk+C6Gi1KG/vELe1ffcc1aEWUIqz2t/ZhwqNNHxUUSp27wteiugHEMVTEIOBhs84t2gIcT/nD/1yKOU=\"}")
        XCTAssertEqual(output.error, "")
    }
}
