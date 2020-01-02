// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Util.h"

#include "Base64.h"
#include "HexCoding.h"
#include "Data.h"

#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>

namespace TW::WalletConsole {

using namespace std;
using namespace TW;

bool Util::hex(const string& p, string& res) {
    res = TW::hex(data(p));
    return true;
}

bool Util::base64Encode(const string& p, string& res) {
    try {
        Data data = parse_hex(p);
        try {
            res = Base64::encode(data);
            return true;
        } catch (exception& ex) {
            cout << "Error while Base64 encode" << endl;
            return false;
        }
    } catch (exception& ex) {
        cout << "Error decoding input as hex" << endl;
        return false;
    }
}

bool Util::base64Decode(const string& p, string& res) {
    try {
        auto dec = Base64::decode(p);
        res = TW::hex(dec);
        return true;
    } catch (exception& ex) {
        cout << "Error while Base64 decode" << endl;
        return false;
    }
}

bool Util::fileW(const string& fileName, const string& data, string& res, ostream& out) {
    if (fileExists(fileName)) {
        out << "Warning: File '" << fileName << "' already exists, not overwriting to be safe." << endl;
        return false;
    }
    try {
        ofstream outfile(fileName,  std::ios::out | std::ios::binary);
        Data bindata = parse_hex(data);
        outfile.write((const char*)bindata.data(), bindata.size());
        outfile.close();
        out << "Written to file '" << fileName << "', " << bindata.size() << " bytes." << endl;
    } catch (exception& ex) {
        out << "Error writing to file '" << fileName << "': " << ex.what() << endl;
    }
    return false;
}

bool Util::fileR(const string& fileName, string& res, ostream& out) {
    if (!fileExists(fileName)) {
        out << "Error: File not found '" << fileName << "'" << endl;
        return false;
    }
    try {
        ifstream infile(fileName,  std::ios::in | std::ios::binary);
        // get length of file:
        infile.seekg (0, infile.end);
        int length = infile.tellg();
        infile.seekg (0, infile.beg);
        char* buffer = new char[length];
        if (!infile.read(buffer, length)) {
            out << "Could not read file '" << fileName << "'" << endl;
            return false;
        }
        int red = infile.gcount();
        infile.close();
        res = string(TW::hex(data((const byte*)buffer, red)));
        delete[] buffer;
        out << "Read " << red << " bytes from file '" << fileName << "'." << endl;
        return true;
    } catch (exception& ex) {
        out << "Error reading from file '" << fileName << "': " << ex.what() << endl;
        return false;
    }
}

vector<string> Util::tokenize(const string& line) {
    vector<string> tok;
    size_t n = line.length();
    size_t idx = 0;
    while (true) {
        auto spaceIdx = line.find(' ', idx);
        if (spaceIdx == string::npos || spaceIdx >= n) {
            // no more space
            if (idx < n) {
                tok.push_back(line.substr(idx, n - idx));
            }
            return tok;
        }
        // there is a space, take current word up to it
        if (idx < spaceIdx) {
            tok.push_back(line.substr(idx, spaceIdx - idx));
        }
        idx = spaceIdx + 1;
    }
}

void Util::trimLeft(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

void Util::toLower(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c){ return std::tolower(c); });
}

bool Util::fileExists(const std::string& fileName) {
    //return filesystem::exists(filesystem::path(fileName)); // not used due to compiler issues (on iOS)
    ifstream f(fileName.c_str());
    return f.good();
}

} // namespace TW::WalletConsole
