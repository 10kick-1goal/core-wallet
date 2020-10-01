// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "ParamFactory.h"
#include "HexCoding.h"

#include <boost/algorithm/string/predicate.hpp>

using namespace std;
using namespace boost::algorithm;

namespace TW::Ethereum::ABI {

static int parseBitSize(const std::string& type) {
    int size = stoi(type);
    if (size < 8 || size > 256 || size % 8 != 0 ||
        size == 8 || size == 16 || size == 32 || size == 64 || size == 256) {
        throw invalid_argument("invalid bit size");
    }
    return size;
}

static std::shared_ptr<ParamBase> makeUInt(const std::string& type) {
    auto bits = parseBitSize(type);
    return make_shared<ParamUIntN>(bits);
}

static std::shared_ptr<ParamBase> makeInt(const std::string& type) {
    auto bits = parseBitSize(type);
    return make_shared<ParamIntN>(bits);
}

std::shared_ptr<ParamBase> ParamFactory::make(const std::string& type) {
    shared_ptr<ParamBase> param;
    if (type == "address") {
        param = make_shared<ParamAddress>();
    } else if (type == "uint8") {
        param = make_shared<ParamUInt8>();
    } else if (type == "uint16") {
        param = make_shared<ParamUInt16>();
    } else if (type == "uint32") {
        param = make_shared<ParamUInt32>();
    } else if (type == "uint64") {
        param = make_shared<ParamUInt64>();
    } else if (type == "uint256" || type == "uint") {
        param = make_shared<ParamUInt256>();
    } else if (type == "int8") {
        param = make_shared<ParamInt8>();
    } else if (type == "int16") {
        param = make_shared<ParamInt16>();
    } else if (type == "int32") {
        param = make_shared<ParamInt32>();
    } else if (type == "int64") {
        param = make_shared<ParamInt64>();
    } else if (type == "int256" || type == "int") {
        param = make_shared<ParamInt256>();
    } else if (starts_with(type, "uint")) {
        param = makeUInt(type.substr(4, type.size() - 1));
    } else if (starts_with(type, "int")) {
        param = makeInt(type.substr(3, type.size() - 1));
    } else if (type == "bool") {
        param = make_shared<ParamBool>();
    } else if (type == "bytes") {
        param = make_shared<ParamByteArray>();
    } else if (starts_with(type, "bytes")) {
        auto bits = stoi(type.substr(5, type.size() - 1));
        param = make_shared<ParamByteArrayFix>(bits);
    } else if (type == "string") {
        param = make_shared<ParamString>();
    }
    return param;
}

std::string ParamFactory::getValue(const std::shared_ptr<ParamBase>& param, const std::string& type) {
    std::string result = "";
    if (type == "address") {
        auto value = dynamic_pointer_cast<ParamAddress>(param);
        result = hexEncoded(value->getData());
    } else if (type == "uint8") {
        result = boost::lexical_cast<std::string>((uint)dynamic_pointer_cast<ParamUInt8>(param)->getVal());
    } else if (type == "uint16") {
        result = boost::lexical_cast<std::string>(dynamic_pointer_cast<ParamUInt16>(param)->getVal());
    } else if (type == "uint32") {
        result = boost::lexical_cast<std::string>(dynamic_pointer_cast<ParamUInt32>(param)->getVal());
    } else if (type == "uint64") {
        result = boost::lexical_cast<std::string>(dynamic_pointer_cast<ParamUInt64>(param)->getVal());
    } else if (type == "uint256" || type == "uint") {
        result = boost::lexical_cast<std::string>(dynamic_pointer_cast<ParamUInt256>(param)->getVal());
    } else if (type == "int8") {
        result = boost::lexical_cast<std::string>((int)dynamic_pointer_cast<ParamInt8>(param)->getVal());
    } else if (type == "int16") {
        result = boost::lexical_cast<std::string>(dynamic_pointer_cast<ParamInt16>(param)->getVal());
    } else if (type == "int32") {
        result = boost::lexical_cast<std::string>(dynamic_pointer_cast<ParamInt32>(param)->getVal());
    } else if (type == "int64") {
        result = boost::lexical_cast<std::string>(dynamic_pointer_cast<ParamInt64>(param)->getVal());
    } else if (type == "int256" || type == "int") {
        result = boost::lexical_cast<std::string>(dynamic_pointer_cast<ParamInt256>(param)->getVal());
    } else if (starts_with(type, "uint")) {
        auto value = dynamic_pointer_cast<ParamUIntN>(param);
        result = boost::lexical_cast<std::string>(value->getVal());
    } else if (starts_with(type, "int")) {
        auto value = dynamic_pointer_cast<ParamIntN>(param);
        result = boost::lexical_cast<std::string>(value->getVal());
    } else if (type == "bool") {
        auto value = dynamic_pointer_cast<ParamBool>(param);
        result = value->getVal() ? "true" : "false";
    } else if (type == "bytes") {
        auto value = dynamic_pointer_cast<ParamByteArray>(param);
        result = hexEncoded(value->getVal());
    } else if (starts_with(type, "bytes")) {
        auto value = dynamic_pointer_cast<ParamByteArrayFix>(param);
        result = hexEncoded(value->getVal());
    } else if (type == "string") {
        auto value = dynamic_pointer_cast<ParamString>(param);
        result = value->getVal();
    }
    return result;
}

} // namespace TW::Ethereum::ABI
