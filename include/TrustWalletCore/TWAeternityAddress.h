// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TWBase.h"
#include "TWData.h"
#include "TWString.h"

TW_EXTERN_C_BEGIN

struct TWPublicKey;

/// Represents a Aeternity address.
TW_EXPORT_CLASS
struct TWAeternityAddress;

/// Compares two addresses for equality.
TW_EXPORT_STATIC_METHOD
bool TWAeternityAddressEqual(struct TWAeternityAddress *_Nonnull lhs, struct TWAeternityAddress *_Nonnull rhs);

/// Determines if the string is a valid address.
TW_EXPORT_STATIC_METHOD
bool TWAeternityAddressIsValidString(TWString *_Nonnull string);

/// Creates an address from a string representaion.
TW_EXPORT_STATIC_METHOD
struct TWAeternityAddress *_Nullable TWAeternityAddressCreateWithString(TWString *_Nonnull string);

/// Creates an address from a public key.
TW_EXPORT_STATIC_METHOD
struct TWAeternityAddress *_Nullable TWAeternityAddressCreateWithPublicKey(struct TWPublicKey *_Nonnull publicKey);

TW_EXPORT_METHOD
void TWAeternityAddressDelete(struct TWAeternityAddress *_Nonnull address);

/// Returns the address string representation.
TW_EXPORT_PROPERTY
TWString *_Nonnull TWAeternityAddressDescription(struct TWAeternityAddress *_Nonnull address);

TW_EXTERN_C_END
