// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "TWBase.h"

TW_EXTERN_C_BEGIN

TW_EXPORT_ENUM(uint32_t)
enum TWBlockchain {
    TWBlockchainBitcoin = 0,
    TWBlockchainEthereum = 1,
    TWBlockchainWanchain = 2,
    TWBlockchainVechain = 3,
    TWBlockchainTron = 4,
    TWBlockchainIcon = 5,
    TWBlockchainBinance = 6,
    TWBlockchainRipple = 7,
    TWBlockchainTezos = 8,
    TWBlockchainNimiq = 9,
    TWBlockchainStellar = 10,
    TWBlockchainAion = 11,
    TWBlockchainCosmos = 12,
    TWBlockchainTheta = 13,
    TWBlockchainOntology = 14,
    TWBlockchainZilliqa = 15,
    TWBlockchainIoTeX = 16,
    TWBlockchainArk = 17,
    TWBlockchainEOS = 18,
    TWBlockchainIOST = 19,
    TWBlockchainSemux = 20,
    TWBlockchainNano = 21,
    TWBlockchainNEO = 22,
    TWBlockchainSteem = 23,
    TWBlockchainWaves = 25,
    TWBlockchainAeternity = 26,
    TWBlockchainNebulas = 27,
    TWBlockchainFIO = 28,
};

TW_EXTERN_C_END
