// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "FeeCalculator.h"

#include <cmath>

using namespace TW;

namespace TW::Bitcoin {

int64_t DefaultFeeCalculator::calculate(int64_t inputs, int64_t outputs, int64_t byteFee) const {
    const auto txsize = ((148 * inputs) + (34 * outputs) + 10);
    return int64_t(txsize) * byteFee;
}

int64_t DefaultFeeCalculator::calculateSingleInput(int64_t byteFee) const {
    return int64_t(148) * byteFee;
}

int64_t SegwitFeeCalculator::calculate(int64_t inputs, int64_t outputs, int64_t byteFee) const {
    const auto txsize = int64_t(std::ceil(101.25 * inputs + 31.0 * outputs + 10.0));
    return txsize * byteFee;
}

int64_t SegwitFeeCalculator::calculateSingleInput(int64_t byteFee) const {
    return int64_t(102) * byteFee; // std::ceil(101.25) = 102
}

class ZCashFeeCalculator : public FeeCalculator {
public:
    int64_t calculate(int64_t inputs, int64_t outputs = 2, int64_t byteFee = 1) const override { return 10000; }
    int64_t calculateSingleInput(int64_t byteFee) const override { return 0; }
};

class GroestlcoinFeeCalculator : public FeeCalculator {
public:
    int64_t calculate(int64_t inputs, int64_t outputs = 2, int64_t byteFee = 1) const override { return 20000; }
    int64_t calculateSingleInput(int64_t byteFee) const override { return 0; }
};

class DecredFeeCalculator : public FeeCalculator {
public:
    int64_t calculate(int64_t inputs, int64_t outputs = 2, int64_t byteFee = 1) const override {
        const auto txsize = ((166 * inputs) + (38 * outputs) + 12);
        return int64_t(txsize) * byteFee;
    }

    int64_t calculateSingleInput(int64_t byteFee) const override {
        return int64_t(166) * byteFee;
    }
};

DefaultFeeCalculator defaultFeeCalculator;
ZCashFeeCalculator zcashFeeCalculator;
GroestlcoinFeeCalculator groestlcoinFeeCalculator;
DecredFeeCalculator decredFeeCalculator;
SegwitFeeCalculator segwitFeeCalculator;

FeeCalculator& getFeeCalculator(TWCoinType coinType) {
    switch (coinType) {
    case TWCoinTypeZelcash:
    case TWCoinTypeZcash:
        return zcashFeeCalculator;

    case TWCoinTypeGroestlcoin:
        return groestlcoinFeeCalculator;

    case TWCoinTypeDecred:
        return decredFeeCalculator;

    case TWCoinTypeBitcoin:
    case TWCoinTypeBitcoinGold:
    case TWCoinTypeDigiByte:
    case TWCoinTypeLitecoin:
    case TWCoinTypeViacoin:
        return segwitFeeCalculator;

    default:
        return defaultFeeCalculator;
    }
}

} // namespace TW::Bitcoin
