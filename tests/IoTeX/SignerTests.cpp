// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <gtest/gtest.h>

#include "HexCoding.h"

#include "IoTeX/Address.h"
#include "IoTeX/Signer.h"
#include "proto/IoTeX.pb.h"

namespace TW::IoTeX {

TEST(IoTeXSigner, Sign) {
    auto input = Proto::SigningInput();
    input.set_version(1);
    input.set_nonce(123);
    input.set_gaslimit(888);
    input.set_gasprice("999");
    auto key = parse_hex("0806c458b262edd333a191e92f561aff338211ee3e18ab315a074a2d82aa343f");
    input.set_privatekey(key.data(), key.size());
    auto tsf = input.mutable_transfer();
    tsf->set_amount("456");
    tsf->set_recipient("io187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8j");
    auto text = parse_hex("68656c6c6f20776f726c6421"); // "hello world!"
    tsf->set_payload(text.data(), text.size());

    auto signer = IoTeX::Signer(std::move(input));
    auto h = signer.hash();
    ASSERT_EQ(hex(h.begin(), h.end()), "0f17cd7f43bdbeff73dfe8f5cb0c0045f2990884e5050841de887cf22ca35b50");
    auto sig = signer.sign();
    ASSERT_EQ(hex(sig.begin(), sig.end()), "555cc8af4181bf85c044c3201462eeeb95374f78aa48c67b87510ee63d5e502372e53082f03e9a11c1e351de539cedf85d8dff87de9d003cb9f92243541541a000");
}

TEST(IoTeXSigner, Build) {
    auto input = Proto::SigningInput();
    input.set_version(1);
    input.set_nonce(123);
    input.set_gaslimit(888);
    input.set_gasprice("999");
    auto keyhex = parse_hex("0806c458b262edd333a191e92f561aff338211ee3e18ab315a074a2d82aa343f");
    input.set_privatekey(keyhex.data(), keyhex.size());
    auto tsf = input.mutable_transfer();
    tsf->set_amount("456");
    tsf->set_recipient("io187wzp08vnhjjpkydnr97qlh8kh0dpkkytfam8j");
    auto text = parse_hex("68656c6c6f20776f726c6421"); // "hello world!"
    tsf->set_payload(text.data(), text.size());

    auto signer = IoTeX::Signer(std::move(input));
    auto output = signer.build();
    auto encoded = output.encoded(); // signed action's serialized bytes
    ASSERT_EQ(hex(encoded.begin(), encoded.end()), "0a4c0801107b18f8062203393939523e0a033435361229696f313837777a703038766e686a6a706b79646e723937716c68386b683064706b6b797466616d386a1a0c68656c6c6f20776f726c64211241044e18306ae9ef4ec9d07bf6e705442d4d1a75e6cdf750330ca2d880f2cc54607c9c33deb9eae9c06e06e04fe9ce3d43962cc67d5aa34fbeb71270d4bad3d648d91a41555cc8af4181bf85c044c3201462eeeb95374f78aa48c67b87510ee63d5e502372e53082f03e9a11c1e351de539cedf85d8dff87de9d003cb9f92243541541a000");
    auto h = output.hash(); // signed action's hash
    ASSERT_EQ(hex(h.begin(), h.end()), "6c84ac119058e859a015221f87a4e187c393d0c6ee283959342eac95fad08c33");
}

} // namespace TW::IoTeX
