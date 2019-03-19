// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TWBase.h"
#include "TWCurve.h"
#include "TWPrivateKey.h"
#include "TWPurpose.h"
#include "TWString.h"

TW_EXTERN_C_BEGIN

/// Coin type for Level 2 of BIP44.
///
/// - SeeAlso: https://github.com/satoshilabs/slips/blob/master/slip-0044.md
TW_EXPORT_ENUM(uint32_t)
enum TWCoinType {
    TWCoinTypeBinance = 714,
    TWCoinTypeBitcoin = 0,
    TWCoinTypeBitcoinCash = 145,
    TWCoinTypeCallisto = 820,
    TWCoinTypeDash = 5,
    TWCoinTypeEthereum = 60,
    TWCoinTypeEthereumClassic = 61,
    TWCoinTypeGo = 6060,
    TWCoinTypeICON = 74,
    TWCoinTypeLitecoin = 2,
    TWCoinTypeOntology = 1024,
    TWCoinTypePoa = 178,
    TWCoinTypeRipple = 144,
    TWCoinTypeTezos = 1729,
    TWCoinTypeThunderToken = 1001,
    TWCoinTypeTomoChain = 889,
    TWCoinTypeTron = 195,
    TWCoinTypeVeChain = 818,
    TWCoinTypeWanChain = 5718350,
    TWCoinTypeXDai = 700,
    TWCoinTypeZcash = 133,
    TWCoinTypeZcoin = 136,
    TWCoinTypeNimiq = 242,
    TWCoinTypeStellar = 148,
    TWCoinTypeAion = 425,
    TWCoinTypeCosmos = 118,
    TWCoinTypeNEO = 888,
    TWCoinTypeKIN = 2017,
};

/// Returns the purpose for a coin type.
TW_EXPORT_PROPERTY
enum TWPurpose TWCoinTypePurpose(enum TWCoinType coin);

/// Returns the curve that should be used for a coin type.
TW_EXPORT_PROPERTY
enum TWCurve TWCoinTypeCurve(enum TWCoinType coin);

/// Validates an address string.
TW_EXPORT_METHOD
bool TWCoinTypeValidate(enum TWCoinType coin, TWString *_Nonnull address);

/// Returns the default derivation path for a particular coin.
TW_EXPORT_METHOD
TWString *_Nonnull TWCoinTypeDerivationPath(enum TWCoinType coin);

/// Derives the address for a particular coin from the private key.
TW_EXPORT_METHOD
TWString *_Nonnull TWCoinTypeDeriveAddress(enum TWCoinType coin, struct TWPrivateKey *_Nonnull privateKey);

TW_EXTERN_C_END
