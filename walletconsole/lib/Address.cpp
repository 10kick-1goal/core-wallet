// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"

#include "WalletConsole.h"
#include "Coin.h"
#include "Data.h"
#include "PrivateKey.h"
#include "HexCoding.h"
#include "HDWallet.h"

#include <iostream>
#include <vector>
#include <cassert>

namespace TW::WalletConsole {

using namespace std;

bool Address::addrPub(const string& coinid, const string& pubkey_in, string& res) {
    Coin coin;
    if (!_coins.findCoin(coinid, coin)) { return false; }
    Data pubDat;
    try {
        pubDat = parse_hex(pubkey_in);
    } catch (exception& ex) {
        _out << "Error: could not parse public key data" << endl;
        return false; 
    }
    TWCoinType ctype = (TWCoinType)coin.c;
    PublicKey pubKey = PublicKey(pubDat, (TWPublicKeyType)coin.pubKeyType);
    res = TW::deriveAddress(ctype, pubKey);
    return true;
}

bool Address::addrPri(const string& coinid, const string& prikey_in, string& res) {
    Coin coin;
    if (!_coins.findCoin(coinid, coin)) { return false; }
    Data priDat;
    try {
        priDat = parse_hex(prikey_in);
    } catch (exception& ex) {
        _out << "Error: could not parse private key data" << endl;
        return false; 
    }
    TWCoinType ctype = (TWCoinType)coin.c;
    PrivateKey priKey = PrivateKey(priDat);
    res = TW::deriveAddress(ctype, priKey);
    return true;
}

bool Address::addr(const string& coinid, const string& addrStr, string& res) {
    Coin coin;
    if (!_coins.findCoin(coinid, coin)) { return false; }
    TWCoinType ctype = (TWCoinType)coin.c;
    if (!TW::validateAddress(ctype, addrStr)) {
        _out << "Address is not a valid " << coin.name << " address! " << addrStr << endl;
        return false;
    }
    _out << "Address is a valid " << coin.name << " address:  " << addrStr << endl;
    return false;
}

bool Address::addrDefault(const string& coinid, string& res) {
    Coin coin;
    if (!_coins.findCoin(coinid, coin)) { return false; }
    TWCoinType ctype = (TWCoinType)coin.c;
    string mnemo = _keys.getMnemo();
    if (mnemo.length() == 0) {
        _out << "Error: no mnemonic set." << endl;
        return false;
    }
    HDWallet wallet(mnemo, "");

    string addr = wallet.deriveAddress(ctype);

    res = addr;
    return true;
}

bool Address::addrDP(const string& coinid, const string& derivPath, string& res) {
    DerivationPath dp(derivPath);
    // get the private key
    string mnemo = _keys.getMnemo();
    if (mnemo.length() == 0) {
        _out << "Error: no mnemonic set." << endl;
        return false;
    }
    HDWallet wallet(mnemo, "");
    PrivateKey priKey = wallet.getKey(dp);

    Coin coin;
    if (!_coins.findCoin(coinid, coin)) { return false; }
    TWCoinType ctype = (TWCoinType)coin.c;

    // derive address
    res = TW::deriveAddress(ctype, priKey);
    return true;
}

} // namespace TW::WalletConsole
