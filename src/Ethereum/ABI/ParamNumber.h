// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "ParamBase.h"
#include "Util.h"

#include "../../Data.h"
#include "../../uint256.h"

#include <string>

namespace TW::Ethereum::ABI {

inline void encode(uint256_t value, Data& data) {
    Data bytes = store(value);
    append(data, Data(Util::encodedUInt256Size - bytes.size()));
    append(data, bytes);
}

inline bool decode(const Data& encoded, uint256_t& decoded, size_t& offset_inout)
{
    decoded = 0u;
    if (encoded.empty() || (encoded.size() < (Util::encodedUInt256Size + offset_inout))) {
        return false;
    }
    decoded = loadWithOffset(encoded, offset_inout);
    offset_inout += Util::encodedUInt256Size;
    return true;
}

/// Generic parameter class for numeric types, like bool, uint32, int64, etc.  All are stored on 256 bits.
template<typename T> 
class ParamNumberType : public ParamBase
{
public:
    ParamNumberType() = default;
    ParamNumberType(T val) { _val = val; }
    void setVal(T val) { _val = val; }
    T getVal() const { return _val; }
    virtual std::string getType() const = 0;
    virtual size_t getSize() const { return Util::encodedUInt256Size; }
    virtual bool isDynamic() const { return false; }
    virtual void encode(Data& data) const {
        // cast up
        uint256_t val256 = static_cast<uint256_t>(_val);
        ABI::encode(val256, data);
    }
    static bool decodeNumber(const Data& encoded, T& decoded, size_t& offset_inout) {
        uint256_t val256;
        if (!ABI::decode(encoded, val256, offset_inout)) { return false; }
        // cast down
        decoded = static_cast<T>(val256);
        return true;
    }
    virtual bool decode(const Data& encoded, size_t& offset_inout) {
        return decodeNumber(encoded, _val, offset_inout);
    }
private:
    T _val;
};

class ParamUInt256 : public ParamNumberType<uint256_t>
{
public:
    ParamUInt256() : ParamNumberType<uint256_t>(uint256_t(0)) {}
    ParamUInt256(uint256_t val) : ParamNumberType<uint256_t>(val) {}
    virtual std::string getType() const { return "uint256"; }
    uint256_t getVal() const { return ParamNumberType<uint256_t>::getVal(); }
};

class ParamInt256 : public ParamNumberType<int256_t>
{
public:
    ParamInt256() : ParamNumberType<int256_t>(int256_t(0)) {}
    ParamInt256(int256_t val) : ParamNumberType<int256_t>(val) {}
    virtual std::string getType() const { return "int256"; }
    int256_t getVal() const { return ParamNumberType<int256_t>::getVal(); }
};

class ParamBool : public ParamNumberType<bool>
{
public:
    ParamBool() : ParamNumberType<bool>(false) {}
    ParamBool(bool val) : ParamNumberType<bool>(val) {}
    virtual std::string getType() const { return "bool"; }
    bool getVal() const { return ParamNumberType<bool>::getVal(); }
};

class ParamUInt8 : public ParamNumberType<uint8_t>
{
public:
    ParamUInt8() : ParamNumberType<uint8_t>(0) {}
    ParamUInt8(uint8_t val) : ParamNumberType<uint8_t>(val) {}
    virtual std::string getType() const { return "uint8"; }
};

class ParamInt8 : public ParamNumberType<int8_t>
{
public:
    ParamInt8() : ParamNumberType<int8_t>(0) {}
    ParamInt8(int8_t val) : ParamNumberType<int8_t>(val) {}
    virtual std::string getType() const { return "int8"; }
};

class ParamUInt16 : public ParamNumberType<uint16_t>
{
public:
    ParamUInt16() : ParamNumberType<uint16_t>(0) {}
    ParamUInt16(uint16_t val) : ParamNumberType<uint16_t>(val) {}
    virtual std::string getType() const { return "uint16"; }
};

class ParamInt16 : public ParamNumberType<int16_t>
{
public:
    ParamInt16() : ParamNumberType<int16_t>(0) {}
    ParamInt16(int16_t val) : ParamNumberType<int16_t>(val) {}
    virtual std::string getType() const { return "int16"; }
};

class ParamUInt32 : public ParamNumberType<uint32_t>
{
public:
    ParamUInt32() : ParamNumberType<uint32_t>(0) {}
    ParamUInt32(uint32_t val) : ParamNumberType<uint32_t>(val) {}
    virtual std::string getType() const { return "uint32"; }
};

class ParamInt32 : public ParamNumberType<int32_t>
{
public:
    ParamInt32() : ParamNumberType<int32_t>(0) {}
    ParamInt32(int32_t val) : ParamNumberType<int32_t>(val) {}
    virtual std::string getType() const { return "int32"; }
};

class ParamUInt64 : public ParamNumberType<uint64_t>
{
public:
    ParamUInt64() : ParamNumberType<uint64_t>(0) {}
    ParamUInt64(uint64_t val) : ParamNumberType<uint64_t>(val) {}
    virtual std::string getType() const { return "uint64"; }
};

class ParamInt64 : public ParamNumberType<int64_t>
{
public:
    ParamInt64() : ParamNumberType<int64_t>(0) {}
    ParamInt64(int64_t val) : ParamNumberType<int64_t>(val) {}
    virtual std::string getType() const { return "int64"; }
};

/// Generic parameter class for Uint8, 16, 24, 32, 40, ... 256.  For smaller sizes use the sepcial name like UInt32.
/// Stored on 256 bits.
class ParamUIntN : public ParamBase
{
public:
    const size_t bits;
    ParamUIntN(size_t bits_in) : bits(bits_in) { init(); }
    ParamUIntN(size_t bits_in, uint256_t val) : bits(bits_in) { init(); setVal(val); }
    void setVal(uint256_t val);
    uint256_t getVal() const { return _val; }
    virtual std::string getType() const { return "uint" + std::to_string(bits); }
    virtual size_t getSize() const { return Util::encodedUInt256Size; }
    virtual bool isDynamic() const { return false; }
    virtual void encode(Data& data) const { ABI::encode(_val, data); }
    static bool decodeNumber(const Data& encoded, uint256_t& decoded, size_t& offset_inout) {
        return ABI::decode(encoded, decoded, offset_inout);
    }
    virtual bool decode(const Data& encoded, size_t& offset_inout);

private:
    void init();
    uint256_t _val;
    uint256_t _mask;
};

/// Generic parameter class for Int8, 16, 24, 32, 40, ... 256.  For smaller sizes use the sepcial name like Int32.
/// Stored on 256 bits.
class ParamIntN : public ParamBase
{
public:
    const size_t bits;
    ParamIntN(size_t bits_in) : bits(bits_in) { init(); }
    ParamIntN(size_t bits_in, int256_t val) : bits(bits_in) { init(); setVal(val); }
    void setVal(int256_t val);
    int256_t getVal() const { return _val; }
    virtual std::string getType() const { return "int" + std::to_string(bits); }
    virtual size_t getSize() const { return Util::encodedUInt256Size; }
    virtual bool isDynamic() const { return false; }
    virtual void encode(Data& data) const { ABI::encode((uint256_t)_val, data); }
    static bool decodeNumber(const Data& encoded, int256_t& decoded, size_t& offset_inout);
    virtual bool decode(const Data& encoded, size_t& offset_inout);

private:
    void init();
    int256_t _val;
    uint256_t _mask;
};

} // namespace TW::Ethereum::ABI
