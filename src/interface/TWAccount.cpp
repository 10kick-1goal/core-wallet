// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWAccount.h>

#include "../Keystore/Account.h"

using namespace TW;

struct TWAccount *_Nonnull TWAccountCreate(TWString *_Nonnull address, TWString *_Nonnull derivationPath, TWString *_Nonnull extendedPublicKey) {
    auto& addressString = *reinterpret_cast<const std::string*>(address);
    auto& derivationPathString = *reinterpret_cast<const std::string*>(derivationPath);
    auto& extendedPublicKeyString = *reinterpret_cast<const std::string*>(extendedPublicKey);
    const auto dp = DerivationPath(derivationPathString);
    return new TWAccount{ Keystore::Account(addressString, dp, extendedPublicKeyString) };
}

void TWAccountDelete(struct TWAccount *_Nonnull account) {
    delete account;
}

TWString *_Nonnull TWAccountAddress(struct TWAccount *_Nonnull account) {
    return TWStringCreateWithUTF8Bytes(account->impl.address.c_str());
}

TWString *_Nonnull TWAccountDerivationPath(struct TWAccount *_Nonnull account) {
    return TWStringCreateWithUTF8Bytes(account->impl.derivationPath.string().c_str());
}

TWString *_Nonnull TWAccountExtendedPublicKey(struct TWAccount *_Nonnull account) {
    return TWStringCreateWithUTF8Bytes(account->impl.extendedPublicKey.c_str());
}

enum TWCoinType TWAccountCoin(struct TWAccount *_Nonnull account) {
    return account->impl.coin();
}
