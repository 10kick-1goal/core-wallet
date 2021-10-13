// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../Data.h"
#include "ReadData.h"
#include <stdexcept>


TW::Data TW::readBytes(const TW::Data& from, int max, int initial_pos) {
    if (from.size() - initial_pos < max) {
        throw std::invalid_argument("Data::Cannot read enough bytes!");
    }
    return TW::Data(from.begin() + initial_pos, from.begin() + initial_pos + max);
}

TW::Data TW::readVarBytes(const Data& from, int initial_pos, uint32_t* dataRead) {
    uint64_t size = readVar<uint64_t>(from, initial_pos);
    auto shift = varIntSize(size);
    if (dataRead) {
        *dataRead = uint32_t(shift + size);
    }
    return readBytes(from, int(size), initial_pos + int(shift));
}

template<> uint64_t TW::readVar(const TW::Data& from, int initial_pos, const uint64_t &max) {
    byte fb = from[initial_pos];
    uint64_t value;
    if (fb == 0xFD) {
        value = decode16LE(from.data() + initial_pos + 1);
    } else if (fb == 0xFE) {
        value = decode32LE(from.data() + initial_pos + 1);
    } else if (fb == 0xFF) {
        value = decode64LE(from.data() + initial_pos + 1);
    } else {
        value = fb;
    }
    if (value > max) {
        // std::cout << "TOO HUGE VALUE: " << value << " max=" << max << std::endl;
        throw std::invalid_argument("ReadData::ReadVarInt error: Too huge value! FormatException");
    }
    return value;
}

template<> int64_t TW::readVar(const TW::Data& from, int initial_pos, const int64_t &max) {
    return (int64_t) readVar<uint64_t>(from, initial_pos, uint64_t(max));
}

TW::Data TW::writeVarBytes(const Data& from, int initial_pos) {
    Data resp;
    encodeVarInt(uint64_t(from.size() - initial_pos), resp);
    resp.insert(resp.end(), from.begin() + initial_pos, from.end());
    return resp;
}
