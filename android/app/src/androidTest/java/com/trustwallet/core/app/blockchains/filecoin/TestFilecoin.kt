package com.trustwallet.core.app.blockchains.filecoin

import com.google.protobuf.ByteString
import com.trustwallet.core.app.utils.toHexByteArray
import com.trustwallet.core.app.utils.toHexBytesInByteString
import junit.framework.Assert.assertEquals
import org.junit.Test
import wallet.core.java.AnySigner
import wallet.core.jni.AnyAddress
import wallet.core.jni.CoinType
import wallet.core.jni.CoinType.FILECOIN
import wallet.core.jni.PrivateKey
import wallet.core.jni.proto.Filecoin
import wallet.core.jni.proto.Filecoin.SigningOutput

class TestFilecoin {

    init {
        System.loadLibrary("TrustWalletCore")
    }

    @Test
    fun testAddress() {
        val privateKey = PrivateKey("1d969865e189957b9824bd34f26d5cbf357fda1a6d844cbf0c9ab1ed93fa7dbe".toHexByteArray())
        val publicKey = privateKey.getPublicKeySecp256k1(false)
        val address = AnyAddress(publicKey, CoinType.FILECOIN)
        assertEquals("f1z4a36sc7mfbv4z3qwutblp2flycdui3baffytbq", address.description())
    }

    @Test
    fun testSigner() {
        val input = Filecoin.SigningInput.newBuilder()
            .setPrivateKey("1d969865e189957b9824bd34f26d5cbf357fda1a6d844cbf0c9ab1ed93fa7dbe".toHexBytesInByteString())
            .setTo("f3um6uo3qt5of54xjbx3hsxbw5mbsc6auxzrvfxekn5bv3duewqyn2tg5rhrlx73qahzzpkhuj7a34iq7oifsq")
            .setNonce(2)
            // 600 FIL
            .setValue(ByteString.copyFrom("2086ac351052600000".toHexByteArray()))
            .setGasLimit(1000)
            .setGasFeeCap(ByteString.copyFrom("25f273933db5700000".toHexByteArray()))
            .setGasPremium(ByteString.copyFrom("2b5e3af16b18800000".toHexByteArray()))
            .build()

        val output = AnySigner.sign(input, CoinType.FILECOIN, SigningOutput.parser())
        val expted = """{"Message":{"From":"f1z4a36sc7mfbv4z3qwutblp2flycdui3baffytbq","GasFeeCap":"700000000000000000000","GasLimit":1000,"GasPremium":"800000000000000000000","Nonce":2,"To":"f3um6uo3qt5of54xjbx3hsxbw5mbsc6auxzrvfxekn5bv3duewqyn2tg5rhrlx73qahzzpkhuj7a34iq7oifsq","Value":"600000000000000000000"},"Signature":{"Data":"jMRu+OZ/lfppgmqSfGsntFrRLWZnUg3ZYmJTTRLsVt4V1310vR3VKGJpaE6S4sNvDOE6sEgmN9YmfTkPVK2qMgE=","Type":1}}"""
        assertEquals(expted, output.json)
    }
}
