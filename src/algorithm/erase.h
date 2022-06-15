// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <algorithm>
#include <vector>

namespace TW {

// C++17 Implementation of https://en.cppreference.com/w/cpp/container/vector/erase2
template <class T, class Alloc, class U>
constexpr typename std::vector<T, Alloc>::size_type erase(std::vector<T, Alloc>& c,
                                                          const U& value) {
    auto it = std::remove(c.begin(), c.end(), value);
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

template <class T, class Alloc, class Functor>
constexpr typename std::vector<T, Alloc>::size_type erase_if(std::vector<T, Alloc>& c,
                                                             Functor&& pred) {
    auto it = std::remove_if(c.begin(), c.end(), std::forward<Functor>(pred));
    auto r = std::distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

} // namespace TW