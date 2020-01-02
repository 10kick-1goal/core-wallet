// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Cbor.h"

#include "HexCoding.h"

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Cbor;
using namespace std;


TEST(Cbor, EncSample1) {
    EXPECT_EQ(
        "8205a26178186461793831", 
        hex(Encode::array({
            Encode::uint(5),
            Encode::map({
                make_pair(Encode::string("x"), Encode::uint(100)),
                make_pair(Encode::string("y"), Encode::negInt(50)),
            }),
        }).encoded())
    );
}

TEST(Cbor, EncUInt) {
    EXPECT_EQ("00", hex(Encode::uint(0).encoded()));
    EXPECT_EQ("01", hex(Encode::uint(1).encoded()));
    EXPECT_EQ("0a", hex(Encode::uint(10).encoded()));
    EXPECT_EQ("17", hex(Encode::uint(23).encoded()));
    EXPECT_EQ("1818", hex(Encode::uint(24).encoded()));
    EXPECT_EQ("1819", hex(Encode::uint(25).encoded()));
    EXPECT_EQ("181a", hex(Encode::uint(26).encoded()));
    EXPECT_EQ("181b", hex(Encode::uint(27).encoded()));
    EXPECT_EQ("181c", hex(Encode::uint(28).encoded()));
    EXPECT_EQ("181d", hex(Encode::uint(29).encoded()));
    EXPECT_EQ("181e", hex(Encode::uint(30).encoded()));
    EXPECT_EQ("181f", hex(Encode::uint(31).encoded()));
    EXPECT_EQ("1820", hex(Encode::uint(32).encoded()));
    EXPECT_EQ("183f", hex(Encode::uint(0x3f).encoded()));
    EXPECT_EQ("1840", hex(Encode::uint(0x40).encoded()));
    EXPECT_EQ("1864", hex(Encode::uint(100).encoded()));
    EXPECT_EQ("187f", hex(Encode::uint(0x7f).encoded()));
    EXPECT_EQ("1880", hex(Encode::uint(0x80).encoded()));
    EXPECT_EQ("18ff", hex(Encode::uint(0xff).encoded()));
    EXPECT_EQ("190100", hex(Encode::uint(0x0100).encoded()));
    EXPECT_EQ("1903e8", hex(Encode::uint(1000).encoded()));
    EXPECT_EQ("198765", hex(Encode::uint(0x8765).encoded()));
    EXPECT_EQ("19ffff", hex(Encode::uint(0xffff).encoded()));
    EXPECT_EQ("1a00010000", hex(Encode::uint(0x00010000).encoded()));
    EXPECT_EQ("1a000f4240", hex(Encode::uint(1000000).encoded()));
    EXPECT_EQ("1a00800000", hex(Encode::uint(0x00800000).encoded()));
    EXPECT_EQ("1a87654321", hex(Encode::uint(0x87654321).encoded()));
    EXPECT_EQ("1affffffff", hex(Encode::uint(0xffffffff).encoded()));
    EXPECT_EQ("1b0000000100000000", hex(Encode::uint(0x0000000100000000).encoded()));
    EXPECT_EQ("1b000000e8d4a51000", hex(Encode::uint(1000000000000).encoded()));
    EXPECT_EQ("1b876543210fedcba9", hex(Encode::uint(0x876543210fedcba9).encoded()));
    EXPECT_EQ("1bffffffffffffffff", hex(Encode::uint(0xffffffffffffffff).encoded()));
}

TEST(Cbor, EncNegInt) {
    EXPECT_EQ("20", hex(Encode::negInt(1).encoded())); // -1
    EXPECT_EQ("00", hex(Encode::negInt(0).encoded())); // 0
    EXPECT_EQ("21", hex(Encode::negInt(2).encoded()));
    EXPECT_EQ("28", hex(Encode::negInt(9).encoded()));
    EXPECT_EQ("37", hex(Encode::negInt(24).encoded()));
    EXPECT_EQ("3818", hex(Encode::negInt(25).encoded()));
    EXPECT_EQ("38ff", hex(Encode::negInt(0x0100).encoded()));
    EXPECT_EQ("390100", hex(Encode::negInt(0x0101).encoded()));
    EXPECT_EQ("39ffff", hex(Encode::negInt(0x10000).encoded()));
    EXPECT_EQ("3a00010000", hex(Encode::negInt(0x00010001).encoded()));
    EXPECT_EQ("3a00800000", hex(Encode::negInt(0x00800001).encoded()));
    EXPECT_EQ("3a87654321", hex(Encode::negInt(0x87654322).encoded()));
    EXPECT_EQ("3affffffff", hex(Encode::negInt(0x100000000).encoded()));
    EXPECT_EQ("3b0000000100000000", hex(Encode::negInt(0x0000000100000001).encoded()));
    EXPECT_EQ("3b876543210fedcba9", hex(Encode::negInt(0x876543210fedcbaa).encoded()));
    EXPECT_EQ("3bfffffffffffffffe", hex(Encode::negInt(0xffffffffffffffff).encoded()));
}


TEST(Cbor, EncString) {
    EXPECT_EQ("60", hex(Encode::string("").encoded()));
    EXPECT_EQ("6141", hex(Encode::string("A").encoded()));
    EXPECT_EQ("656162636465", hex(Encode::string("abcde").encoded()));
    Data long258(258);
    EXPECT_EQ(
        "590102000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", 
        hex(Encode::bytes(long258).encoded())
    );
}

TEST(Cbor, EncTag) {
    EXPECT_EQ("c506", hex(Encode::tag(5, Encode::uint(6)).encoded()));
    EXPECT_EQ("d94321191234", hex(Encode::tag(0x4321, Encode::uint(0x1234)).encoded()));
}

TEST(Cbor, EncInvalid) {
    Data invalid = parse_hex("5b99999999999999991234"); // invalid very looong string
    EXPECT_FALSE(Decode(invalid).isValid());

    try {
        Encode::fromRaw(invalid);
    } catch (exception& ex) {
        return;
    }
    FAIL() << "Expected exception";
}

TEST(Cbor, DecInt) {
    EXPECT_EQ(0, Decode(parse_hex("00")).getValue());
    EXPECT_EQ(1, Decode(parse_hex("01")).getValue());
    EXPECT_EQ(10, Decode(parse_hex("0a")).getValue());
    EXPECT_EQ(23, Decode(parse_hex("17")).getValue());
    EXPECT_EQ(24, Decode(parse_hex("1818")).getValue());
    EXPECT_EQ(25, Decode(parse_hex("1819")).getValue());
    EXPECT_EQ(26, Decode(parse_hex("181a")).getValue());
    EXPECT_EQ(27, Decode(parse_hex("181b")).getValue());
    EXPECT_EQ(28, Decode(parse_hex("181c")).getValue());
    EXPECT_EQ(29, Decode(parse_hex("181d")).getValue());
    EXPECT_EQ(30, Decode(parse_hex("181e")).getValue());
    EXPECT_EQ(31, Decode(parse_hex("181f")).getValue());
    EXPECT_EQ(32, Decode(parse_hex("1820")).getValue());
    EXPECT_EQ(0x3f, Decode(parse_hex("183f")).getValue());
    EXPECT_EQ(0x40, Decode(parse_hex("1840")).getValue());
    EXPECT_EQ(100, Decode(parse_hex("1864")).getValue());
    EXPECT_EQ(0x7f, Decode(parse_hex("187f")).getValue());
    EXPECT_EQ(0x80, Decode(parse_hex("1880")).getValue());
    EXPECT_EQ(0xff, Decode(parse_hex("18ff")).getValue());
    EXPECT_EQ(0x100, Decode(parse_hex("190100")).getValue());
    EXPECT_EQ(1000, Decode(parse_hex("1903e8")).getValue());
    EXPECT_EQ(0x8765, Decode(parse_hex("198765")).getValue());
    EXPECT_EQ(0xffff, Decode(parse_hex("19ffff")).getValue());
    EXPECT_EQ(0x00010000, Decode(parse_hex("1a00010000")).getValue());
    EXPECT_EQ(1000000, Decode(parse_hex("1a000f4240")).getValue());
    EXPECT_EQ(0x00800000, Decode(parse_hex("1a00800000")).getValue());
    EXPECT_EQ(0x87654321, Decode(parse_hex("1a87654321")).getValue());
    EXPECT_EQ(0xffffffff, Decode(parse_hex("1affffffff")).getValue());
    EXPECT_EQ(0x0000000100000000, Decode(parse_hex("1b0000000100000000")).getValue());
    EXPECT_EQ(1000000000000, Decode(parse_hex("1b000000e8d4a51000")).getValue());
    EXPECT_EQ(0x876543210fedcba9, Decode(parse_hex("1b876543210fedcba9")).getValue());
    EXPECT_EQ(0xffffffffffffffff, Decode(parse_hex("1bffffffffffffffff")).getValue());
}

TEST(Cbor, DecMinortypeInvlalid) {
    EXPECT_FALSE(Decode(parse_hex("1c")).isValid()); // 28 unused
    EXPECT_FALSE(Decode(parse_hex("1d")).isValid()); // 29 unused
    EXPECT_FALSE(Decode(parse_hex("1e")).isValid()); // 30 unused
    EXPECT_TRUE(Decode(parse_hex("1b0000000000000000")).isValid());
}

TEST(Cbor, DecArray3) {
    Decode cbor = Decode(parse_hex("83010203"));
    EXPECT_EQ(3, cbor.getArrayElements().size());
}

TEST(Cbor, DecArrayNested) {
    Data d1 = parse_hex("8301820203820405");
    Decode cbor = Decode(d1);
    EXPECT_EQ(3, cbor.getArrayElements().size());

    EXPECT_EQ(1, cbor.getArrayElements()[0].getValue());
    EXPECT_EQ(2, cbor.getArrayElements()[1].getArrayElements().size());
    EXPECT_EQ(2, cbor.getArrayElements()[1].getArrayElements()[0].getValue());
    EXPECT_EQ(3, cbor.getArrayElements()[1].getArrayElements()[1].getValue());
    EXPECT_EQ(2, cbor.getArrayElements()[2].getArrayElements().size());
    EXPECT_EQ(4, cbor.getArrayElements()[2].getArrayElements()[0].getValue());
    EXPECT_EQ(5, cbor.getArrayElements()[2].getArrayElements()[1].getValue());
}

TEST(Cbor, DecEncoded) {
    // sometimes getting the encoded version is useful during decoding too
    Decode cbor = Decode(parse_hex("8301820203820405"));
    EXPECT_EQ(3, cbor.getArrayElements().size());
    EXPECT_EQ("820203", hex(cbor.getArrayElements()[1].encoded()));
    EXPECT_EQ("820405", hex(cbor.getArrayElements()[2].encoded()));
}

TEST(Cbor, DecMemoryref) {
    // make sure reference to data is valid even if parent object has been destroyed
    Decode* cbor = new Decode(parse_hex("828301020383010203"));
    auto elems = cbor->getArrayElements();
    // delete parent
    delete cbor;
    // also do some new allocation
    Decode* dummy = new Decode(parse_hex("5555555555555555"));
    // work with the child references
    EXPECT_EQ(2, elems.size());
    EXPECT_EQ(3, elems[0].getArrayElements().size());
    EXPECT_EQ(3, elems[1].getArrayElements().size());
    delete dummy;
}

TEST(Cbor, getValue) {
   EXPECT_EQ(5, Decode(parse_hex("05")).getValue());
}

TEST(Cbor, getValueInvalid) {
    try {
        Decode(parse_hex("83010203")).getValue(); // array
    } catch (exception& ex) {
        return;
    }
    FAIL() << "Expected exception";
}

TEST(Cbor, getString) {
    // bytes/string and getString/getBytes work in all combinations
    EXPECT_EQ("abcde", Decode(parse_hex("656162636465")).getString());
    EXPECT_EQ("abcde", Decode(parse_hex("456162636465")).getString());
    EXPECT_EQ("6162636465", hex(Decode(parse_hex("656162636465")).getBytes()));
    EXPECT_EQ("6162636465", hex(Decode(parse_hex("456162636465")).getBytes()));
}

TEST(Cbor, ArrayEmpty) {
    Data cbor = Encode::array({}).encoded();
    
    EXPECT_EQ("80", hex(cbor));
    EXPECT_TRUE(Decode(cbor).isValid());
    EXPECT_EQ("[]", Decode(cbor).dumpToString());

    Decode decode(cbor);
    EXPECT_EQ(0, decode.getArrayElements().size());
}

TEST(Cbor, Array3) {
    Data cbor = Encode::array({
        Encode::uint(1),
        Encode::uint(2),
        Encode::uint(3),
    }).encoded();
    
    EXPECT_EQ("83010203", hex(cbor));
    EXPECT_TRUE(Decode(cbor).isValid());
    EXPECT_EQ("[1, 2, 3]", Decode(cbor).dumpToString());

    Decode decode(cbor);
    EXPECT_EQ(3, decode.getArrayElements().size());
    EXPECT_EQ(1, decode.getArrayElements()[0].getValue());
    EXPECT_EQ(2, decode.getArrayElements()[1].getValue());
    EXPECT_EQ(3, decode.getArrayElements()[2].getValue());
}

TEST(Cbor, ArrayNested) {
    Data cbor = Encode::array({
        Encode::uint(1),
        Encode::array({
            Encode::uint(2),
            Encode::uint(3),
        }),
        Encode::array({
            Encode::uint(4),
            Encode::uint(5),
        }),
    }).encoded();
    
    EXPECT_EQ("8301820203820405", hex(cbor));
    EXPECT_TRUE(Decode(cbor).isValid());
    EXPECT_EQ("[1, [2, 3], [4, 5]]", 
        Decode(cbor).dumpToString());

    Decode decode(cbor);
    EXPECT_EQ(3, decode.getArrayElements().size());
    EXPECT_EQ(1, decode.getArrayElements()[0].getValue());
    EXPECT_EQ(2, decode.getArrayElements()[1].getArrayElements().size());
    EXPECT_EQ(2, decode.getArrayElements()[1].getArrayElements()[0].getValue());
    EXPECT_EQ(3, decode.getArrayElements()[1].getArrayElements()[1].getValue());
    EXPECT_EQ(2, decode.getArrayElements()[2].getArrayElements().size());
    EXPECT_EQ(4, decode.getArrayElements()[2].getArrayElements()[0].getValue());
    EXPECT_EQ(5, decode.getArrayElements()[2].getArrayElements()[1].getValue());
}

TEST(Cbor, Array25) {
    auto elem = vector<Encode>();
    for (int i = 1; i <= 25; ++i) {
        elem.push_back(Encode::uint(i));
    }
    Data cbor = Encode::array(elem).encoded();
    
    EXPECT_EQ("98190102030405060708090a0b0c0d0e0f101112131415161718181819", hex(cbor));
    EXPECT_TRUE(Decode(cbor).isValid());
    EXPECT_EQ("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]",
        Decode(cbor).dumpToString());

    Decode decode(cbor);
    EXPECT_EQ(25, decode.getArrayElements().size());
    for (int i = 1; i <= 25; ++i) {
        EXPECT_EQ(i, decode.getArrayElements()[i - 1].getValue());
    }
}

TEST(Cbor, MapEmpty) {
    Data cbor = Encode::map({}).encoded();
    
    EXPECT_EQ("a0", hex(cbor));
    EXPECT_TRUE(Decode(cbor).isValid());
    EXPECT_EQ("{}", Decode(cbor).dumpToString());

    Decode decode(cbor);
    EXPECT_EQ(0, decode.getMapElements().size());
}

TEST(Cbor, Map2Num) {
    Data cbor = Encode::map({
        make_pair(Encode::uint(1), Encode::uint(2)),
        make_pair(Encode::uint(3), Encode::uint(4)),
    }).encoded();
    
    EXPECT_EQ("a201020304", hex(cbor));
    EXPECT_TRUE(Decode(cbor).isValid());
    EXPECT_EQ("{1: 2, 3: 4}", Decode(cbor).dumpToString());

    Decode decode(cbor);
    EXPECT_EQ(2, decode.getMapElements().size());
    EXPECT_EQ(1, decode.getMapElements()[0].first.getValue());
    EXPECT_EQ(2, decode.getMapElements()[0].second.getValue());
}

TEST(Cbor, Map2WithArr) {
    Data cbor = Encode::map({
        make_pair(Encode::string("a"), Encode::uint(1)),
        make_pair(Encode::string("b"), Encode::array({
            Encode::uint(2),
            Encode::uint(3),
        })),
    }).encoded();
    
    EXPECT_EQ("a26161016162820203", hex(cbor));
    EXPECT_TRUE(Decode(cbor).isValid());
    EXPECT_EQ("{\"a\": 1, \"b\": [2, 3]}", 
        Decode(cbor).dumpToString());

    Decode decode(cbor);
    EXPECT_EQ(2, decode.getMapElements().size());
    EXPECT_EQ("a", decode.getMapElements()[0].first.getString());
    EXPECT_EQ(1, decode.getMapElements()[0].second.getValue());
    EXPECT_EQ("b", decode.getMapElements()[1].first.getString());
    EXPECT_EQ(2, decode.getMapElements()[1].second.getArrayElements().size());
    EXPECT_EQ(2, decode.getMapElements()[1].second.getArrayElements()[0].getValue());
    EXPECT_EQ(3, decode.getMapElements()[1].second.getArrayElements()[1].getValue());
}

TEST(Cbor, MapNested) {
    Data cbor = Encode::map({
        make_pair(Encode::string("a"), Encode::map({
            make_pair(Encode::string("b"), Encode::string("c")),
        })),
    }).encoded();
    
    EXPECT_EQ("a16161a161626163", hex(cbor));
    EXPECT_TRUE(Decode(cbor).isValid());
    EXPECT_EQ("{\"a\": {\"b\": \"c\"}}", 
        Decode(cbor).dumpToString());

    Decode decode(cbor);
    EXPECT_EQ(1, decode.getMapElements().size());
    EXPECT_EQ("a", decode.getMapElements()[0].first.getString());
    EXPECT_EQ(1, decode.getMapElements()[0].second.getMapElements().size());
    EXPECT_EQ("b", decode.getMapElements()[0].second.getMapElements()[0].first.getString());
    EXPECT_EQ("c", decode.getMapElements()[0].second.getMapElements()[0].second.getString());
}

TEST(Cbor, ArrayIndef) {
    Data cbor = Encode::indefArray()
        .addIndefArrayElem(Encode::uint(1))
        .addIndefArrayElem(Encode::uint(2))
        .closeIndefArray()
    .encoded();
    
    EXPECT_EQ("9f0102ff", hex(cbor));
    EXPECT_TRUE(Decode(cbor).isValid());
    EXPECT_EQ("[_ 1, 2]",
        Decode(cbor).dumpToString());

    Decode decode(cbor);
    EXPECT_EQ(2, decode.getArrayElements().size());
    EXPECT_EQ(1, decode.getArrayElements()[0].getValue());
    EXPECT_EQ(2, decode.getArrayElements()[1].getValue());
}

TEST(Cbor, ArrayInfefErrorAddNostart) {
    try {
        Data cbor = Encode::uint(0).addIndefArrayElem(Encode::uint(1)).encoded();
    } catch (exception& ex) {
        return;
    }
    FAIL() << "Expected exception";
}

TEST(Cbor, ArrayInfefErrorCloseNostart) {
    try {
        Data cbor = Encode::uint(0).closeIndefArray().encoded();
    } catch (exception& ex) {
        return;
    }
    FAIL() << "Expected exception";
}

TEST(Cbor, ArrayInfefErrorResultNoclose) {
    try {
        Data cbor = Encode::indefArray()
            .addIndefArrayElem(Encode::uint(1))
            .addIndefArrayElem(Encode::uint(2))
            // close is missing, break command not written
        .encoded();
    } catch (exception& ex) {
        return;
    }
    FAIL() << "Expected exception";
}

TEST(Cbor, ArrayInfefErrorNoBreak) {
    EXPECT_TRUE(Decode(parse_hex("9f0102ff")).isValid());
    // without break it's invalid
    EXPECT_FALSE(Decode(parse_hex("9f0102")).isValid());
}
