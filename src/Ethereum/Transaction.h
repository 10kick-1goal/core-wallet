// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Address.h"
#include "../uint256.h"

namespace TW::Ethereum {

class Transaction {
public:
    uint256_t nonce;
    uint256_t gasPrice;
    uint256_t gasLimit;
    // Public key hash (Address.bytes)
    Data to;
    uint256_t amount;
    Data payload;

    // Signature values
    uint256_t v = uint256_t();
    uint256_t r = uint256_t();
    uint256_t s = uint256_t();

    // Factory methods
    // Create an ERC20 token transfer transaction
    static Transaction buildERC20Transfer(uint256_t nonce, uint256_t gasPrice, uint256_t gasLimit,
        const Data& tokenContract, const Data& toAddress, uint256_t amount);

    // Create an ERC20 approve transaction
    static Transaction buildERC20Approve(uint256_t nonce, uint256_t gasPrice, uint256_t gasLimit,
        const Data& tokenContract, const Data& spenderAddress, uint256_t amount);

    // Create an ERC721 NFT transfer transaction
    static Transaction buildERC721Transfer(uint256_t nonce, uint256_t gasPrice, uint256_t gasLimit,
        const Data& tokenContract, const Data& from, const Data& to, uint256_t tokenId);

    // Create an ERC1155 NFT transfer transaction
    static Transaction buildERC1155Transfer(uint256_t nonce, uint256_t gasPrice, uint256_t gasLimit,
        const Data& tokenContract, const Data& from, const Data& to, uint256_t tokenId, uint256_t value, const Data& data);

    // Helpers for building contract calls
    static Data buildERC20TransferCall(const Data& to, uint256_t amount);
    static Data buildERC20ApproveCall(const Data& spender, uint256_t amount);
    static Data buildERC721TransferFromCall(const Data& from, const Data& to, uint256_t tokenId);
    static Data buildERC1155TransferFromCall(const Data& from, const Data& to, uint256_t tokenId, uint256_t value, const Data& data);

public:
    Transaction(uint256_t nonce, uint256_t gasPrice, uint256_t gasLimit, const Data& to, uint256_t amount, const Data& payload = {})
        : nonce(std::move(nonce))
        , gasPrice(std::move(gasPrice))
        , gasLimit(std::move(gasLimit))
        , to(std::move(to))
        , amount(std::move(amount))
        , payload(std::move(payload)) {}
};

} // namespace TW::Ethereum
