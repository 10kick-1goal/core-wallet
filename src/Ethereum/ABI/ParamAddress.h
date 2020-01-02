// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Bytes.h"
#include "Util.h"
#include "../../Data.h"

namespace TW::Ethereum::ABI {

/// 160-bit Address parameter, "address".  Padded to the right, treated like ParamUInt160
class ParamAddress: public ParamUIntN
{
public:
    static const size_t bytes = 20;
    static const size_t bits = bytes * 8;
    ParamAddress(): ParamUIntN(bits) {}
    ParamAddress(const Data& val): ParamUIntN(bits, TW::load(val)) {}
    virtual std::string getType() const { return "address"; };
    // get the value as (20-byte) byte array (as opposed to uint256_t)
    Data getData() const {
        Data data = TW::store(getVal());
        if (data.size() >= bytes) { return data; }
        // need to pad
        Data padded(bytes - data.size());
        TW::append(padded, data);
        return padded;
    }
};

} // namespace TW::Ethereum::ABI
