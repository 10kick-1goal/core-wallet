// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Parameters.h"

#include <string>
#include <cassert>

using namespace TW::Ethereum::ABI;

ParamSet::~ParamSet() {
   _params.clear();
}

/// Returns the index of the parameter
int ParamSet::addParam(const std::shared_ptr<ParamBase>& param) {
    assert(param.get() != nullptr);
    if (param.get() == nullptr) { return -1; }
    _params.push_back(param);
    return static_cast<int>(_params.size() - 1);
}

void ParamSet::addParams(const std::vector<std::shared_ptr<ParamBase>>& params) {
    for (auto p: params) { addParam(p); }
}

bool ParamSet::getParam(int paramIndex, std::shared_ptr<ParamBase>& param_out) const {
    if (paramIndex >= _params.size() || paramIndex < 0) {
        return false;
    }
    param_out = _params[paramIndex];
    return true;
}

std::shared_ptr<ParamBase> ParamSet::getParamUnsafe(int paramIndex) const {
    if (_params.size() == 0) {
        // zero parameter, nothing to return.  This may cause trouble (segfault)
        return nullptr;
    }
    if (paramIndex >= _params.size() || paramIndex < 0) {
        // invalid index, return the first instead of nullptr
        return _params[0];
    }
    return _params[paramIndex];
}

/// Return the function type signature, of the form "baz(int32,uint256)"
std::string ParamSet::getType() const {
    std::string t = "(";
    int cnt = 0;
    for(auto p: _params) {
        if (cnt > 0) t += ",";
        t += p->getType();
        ++cnt;
    }
    t += ")";
    return t;
}

size_t ParamSet::getSize() const {
    size_t s = 0;
    for(auto p: _params) {
        s += p->getSize();
    }
    return 32 + Util::paddedTo32(s);
}

size_t ParamSet::getHeadSize() const {
    size_t s = 0;
    for(auto p: _params) {
        if (p->isDynamic()) {
            s += 32;
        } else {
            s += p->getSize();
        }
    }
    return s;
}

void ParamSet::encode(Data& data) const {
    // 2-pass encoding
    size_t headSize = getHeadSize();
    size_t dynamicOffset = 0;

    // pass 1: small values or indices
    for(auto p: _params) {
        if (p->isDynamic() || p->getSize() > Util::encodedUInt256Size) {
            // include only offset
            ABI::encode(uint256_t(headSize + dynamicOffset), data);
            dynamicOffset += p->getSize();
        } else {
            // encode small data
            p->encode(data);
        }
    }        

    // pass 2: dynamic values
    for(auto p: _params) {
        if (p->isDynamic() || p->getSize() > Util::encodedUInt256Size) {
            // encode large data
            p->encode(data);
        }
    }        
}

bool ParamSet::decode(const Data& encoded, size_t& offset_inout) {
    // pass 1: small values
    for(auto p: _params) {
        if (p->isDynamic()) {
            uint256_t index;
            if (!ABI::decode(encoded, index, offset_inout)) { return false; }
            // index is read but not used
        } else {
            if (!p->decode(encoded, offset_inout)) { return false; }
        }
    }
    // pass2: large values    
    for(auto p: _params) {
        if (p->isDynamic()) {
            if (!p->decode(encoded, offset_inout)) { return false; }
        }
    }
    return true;
}
