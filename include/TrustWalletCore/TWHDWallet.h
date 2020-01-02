// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TWBase.h"
#include "TWCoinType.h"
#include "TWCurve.h"
#include "TWData.h"
#include "TWHDVersion.h"
#include "TWPrivateKey.h"
#include "TWPublicKey.h"
#include "TWPurpose.h"
#include "TWString.h"

TW_EXTERN_C_BEGIN

TW_EXPORT_CLASS
struct TWHDWallet;

/// Determines if a mnemonic phrase is valid.
TW_EXPORT_STATIC_METHOD
bool TWHDWalletIsValid(TWString *_Nonnull mnemonic);

/// Creates a new random HDWallet with the provided strength in bits.
TW_EXPORT_STATIC_METHOD
struct TWHDWallet *_Nonnull TWHDWalletCreate(int strength, TWString *_Nonnull passphrase);

/// Creates an HDWallet from a mnemonic seed.
TW_EXPORT_STATIC_METHOD
struct TWHDWallet *_Nonnull TWHDWalletCreateWithMnemonic(TWString *_Nonnull mnemonic, TWString *_Nonnull passphrase);

/// Creates an HDWallet from a seed.
TW_EXPORT_STATIC_METHOD
struct TWHDWallet *_Nonnull TWHDWalletCreateWithData(TWData *_Nonnull data, TWString *_Nonnull passphrase);

/// Deletes a wallet.
TW_EXPORT_METHOD
void TWHDWalletDelete(struct TWHDWallet *_Nonnull wallet);

/// Wallet seed.
TW_EXPORT_PROPERTY
TWData *_Nonnull TWHDWalletSeed(struct TWHDWallet *_Nonnull wallet);

// Wallet Mnemonic
TW_EXPORT_PROPERTY
TWString *_Nonnull TWHDWalletMnemonic(struct TWHDWallet *_Nonnull wallet);

/// Returns master key.
TW_EXPORT_METHOD
struct TWPrivateKey *_Nonnull TWHDWalletGetMasterKey(struct TWHDWallet *_Nonnull wallet, enum TWCurve curve);

/// Generates the default private key for the specified coin.
TW_EXPORT_METHOD
struct TWPrivateKey *_Nonnull TWHDWalletGetKeyForCoin(struct TWHDWallet *_Nonnull wallet, enum TWCoinType coin);

/// Generates the default address for the specified coin (without exposing intermediary private key).
TW_EXPORT_METHOD
TWString *_Nonnull TWHDWalletGetAddressForCoin(struct TWHDWallet *_Nonnull wallet, enum TWCoinType coin);

/// Generates the private key for the specified derivation path.
TW_EXPORT_METHOD
struct TWPrivateKey *_Nonnull TWHDWalletGetKey(struct TWHDWallet *_Nonnull wallet, TWString *_Nonnull derivationPath);

/// Generates the private key for the specified BIP44 path.
///
/// @see https://github.com/bitcoin/bips/blob/master/bip-0044.mediawiki
TW_EXPORT_METHOD
struct TWPrivateKey *_Nonnull TWHDWalletGetKeyBIP44(struct TWHDWallet *_Nonnull wallet, enum TWCoinType coin, uint32_t account, uint32_t change, uint32_t address);

/// Returns the extended private key.
TW_EXPORT_METHOD
TWString *_Nonnull TWHDWalletGetExtendedPrivateKey(struct TWHDWallet *_Nonnull wallet, enum TWPurpose purpose, enum TWCoinType coin, enum TWHDVersion version);

/// Returns the exteded public key.
TW_EXPORT_METHOD
TWString *_Nonnull TWHDWalletGetExtendedPublicKey(struct TWHDWallet *_Nonnull wallet, enum TWPurpose purpose, enum TWCoinType coin, enum TWHDVersion version);

/// Computes the public key from an exteded public key representation.
TW_EXPORT_STATIC_METHOD
struct TWPublicKey *_Nullable TWHDWalletGetPublicKeyFromExtended(TWString *_Nonnull extended, TWString *_Nonnull derivationPath);

TW_EXTERN_C_END
