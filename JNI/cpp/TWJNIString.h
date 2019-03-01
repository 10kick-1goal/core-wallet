// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <jni.h>
#include "TWJNI.h"

TW_EXTERN_C_BEGIN

/// Converts a TWString to a Java string.
jstring TWStringJString(TWString *string, JNIEnv *env);

/// Converts a Java string to a TWString.
TWString * TWStringCreateWithJString(JNIEnv *env, jstring string);

TW_EXTERN_C_END
