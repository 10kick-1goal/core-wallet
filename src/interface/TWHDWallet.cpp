// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWHDWallet.h>

#include "../Coin.h"
#include "../HDWallet.h"

using namespace TW;

bool TWHDWalletIsValid(TWString *_Nonnull mnemonic) {
    return HDWallet::isValid(TWStringUTF8Bytes(mnemonic));
}

struct TWHDWallet *_Nonnull TWHDWalletCreate(int strength, TWString *_Nonnull passphrase) {
    return new TWHDWallet{ HDWallet(strength, TWStringUTF8Bytes(passphrase)) };
}

struct TWHDWallet *_Nonnull TWHDWalletCreateWithMnemonic(TWString *_Nonnull mnemonic, TWString *_Nonnull passphrase) {
    return new TWHDWallet{ HDWallet(TWStringUTF8Bytes(mnemonic), TWStringUTF8Bytes(passphrase)) };
}

struct TWHDWallet *_Nonnull TWHDWalletCreateWithData(TWData *_Nonnull data, TWString *_Nonnull passphrase) {
    auto *d = reinterpret_cast<const Data*>(data);
    return new TWHDWallet{ HDWallet(*d, TWStringUTF8Bytes(passphrase)) };
}

void TWHDWalletDelete(struct TWHDWallet *wallet) {
    delete wallet;
}

TWData *_Nonnull TWHDWalletSeed(struct TWHDWallet *_Nonnull wallet) {
    return TWDataCreateWithBytes(wallet->impl.seed.data(), HDWallet::seedSize);
}

TWString *_Nonnull TWHDWalletMnemonic(struct TWHDWallet *_Nonnull wallet){
    return TWStringCreateWithUTF8Bytes(wallet->impl.mnemonic.c_str());
}

struct TWPrivateKey *_Nonnull TWHDWalletGetKeyForCoin(struct TWHDWallet *wallet, TWCoinType coin) {
    auto derivationPath = TW::derivationPath(coin);
    return new TWPrivateKey{ wallet->impl.getKey(derivationPath) };
}

struct TWPrivateKey *_Nonnull TWHDWalletGetKey(struct TWHDWallet *_Nonnull wallet, TWString *_Nonnull derivationPath) {
    auto& s = *reinterpret_cast<const std::string*>(derivationPath);
    return new TWPrivateKey{ wallet->impl.getKey( TW::DerivationPath(s)) };
}

struct TWPrivateKey *_Nonnull TWHDWalletGetKeyBIP44(struct TWHDWallet *_Nonnull wallet, enum TWCoinType coin, uint32_t account, uint32_t change, uint32_t address) {
    const auto derivationPath = DerivationPath(TW::purpose(coin), coin, account, change, address);
    return new TWPrivateKey{ wallet->impl.getKey(derivationPath) };
}

TWString *_Nonnull TWHDWalletGetExtendedPrivateKey(struct TWHDWallet *wallet, TWPurpose purpose, TWCoinType coin, TWHDVersion version) {
    return new std::string(wallet->impl.getExtendedPrivateKey(purpose, coin, version));
}

TWString *_Nonnull TWHDWalletGetExtendedPublicKey(struct TWHDWallet *wallet, TWPurpose purpose, TWCoinType coin, TWHDVersion version) {
    return new std::string(wallet->impl.getExtendedPublicKey(purpose, coin, version));
}

TWPublicKey *TWHDWalletGetPublicKeyFromExtended(TWString *_Nonnull extended, TWString *_Nonnull derivationPath) {
    const auto derivationPathObject = DerivationPath(*reinterpret_cast<const std::string*>(derivationPath));
    auto publicKey = HDWallet::getPublicKeyFromExtended(*reinterpret_cast<const std::string*>(extended), derivationPathObject);
    if (!publicKey) {
        return nullptr;
    }
    return new TWPublicKey{ PublicKey(*publicKey) };
}
