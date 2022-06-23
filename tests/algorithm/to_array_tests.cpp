// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "algorithm/to_array.h"

#include "gtest/gtest.h"

using namespace TW;

TEST(Algorithms, ToArray) {
    std::string str{"foo"};
    auto value = TW::to_array<std::uint8_t, 4>(str);
    auto expected = std::array<std::uint8_t, 4>{"foo"};
    ASSERT_EQ(value, expected);

    std::vector<std::uint8_t> ints{0, 1, 2};
    auto another_value = TW::to_array<std::uint8_t, 3>(ints);
    auto expected_ints = std::array<std::uint8_t, 3>{0, 1, 2};
    ASSERT_EQ(another_value, expected_ints);
}
