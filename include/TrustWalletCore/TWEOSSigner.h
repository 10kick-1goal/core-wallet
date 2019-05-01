// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TWBase.h"

#include "TWEOSProto.h"
#include "TWCommonProto.h"

TW_EXTERN_C_BEGIN

/// Represents a EOS Signer.
TW_EXPORT_CLASS
struct TWEOSSigner;

TW_EXPORT_STATIC_METHOD
TW_Proto_Result TWEOSSignerSign(TW_EOS_Proto_SigningInput input);

TW_EXTERN_C_END