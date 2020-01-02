// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWEthereumAbiFunction.h>

#include "Ethereum/ABI.h"
#include "Data.h"
#include "HexCoding.h"
#include "../uint256.h"

#include <string>
#include <vector>
#include <cassert>

using namespace TW;
using namespace TW::Ethereum;
using namespace TW::Ethereum::ABI;

struct TWEthereumAbiFunction *_Nullable TWEthereumAbiFunctionCreateWithString(TWString *_Nonnull name) {
    auto func = Function(TWStringUTF8Bytes(name));
    return new TWEthereumAbiFunction{ func };
}

void TWEthereumAbiFunctionDelete(struct TWEthereumAbiFunction *_Nonnull func_in) {
    assert(func_in != nullptr);
    delete func_in;
}

TWString *_Nonnull TWEthereumAbiFunctionGetType(struct TWEthereumAbiFunction *_Nonnull func_in) {
    assert(func_in != nullptr);
    auto function = func_in->impl;
    std::string sign = function.getType();
    return TWStringCreateWithUTF8Bytes(sign.c_str());
}

///// AddParam

int TWEthereumAbiFunctionAddParamUInt8(struct TWEthereumAbiFunction *_Nonnull func_in, uint8_t val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamUInt8>(val);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamUInt16(struct TWEthereumAbiFunction *_Nonnull func_in, uint16_t val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamUInt16>(val);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamUInt32(struct TWEthereumAbiFunction *_Nonnull func_in, uint32_t val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamUInt32>(val);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamUInt64(struct TWEthereumAbiFunction *_Nonnull func_in, uint64_t val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamUInt64>(val);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamUInt256(struct TWEthereumAbiFunction *_Nonnull func_in, TWData *_Nonnull val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    uint256_t val2 = load(*static_cast<const Data*>(val));
    auto param = std::make_shared<ParamUInt256>(val2);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamUIntN(struct TWEthereumAbiFunction *_Nonnull func_in, int bits, TWData *_Nonnull val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    uint256_t val2 = load(*static_cast<const Data*>(val));
    auto param = std::make_shared<ParamUIntN>(bits, val2);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamInt8(struct TWEthereumAbiFunction *_Nonnull func_in, int8_t val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamInt8>(val);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamInt16(struct TWEthereumAbiFunction *_Nonnull func_in, int16_t val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamInt16>(val);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamInt32(struct TWEthereumAbiFunction *_Nonnull func_in, int32_t val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamInt32>(val);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamInt64(struct TWEthereumAbiFunction *_Nonnull func_in, int64_t val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamInt64>(val);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamInt256(struct TWEthereumAbiFunction *_Nonnull func_in, TWData *_Nonnull val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    assert(val != nullptr);
    int256_t val2 = Util::int256FromUint256(load(*static_cast<const Data*>(val)));
    auto param = std::make_shared<ParamInt256>(val2);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamIntN(struct TWEthereumAbiFunction *_Nonnull func_in, int bits, TWData *_Nonnull val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    assert(val != nullptr);
    int256_t val2 = Util::int256FromUint256(load(*static_cast<const Data*>(val)));
    auto param = std::make_shared<ParamIntN>(bits, val2);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamBool(struct TWEthereumAbiFunction *_Nonnull func_in, bool val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamBool>(val);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamString(struct TWEthereumAbiFunction *_Nonnull func_in, TWString *_Nonnull val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    assert(val != nullptr);
    auto param = std::make_shared<ParamString>(TWStringUTF8Bytes(val));
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamAddress(struct TWEthereumAbiFunction *_Nonnull func_in, TWData *_Nonnull val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    assert(val != nullptr);
    Data data = TW::data(TWDataBytes(val), TWDataSize(val));
    auto param = std::make_shared<ParamAddress>(data);
    auto idx = function.addParam(param, isOutput);
    return idx;
}

int TWEthereumAbiFunctionAddParamBytes(struct TWEthereumAbiFunction *_Nonnull func_in, TWData *_Nonnull val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    Data data = TW::data(TWDataBytes(val), TWDataSize(val));
    auto param = std::make_shared<ParamByteArray>(data);
    auto idx = function.addParam(param, isOutput);
    return idx;    
}

int TWEthereumAbiFunctionAddParamBytesFix(struct TWEthereumAbiFunction *_Nonnull func_in, size_t count, TWData *_Nonnull val, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    Data data = TW::data(TWDataBytes(val), TWDataSize(val));
    auto param = std::make_shared<ParamByteArrayFix>(count, data);
    auto idx = function.addParam(param, isOutput);
    return idx;    
}

int TWEthereumAbiFunctionAddParamArray(struct TWEthereumAbiFunction *_Nonnull func_in, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    auto param = std::make_shared<ParamArray>();
    auto idx = function.addParam(param, isOutput);
    return idx;    
}

///// GetParam

uint8_t TWEthereumAbiFunctionGetParamUInt8(struct TWEthereumAbiFunction *_Nonnull func_in, int idx, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    std::shared_ptr<ParamBase> param;
    if (!function.getParam(idx, param, isOutput)) {
        return 0;
    }
    return (std::dynamic_pointer_cast<ParamUInt8>(param))->getVal();
}

uint64_t TWEthereumAbiFunctionGetParamUInt64(struct TWEthereumAbiFunction *_Nonnull func_in, int idx, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    std::shared_ptr<ParamBase> param;
    if (!function.getParam(idx, param, isOutput)) {
        return 0;
    }
    return (std::dynamic_pointer_cast<ParamUInt64>(param))->getVal();
}

TWData *_Nonnull TWEthereumAbiFunctionGetParamUInt256(struct TWEthereumAbiFunction *_Nonnull func_in, int idx, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    TW::Data valDat;
    std::shared_ptr<ParamBase> param;
    if (!function.getParam(idx, param, isOutput)) {
        return TWDataCreateWithData(&valDat);
    }
    uint256_t val256 = (std::dynamic_pointer_cast<ParamUInt256>(param))->getVal();
    valDat = TW::store(val256);
    return TWDataCreateWithData(&valDat);
}

bool TWEthereumAbiFunctionGetParamBool(struct TWEthereumAbiFunction *_Nonnull func_in, int idx, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    std::shared_ptr<ParamBase> param;
    if (!function.getParam(idx, param, isOutput)) {
        return false;
    }
    return (std::dynamic_pointer_cast<ParamBool>(param))->getVal();
}

TWString *_Nonnull TWEthereumAbiFunctionGetParamString(struct TWEthereumAbiFunction *_Nonnull func_in, int idx, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    std::string valStr;
    std::shared_ptr<ParamBase> param;
    if (!function.getParam(idx, param, isOutput)) {
        return TWStringCreateWithUTF8Bytes(valStr.c_str());
    }
    valStr = (std::dynamic_pointer_cast<ParamString>(param))->getVal();
    return TWStringCreateWithUTF8Bytes(valStr.c_str());
}

TWData *_Nonnull TWEthereumAbiFunctionGetParamAddress(struct TWEthereumAbiFunction *_Nonnull func_in, int idx, bool isOutput) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    Data valData;
    std::shared_ptr<ParamBase> param;
    if (!function.getParam(idx, param, isOutput)) {
        return TWDataCreateWithBytes(valData.data(), valData.size());
    }
    valData = (std::dynamic_pointer_cast<ParamAddress>(param))->getData();
    return TWDataCreateWithBytes(valData.data(), valData.size());
}

///// AddInArrayParam

int addInArrayParam(Function& function, int arrayIdx, const std::shared_ptr<ParamBase>& childParam) {
    std::shared_ptr<ParamBase> param;
    if (!function.getInParam(arrayIdx, param)) {
        return -1;
    }
    std::shared_ptr<ParamArray> paramArr = std::dynamic_pointer_cast<ParamArray>(param);
    return paramArr->addParam(childParam);
}

int TWEthereumAbiFunctionAddInArrayParamUInt8(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, uint8_t val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    return addInArrayParam(function, arrayIdx, std::make_shared<ParamUInt8>(val));
}

int TWEthereumAbiFunctionAddInArrayParamUInt16(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, uint16_t val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    return addInArrayParam(function, arrayIdx, std::make_shared<ParamUInt16>(val));
}

int TWEthereumAbiFunctionAddInArrayParamUInt32(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, uint32_t val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    return addInArrayParam(function, arrayIdx, std::make_shared<ParamUInt32>(val));
}

int TWEthereumAbiFunctionAddInArrayParamUInt64(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, uint64_t val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    return addInArrayParam(function, arrayIdx, std::make_shared<ParamUInt64>(val));
}

int TWEthereumAbiFunctionAddInArrayParamUInt256(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, TWData *_Nonnull val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    uint256_t val2 = load(*static_cast<const Data*>(val));
    return addInArrayParam(function, arrayIdx, std::make_shared<ParamUInt256>(val2));
}

int TWEthereumAbiFunctionAddInArrayParamUIntN(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, int bits, TWData *_Nonnull val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    uint256_t val2 = load(*static_cast<const Data*>(val));
    return addInArrayParam(function, arrayIdx, std::make_shared<ParamUIntN>(bits, val2));
}

int TWEthereumAbiFunctionAddInArrayParamInt8(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, int8_t val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    return addInArrayParam(function, arrayIdx, std::make_shared<ParamInt8>(val));
}

int TWEthereumAbiFunctionAddInArrayParamInt16(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, int16_t val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    return addInArrayParam(function, arrayIdx, std::make_shared<ParamInt16>(val));
}

int TWEthereumAbiFunctionAddInArrayParamInt32(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, int32_t val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    return addInArrayParam(function, arrayIdx, std::make_shared<ParamInt32>(val));
}

int TWEthereumAbiFunctionAddInArrayParamInt64(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, int64_t val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    return addInArrayParam(function, arrayIdx, std::make_shared<ParamInt64>(val));
}

int TWEthereumAbiFunctionAddInArrayParamInt256(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, TWData *_Nonnull val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    assert(val != nullptr);
    int256_t val2 = Util::int256FromUint256(load(*static_cast<const Data*>(val)));
    return addInArrayParam(function, arrayIdx, std::make_shared<ParamInt256>(val2));
}

int TWEthereumAbiFunctionAddInArrayParamIntN(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, int bits, TWData *_Nonnull val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    assert(val != nullptr);
    int256_t val2 = Util::int256FromUint256(load(*static_cast<const Data*>(val)));
    return addInArrayParam(function, arrayIdx, std::make_shared<ParamIntN>(bits, val2));
}

int TWEthereumAbiFunctionAddInArrayParamBool(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, bool val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    return addInArrayParam(function, arrayIdx, std::make_shared<ParamBool>(val));
}

int TWEthereumAbiFunctionAddInArrayParamString(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, TWString *_Nonnull val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    assert(val != nullptr);
    return addInArrayParam(function, arrayIdx, std::make_shared<ParamString>(TWStringUTF8Bytes(val)));
}

int TWEthereumAbiFunctionAddInArrayParamAddress(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, TWData *_Nonnull val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    assert(val != nullptr);
    Data data = TW::data(TWDataBytes(val), TWDataSize(val));
    return addInArrayParam(function, arrayIdx, std::make_shared<ParamAddress>(data));
}

int TWEthereumAbiFunctionAddInArrayParamBytes(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, TWData *_Nonnull val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    Data data = TW::data(TWDataBytes(val), TWDataSize(val));
    return addInArrayParam(function, arrayIdx, std::make_shared<ParamByteArray>(data));
}

int TWEthereumAbiFunctionAddInArrayParamBytesFix(struct TWEthereumAbiFunction *_Nonnull func_in, int arrayIdx, size_t count, TWData *_Nonnull val) {
    assert(func_in != nullptr);
    Function& function = func_in->impl;

    Data data = TW::data(TWDataBytes(val), TWDataSize(val));
    return addInArrayParam(function, arrayIdx, std::make_shared<ParamByteArrayFix>(count, data));
}
