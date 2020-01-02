// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Data.h"

#include <string>

namespace TW::Ethereum::ABI {

/// Abstract base class for parameters.
class ParamBase
{
public:
    virtual ~ParamBase() = default;
    virtual std::string getType() const = 0;
    virtual size_t getSize() const = 0;
    virtual bool isDynamic() const = 0;
    virtual void encode(Data& data) const = 0;
    virtual bool decode(const Data& encoded, size_t& offset_inout) = 0;
};

/// Collection parameters base class
class ParamCollection: public ParamBase
{
public:
    virtual size_t getCount() const = 0;
};

} // namespace TW::Ethereum::ABI
