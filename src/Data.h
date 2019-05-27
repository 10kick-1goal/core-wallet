// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace TW {

using byte = std::uint8_t;
using Data = std::vector<byte>;

inline Data data(const std::string& data) {
    return std::vector<byte>(data.begin(), data.end());
}

inline void append(Data& data, const Data& suffix) {
    data.insert(data.end(), suffix.begin(), suffix.end());
}

/// Determines if a byte array has a specific prefix.
template <typename T>
inline bool has_prefix(const Data& data, T& prefix) {
    return std::equal(prefix.begin(), prefix.end(), data.begin(), data.begin() + std::min(data.size(), prefix.size()));
}

} // namespace TW
