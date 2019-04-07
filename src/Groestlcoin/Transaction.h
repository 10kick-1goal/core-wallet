// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Bitcoin/Transaction.h"

namespace TW::Groestlcoin {

struct Transaction : public Bitcoin::Transaction {
    Transaction() : Bitcoin::Transaction(1, 0, static_cast<Data (*)(const byte*, const byte*)>(TW::Hash::sha256)) {}
    Transaction(int32_t version, uint32_t lockTime) :
        Bitcoin::Transaction(version, lockTime, static_cast<Data (*)(const byte*, const byte*)>(TW::Hash::sha256)) {}
};

} // namespace TW::Groestlcoin
