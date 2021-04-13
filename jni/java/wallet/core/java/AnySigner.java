// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

package wallet.core.java;

import com.google.protobuf.Message;
import com.google.protobuf.Parser;

import wallet.core.jni.CoinType;

public class AnySigner {
    public static <T extends Message> T sign(Message input, CoinType coin, Parser<T> parser) throws Exception {
        byte[] data = input.toByteArray();
        byte[] outputData = nativeSign(data, coin.value());
        T output = parser.parseFrom(outputData);
        outputData = null;
        return output;
    }
    public static native byte[] nativeSign(byte[] data, int coin);

    public static native String signJSON(String json, byte[] key, int coin);

    public static native boolean supportsJSON(int coin);

    public static <T extends Message> T plan(Message input, CoinType coin, Parser<T> parser) throws Exception {
        byte[] data = input.toByteArray();
        byte[] outputData = nativePlan(data, coin.value());
        T output = parser.parseFrom(outputData);
        outputData = null;
        return output;
    }
    public static native byte[] nativePlan(byte[] data, int coin);
}
