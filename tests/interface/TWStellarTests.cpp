// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "TWTestUtilities.h"

#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWPrivateKey.h>
#include <TrustWalletCore/TWPublicKey.h>
#include <TrustWalletCore/TWPurpose.h>
#include <TrustWalletCore/TWStellarAddress.h>

#include <gtest/gtest.h>
#include <HDWallet.h>

TEST(Stellar, DeriveAddress) {
    auto words = STRING("indicate rival expand cave giant same grocery burden ugly rose tuna blood");
    auto passphrase = STRING("");

    auto wallet = WRAP(TWHDWallet, TWHDWalletCreateWithMnemonic(words.get(), passphrase.get()));
    auto key = WRAP(TWPrivateKey, TWHDWalletGetKeyForCoin(wallet.get(), TWCoinTypeStellar));
    auto publicKey = TWPrivateKeyGetPublicKeyEd25519(key.get());

    auto address = TWStellarAddressCreateWithPublicKey(publicKey);
    auto addressString = WRAPS(TWStellarAddressDescription(address));

    assertStringsEqual(addressString, "GAE2SZV4VLGBAPRYRFV2VY7YYLYGYIP5I7OU7BSP6DJT7GAZ35OKFDYI");
}