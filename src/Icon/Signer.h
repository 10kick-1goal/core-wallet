// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Data.h"
#include "../proto/Icon.pb.h"

#include <map>
#include <string>

namespace TW::Icon {

class Signer {
  public:
    const Proto::SigningInput& input;

    Signer(const Proto::SigningInput& input) : input(input) {}

    /// Signs an Icon transaction.
    Proto::SigningOutput sign() const noexcept;

    /// Signature pre-image.
    std::string preImage() const noexcept;

    /// Encodes a signed transaction as JSON.
    std::string encode(const Data& signature) const noexcept;

  private:
    std::map<std::string, std::string> parameters() const noexcept;
};

} // namespace TW::Icon
