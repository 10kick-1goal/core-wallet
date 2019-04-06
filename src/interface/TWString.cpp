// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.


#include <TrustWalletCore/TWString.h>
#include <string>

TWString *_Nonnull TWStringCreateWithUTF8Bytes(const char *_Nonnull bytes) {
    auto s = new std::string(bytes);
    return s;
}

size_t TWStringSize(TWString *_Nonnull string) {
    auto s = reinterpret_cast<const std::string*>(string);
    return s->size();
}

char TWStringGet(TWString *_Nonnull string, size_t index) {
    auto s = reinterpret_cast<const std::string*>(string);
    return (*s)[index];
}

const char *_Nonnull TWStringUTF8Bytes(TWString *_Nonnull string) {
    auto s = reinterpret_cast<const std::string*>(string);
    return s->data();
}

void TWStringDelete(TWString *_Nonnull string) {
    auto s = reinterpret_cast<const std::string*>(string);
    delete s;
}
