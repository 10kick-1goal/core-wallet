// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TWBase.h"
#include "TWData.h"
#include "TWTronProto.h"

TW_EXTERN_C_BEGIN

/// Helper class to sign Tron transactions.
TW_EXPORT_CLASS
struct TWTronSigner;

/// Signs a transaction.
TW_EXPORT_STATIC_METHOD
TW_Tron_Proto_SigningOutput TWTronSignerSign(TW_Tron_Proto_SigningInput input);

TW_EXTERN_C_END
