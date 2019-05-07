// Copyright © 2017-2019 Trust.
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

/// Represents a Semux address.
TW_EXPORT_CLASS
struct TWSemuxAddress;

/// Compares two addresses for equality.
TW_EXPORT_STATIC_METHOD
bool TWSemuxAddressEqual(struct TWSemuxAddress *_Nonnull lhs, struct TWSemuxAddress *_Nonnull rhs);

/// Determines if the string is a valid address.
TW_EXPORT_STATIC_METHOD
bool TWSemuxAddressIsValidString(TWString *_Nonnull string);

/// Creates an address from a string representaion.
TW_EXPORT_STATIC_METHOD
struct TWSemuxAddress *_Nullable TWSemuxAddressCreateWithString(TWString *_Nonnull string);

/// Creates an address from a public key.
TW_EXPORT_STATIC_METHOD
struct TWSemuxAddress *_Nonnull TWSemuxAddressCreateWithPublicKey(struct TWPublicKey *_Nonnull publicKey);

TW_EXPORT_METHOD
void TWSemuxAddressDelete(struct TWSemuxAddress *_Nonnull address);

/// Returns the address string representation.
TW_EXPORT_PROPERTY
TWString *_Nonnull TWSemuxAddressDescription(struct TWSemuxAddress *_Nonnull address);

TW_EXTERN_C_END
