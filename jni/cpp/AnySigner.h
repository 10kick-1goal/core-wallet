// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#ifndef JNI_TW_ANYSIGNER_H
#define JNI_TW_ANYSIGNER_H

#include <jni.h>
#include <TrustWalletCore/TWBase.h>

TW_EXTERN_C_BEGIN

JNIEXPORT
jbyteArray JNICALL Java_wallet_core_java_AnySigner_nativeSign(JNIEnv *env, jclass thisClass, jbyteArray input, jint coin);

JNIEXPORT
jbyteArray JNICALL Java_wallet_core_java_AnySigner_nativeEncode(JNIEnv *env, jclass thisClass, jbyteArray input, jint coin);

JNIEXPORT
jbyteArray JNICALL Java_wallet_core_java_AnySigner_nativeDecode(JNIEnv *env, jclass thisClass, jbyteArray input, jint coin);

JNIEXPORT
jboolean JNICALL Java_wallet_core_java_AnySigner_supportsJSON(JNIEnv *env, jclass thisClass, jint coin);

JNIEXPORT
jbyteArray JNICALL Java_wallet_core_java_AnySigner_signJSON(JNIEnv *env, jclass thisClass, jstring json, jbyteArray key, jint coin);

JNIEXPORT
jbyteArray JNICALL Java_wallet_core_java_AnySigner_nativePlan(JNIEnv *env, jclass thisClass, jbyteArray input, jint coin);

TW_EXTERN_C_END

#endif // JNI_TW_ANYSIGNER_H
