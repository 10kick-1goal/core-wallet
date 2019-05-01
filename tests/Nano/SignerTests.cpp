// Copyright © 2019 Mart Roosmaa.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Nano/Signer.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

using namespace std;
using namespace TW;
using namespace TW::Nano;

const std::string kPrivateKey{"173c40e97fe2afcd24187e74f6b603cb949a5365e72fbdd065a6b165e2189e34"};
const std::string kRepOfficial1{"xrb_3arg3asgtigae3xckabaaewkx3bzsh7nwz7jkmjos79ihyaxwphhm6qgjps4"};
const std::string kRepNanode{"xrb_1nanode8ngaakzbck8smq6ru9bethqwyehomf79sae1k7xd47dkidjqzffeg"};

TEST(NanoSigner, sign1) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));
    const auto linkBlock = parse_hex("491fca2c69a84607d374aaf1f6acd3ce70744c5be0721b5ed394653e85233507");

    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_link_block(linkBlock.data(), linkBlock.size());
    input.set_representative(kRepOfficial1);
    input.set_balance("96242336390000000000000000000");

    // https://www.nanode.co/block/f9a323153daefe041efb94d69b9669c882c935530ed953bbe8a665dfedda9696
    const auto signer = Signer(input);
    ASSERT_EQ(hex(signer.blockHash), "f9a323153daefe041efb94d69b9669c882c935530ed953bbe8a665dfedda9696");
    const auto signature = signer.sign();
    ASSERT_EQ(hex(signature), "d247f6b90383b24e612569c75a12f11242f6e03b4914eadc7d941577dcf54a3a7cb7f0a4aba4246a40d9ebb5ee1e00b4a0a834ad5a1e7bef24e11f62b95a9e09");
}

TEST(NanoSigner, sign2) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));
    const auto parentBlock = parse_hex("f9a323153daefe041efb94d69b9669c882c935530ed953bbe8a665dfedda9696");

    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_parent_block(parentBlock.data(), parentBlock.size());
    input.set_representative(kRepNanode);
    input.set_balance("96242336390000000000000000000");

    // https://www.nanode.co/block/2568bf76336f7a415ca236dab97c1df9de951ca057a2e79df1322e647a259e7b
    const auto signer = Signer(input);
    ASSERT_EQ(hex(signer.blockHash), "2568bf76336f7a415ca236dab97c1df9de951ca057a2e79df1322e647a259e7b");
    const auto signature = signer.sign();
    ASSERT_EQ(hex(signature), "3a0687542405163d5623808052042b3482360a82cc003d178a0c0d8bfbca86450975d0faec60ae5ac37feba9a8e2205c8540317b26f2c589c2a6578b03870403");
}

TEST(NanoSigner, sign3) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));
    const auto parentBlock = parse_hex("2568bf76336f7a415ca236dab97c1df9de951ca057a2e79df1322e647a259e7b");
    const auto linkBlock = parse_hex("d7384845d2ae530b45a5dd50ee50757f988329f652781767af3f1bc2322f52b9");

    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_parent_block(parentBlock.data(), parentBlock.size());
    input.set_link_block(linkBlock.data(), linkBlock.size());
    input.set_representative(kRepNanode);
    input.set_balance("196242336390000000000000000000");

    // https://www.nanode.co/block/1ca240212838d053ecaa9dceee598c52a6080067edecaeede3319eb0b7db6525
    const auto signer = Signer(input);
    ASSERT_EQ(hex(signer.blockHash), "1ca240212838d053ecaa9dceee598c52a6080067edecaeede3319eb0b7db6525");
    const auto signature = signer.sign();
    ASSERT_EQ(hex(signature), "e980d45365ae2fb291950019f7c19a3d5fa5df2736ca7e7ca1984338b4686976cb7efdda2894ddcea480f82645b50f2340c9d0fc69a05621bdc355783a21820d");
}

TEST(NanoSigner, sign4) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));
    const auto parentBlock = parse_hex("1ca240212838d053ecaa9dceee598c52a6080067edecaeede3319eb0b7db6525");

    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_parent_block(parentBlock.data(), parentBlock.size());
    input.set_link_recipient("xrb_3wm37qz19zhei7nzscjcopbrbnnachs4p1gnwo5oroi3qonw6inwgoeuufdp");
    input.set_representative(kRepNanode);
    input.set_balance("126242336390000000000000000000");

    // https://www.nanode.co/block/32ac7d8f5a16a498abf203b8dfee623c9e111ff25e7339f8cd69ec7492b23edd
    const auto signer = Signer(input);
    ASSERT_EQ(hex(signer.blockHash), "32ac7d8f5a16a498abf203b8dfee623c9e111ff25e7339f8cd69ec7492b23edd");
    const auto signature = signer.sign();
    ASSERT_EQ(hex(signature), "bcb806e140c9e2bc71c51ebbd941b4d99cee3d97fd50e3006eabc5e325c712662e2dc163ee32660875d67815ce4721e122389d2e64f1c9ad4555a9d3d8c33802");
}

TEST(NanoSigner, signInvalid1) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));

    // Missing link_block
    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_representative(kRepOfficial1);
    input.set_balance("96242336390000000000000000000");

    ASSERT_THROW(Signer signer(input), std::invalid_argument);
}

TEST(NanoSigner, signInvalid2) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));
    const auto linkBlock = parse_hex("491fca2c69a84607d374aaf1f6acd3ce70744c5be0721b5ed394653e85233507");

    // Missing representative
    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_link_block(linkBlock.data(), linkBlock.size());
    input.set_balance("96242336390000000000000000000");

    ASSERT_THROW(Signer signer(input), std::invalid_argument);
}

TEST(NanoSigner, signInvalid3) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));
    const auto linkBlock = parse_hex("491fca2c69a84607d374aaf1f6acd3ce70744c5be0721b5ed394653e85233507");

    // Missing balance
    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_link_block(linkBlock.data(), linkBlock.size());
    input.set_representative(kRepOfficial1);

    ASSERT_THROW(Signer signer(input), std::invalid_argument);
}

TEST(NanoSigner, signInvalid4) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));
    const auto linkBlock = parse_hex("491fca2c69a84607d374aaf1f6acd3ce70744c5be0721b5ed394653e85233507");

    // Account first block cannot be 0 balance
    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_link_block(linkBlock.data(), linkBlock.size());
    input.set_representative(kRepOfficial1);
    input.set_balance("0");

    ASSERT_THROW(Signer signer(input), std::invalid_argument);
}

TEST(NanoSigner, signInvalid5) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));

    // First block must use link_block not link_recipient
    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_link_recipient("xrb_3wm37qz19zhei7nzscjcopbrbnnachs4p1gnwo5oroi3qonw6inwgoeuufdp");
    input.set_representative(kRepOfficial1);
    input.set_balance("96242336390000000000000000000");

    ASSERT_THROW(Signer signer(input), std::invalid_argument);
}

TEST(NanoSigner, signInvalid6) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));
    const auto linkBlock = parse_hex("491fca2c69a84607d374aaf1f6acd3ce70744c5be0721b5ed394653e85233507");

    // Invalid representative value
    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_link_block(linkBlock.data(), linkBlock.size());
    input.set_representative("xrb_4wm37qz19zhei7nzscjcopbrbnnachs4p1gnwo5oroi3qonw6inwgoeuufdp");
    input.set_balance("96242336390000000000000000000");

    ASSERT_THROW(Signer signer(input), std::invalid_argument);
}

TEST(NanoSigner, signInvalid7) {
    const auto privateKey = PrivateKey(parse_hex(kPrivateKey));
    const auto parentBlock = parse_hex("f9a323153daefe041efb94d69b9669c882c935530ed953bbe8a665dfedda9696");

    auto input = Proto::SigningInput();
    input.set_private_key(privateKey.bytes.data(), privateKey.bytes.size());
    input.set_parent_block(parentBlock.data(), parentBlock.size());
    input.set_link_recipient("xrb_4wm37qz19zhei7nzscjcopbrbnnachs4p1gnwo5oroi3qonw6inwgoeuufdp");
    input.set_representative(kRepOfficial1);
    input.set_balance("1.2.3");

    ASSERT_THROW(Signer signer(input), std::invalid_argument);
}
