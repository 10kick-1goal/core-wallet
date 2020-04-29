// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "ISerializable.h"

namespace TW::NEO {

class Serializable : public ISerializable {
  public:
    template<class T>
    static inline Data serialize(const std::vector <T> &data) {
        Data resp;
        encodeVarInt(data.size(), resp);
        for (auto it = data.begin(); it < data.end(); ++it) {
            append(resp, it->serialize());
        }
        return resp;
    }

    template<class T>
    static inline Data serialize(const T *data, int size) {
        Data resp;
        encodeVarInt(uint64_t(size), resp);
        for (int i = 0; i < size; ++i) {
            append(resp, data[i].serialize());
        }
        return resp;
    }

    template<class T>
    static inline Data
    serialize(std::iterator <std::input_iterator_tag, T> begin, std::iterator <std::input_iterator_tag, T> end) {
        Data resp;
        encodeVarInt(uint64_t(end - begin), resp);
        for (auto it = begin; it < end; ++it) {
            append(resp, it->serialize());
        }
        return resp;
    }

    template<class T>
    static inline int deserialize(std::vector <T> &resp, const Data& data, int initial_pos = 0) {
        uint64_t size = readVar<uint64_t>(data, initial_pos, INT_MAX);
        // assert(size >= 0);
        initial_pos += varIntSize(size);
        for (uint64_t i = 0; i < size; ++i) {
            T value;
            value.deserialize(data, initial_pos);
            resp.push_back(value);
            initial_pos += value.size();
        }

        return initial_pos;
    }

};

} // namespace TW::NEO
