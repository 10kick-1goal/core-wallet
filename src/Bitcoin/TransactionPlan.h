// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Amount.h"
#include "../proto/Bitcoin.pb.h"

namespace TW::Bitcoin {

/// Describes a preliminary transaction plan.
struct TransactionPlan {
    /// Amount to be received at the other end.
    Amount amount = 0;

    /// Maximum available amount.
    Amount availableAmount = 0;

    /// Estimated transaction fee.
    Amount fee = 0;

    /// Change.
    Amount change = 0;

    /// Selected unspent transaction outputs.
    std::vector<Bitcoin::Proto::UnspentTransaction> utxos;

    /// Zcash branch id
    std::vector<uint8_t> branchId;

    TransactionPlan() = default;

    TransactionPlan(const Proto::TransactionPlan& plan)
        : amount(plan.amount())
        , availableAmount(plan.available_amount())
        , fee(plan.fee())
        , change(plan.change())
        , utxos(plan.utxos().begin(), plan.utxos().end())
        , branchId(plan.branch_id().begin(), plan.branch_id().end()) {}

    Proto::TransactionPlan proto() const {
        auto plan = Proto::TransactionPlan();
        plan.set_amount(amount);
        plan.set_available_amount(availableAmount);
        plan.set_fee(fee);
        plan.set_change(change);
        *plan.mutable_utxos() = {utxos.begin(), utxos.end()};
        plan.set_branch_id(branchId.data(), branchId.size());
        return plan;
    }
};

} // namespace TW::Bitcoin
