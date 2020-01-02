// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

/// A hierarchical deterministic wallet.
public extension HDWallet {
    static func derive(from extended: String, at path: DerivationPath) -> PublicKey? {
        return HDWallet.getPublicKeyFromExtended(extended: extended, derivationPath: path.description)
    }

    func getKey(at path: DerivationPath) -> PrivateKey {
        return getKey(derivationPath: path.description)
    }
}
