// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Data.h"
#include "../HexCoding.h"

#include <nlohmann/json.hpp>
#include <optional>

namespace TW::Keystore {

enum class ScryptValidationError {
    desiredKeyLengthTooLarge,
    blockSizeTooLarge,
    invalidCostFactor,
    overflow,
};

/// Scrypt function parameters.
struct ScryptParameters {
    /// The N parameter of Scrypt encryption algorithm, using 256MB memory and
    /// taking approximately 1s CPU time on a modern processor.
    static const uint32_t standardN = 1 << 18;

    /// The P parameter of Scrypt encryption algorithm, using 256MB memory and
    /// taking approximately 1s CPU time on a modern processor.
    static const uint32_t standardP = 1;

    /// The N parameter of Scrypt encryption algorithm, using 4MB memory and
    /// taking approximately 100ms CPU time on a modern processor.
    static const uint32_t lightN = 1 << 12;

    /// The P parameter of Scrypt encryption algorithm, using 4MB memory and
    /// taking approximately 100ms CPU time on a modern processor.
    static const uint32_t lightP = 6;

    /// Default `R` parameter of Scrypt encryption algorithm.
    static const uint32_t defaultR = 8;

    /// Default desired key length of Scrypt encryption algorithm.
    static const std::size_t defaultDesiredKeyLength = 32;

    /// Random salt.
    Data salt;

    /// Desired key length in bytes.
    std::size_t desiredKeyLength = defaultDesiredKeyLength;

    /// CPU/Memory cost factor.
    uint32_t n = lightN;

    /// Parallelization factor (1..232-1 * hLen/MFlen).
    uint32_t p = lightP;

    /// Block size factor.
    uint32_t r = defaultR;

    /// Initializes with default scrypt parameters and a random salt.
    ScryptParameters();

    /// Initializes `ScryptParameters` with all values.
    ///
    /// @throws ScryptValidationError if the parameters are invalid.
    ScryptParameters(Data salt, uint32_t n, uint32_t r, uint32_t p, std::size_t desiredKeyLength)
        : salt(std::move(salt)), desiredKeyLength(desiredKeyLength), n(n), p(p), r(r) {
        auto error = validate();
        if (error) {
            throw *error;
        }
    }

    /// Validates the parameters.
    ///
    /// - Returns: a `ValidationError` or `nil` if the parameters are valid.
    std::optional<ScryptValidationError> validate() const;

    /// Initializes `ScryptParameters` with a JSON object.
    ScryptParameters(const nlohmann::json& json);

    /// Saves `this` as a JSON object.
    nlohmann::json json() const;
};

} // namespace TW::Keystore
