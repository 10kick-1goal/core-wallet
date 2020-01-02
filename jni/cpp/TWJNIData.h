// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <jni.h>
#include "TWJNI.h"

TW_EXTERN_C_BEGIN

/// Converts a TWData to a Java byte array.
jbyteArray TWDataJByteArray(TWData *data, JNIEnv *env);

/// Converts a Java byte array to a TWData.
TWData * TWDataCreateWithJByteArray(JNIEnv *env, jbyteArray array);

TW_EXTERN_C_END
