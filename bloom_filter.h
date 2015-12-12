#pragma once
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <bitset>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include "hash/MurmurHash2.h"
#include "glog/logging.h"
using namespace std;

//#define DEBUG 1
typedef unsigned char byte_t;
typedef uint64_t hashkey_t;
const size_t BYTE_LEN = 8;
const size_t SEED = 13245;

inline size_t bitsize2byte (size_t size) {
    return size / BYTE_LEN + 1;
}
string byte2string(byte_t byte) {
    stringstream ss;
    for(size_t i = 0; i < BYTE_LEN; i ++) {
        if ((byte & ((byte_t)1<<i)) != 0) ss << "1";
        else ss << "0";
    }
    return ss.str();
}

class Bits {
public:
    Bits() { }
    Bits (size_t size): 
        _bits{ new byte_t[bitsize2byte(size)] },
        _size{size}
    { 
        LOG(WARNING) << "malloc " << bitsize2byte(size) << " bytes";
        memset(_bits.get(), 0, bitsize2byte(size));
    }
    void init(size_t size) {
        LOG(WARNING) << "malloc " << bitsize2byte(size) << " bytes";
        _bits.reset(new byte_t[bitsize2byte(size)]);
    }
    size_t size() const {
        return _size;
    }
    void set(size_t id) {
        CHECK_LT(id, size());
        _bits[id / BYTE_LEN] |= ((byte_t)1 << (id % BYTE_LEN));
    }
    void reset(size_t id) {
        CHECK_LT(id, size());
        _bits[id / BYTE_LEN] &= ~((byte_t)1 << (id % BYTE_LEN));
    }
    bool test(size_t id) const {
        CHECK_LT(id, size());
        return (_bits[id / BYTE_LEN] & ((byte_t)1 << (id % BYTE_LEN))) != 0;
    }
    const byte_t* bits() const {
        return _bits.get();
    }
    byte_t* buffer() {
        return _bits.get();
    }
    // for debug
    string tostring() const {
        stringstream ss;
        for(size_t i = 0; i < size(); i ++) {
            if (test(i)) ss << "1";
            else ss << "0";
        }
        return ss.str();
    }
private:
    std::unique_ptr<byte_t[]> _bits;
    size_t _size {0};
};

class BloomFilter {
public:
    BloomFilter() { }
    BloomFilter(size_t size, size_t n_hashes) : \
        _bits{size},
        _n_hashes{n_hashes}
    { }
    void init(size_t size, size_t n_hashes) {
        _bits.init(size);
        _bits = size;
        _n_hashes = n_hashes;
    }
    // line should be a char array
    // size of the array should be len + 1
    void set(const char* line, size_t len) {
        CHECK_EQ (strlen(line), len);
        for (int i = 0; i < _n_hashes; i++) {
            set(line, len, i);
        }
    }
    bool test(const char* line, size_t len) const {
        CHECK_EQ (strlen(line), len);
        for (int i = 0; i < _n_hashes; i++) {
            //LOG(INFO) << i << "th hash test ok";
            if (! test(line, len, i)) return false;
        }
        return true;
    }
    const Bits& bits() const {
        return _bits;
    }
    byte_t* buffer () {
        return _bits.buffer();
    }
protected:
    void set(const char* line, size_t len, int hashno) {
        hashkey_t key = hashn(line, len, hashno);
        #ifdef DEBUG
        LOG(INFO) << "hash\t" << hashno << "\t" << key;
        #endif
        _bits.set(key % _bits.size());
    }
    bool test(const char* line, size_t len, int hashno) const {
        hashkey_t key = hashn(line, len, hashno);
        #ifdef DEBUG
        LOG(INFO) << "hash\t" << hashno << "\t" << key;
        #endif
        return _bits.test(key % _bits.size());
    }
    // no: id of the hashn
    hashkey_t hashn(const char* line, size_t len, int no) const {
        hashkey_t hash1 = MurmurHash64A(line, len, SEED);
        hashkey_t hash2 = MurmurHash64B(line, len, SEED);
        return hash1 + no * hash2;
    }

private:
    Bits _bits;
    size_t _n_hashes {0};
};

// bloom filter database
class BFdb {
public:
    BFdb() { }
    BFdb(size_t n_bits, size_t n_hashes) : 
        _n_bits {n_bits},
        _n_hashes {n_hashes},
        _bf{n_bits, n_hashes}
    {
        CHECK_GT(_n_bits, 0);
        CHECK_GT(_n_hashes, 0);
    }
    BFdb(size_t n_hashes) : 
        _n_hashes{n_hashes}
    { }
    void set(const char* rcd) {
        set(rcd, strlen(rcd));
    }
    void set(const char* rcd, size_t len) {
        _bf.set(rcd, len);
    }
    bool test(const char* rcd) {
        if (strcmp(rcd, last_key.c_str()) == 0) return true;
        bool res = test(rcd, strlen(rcd));
        if (res) last_key = rcd;
        return res;
    }
    bool test(const char* rcd, size_t len) const {
        return _bf.test(rcd, len);
    }
    void save(const string& path, bool binary=true) {
        FILE* file = fopen(path.c_str(), "wb");
        // write meta info
        fwrite(&_n_bits, 1, sizeof(size_t), file);
        fwrite(&_n_hashes, 1, sizeof(size_t), file);
        // write content
        if (binary) 
            fwrite(_bf.bits().bits(), 1, bitsize2byte(_bf.bits().size()), file);
        else
            fwrite(_bf.bits().tostring().c_str(), 1, _bf.bits().size() * sizeof(char), file);
        fclose(file);
    }
    // only support binary database!
    void load(const string& path) {
        FILE* file = fopen(path.c_str(), "rb");
        // read metainfo
        fread(&_n_bits, 1, sizeof(size_t), file);
        fread(&_n_hashes, 1, sizeof(size_t), file);
        LOG(INFO) << "load n_bits:\t" << _n_bits;
        LOG(INFO) << "load n_hashes:\t" << _n_hashes;
        _bf.init(n_bits(), n_hashes());
        // read content
        size_t cur_pos = ftell(file);
        fseek(file, 0L, SEEK_END);
        size_t filesize = ftell(file);
        size_t size_of_content = filesize - cur_pos;
        CHECK_EQ(size_of_content + 2 * sizeof(size_t), filesize) << "database file is broken";
        fseek(file, cur_pos, SEEK_SET);
        fread(_bf.buffer(), 1, bitsize2byte(_n_bits), file);
        LOG(WARNING) << "finish loadding database";
        fclose(file);
    }
    size_t n_bits() const { return _n_bits; }
    size_t n_hashes() const { return _n_hashes; }

private:
    size_t _n_bits{0};
    size_t _n_hashes{0};
    BloomFilter _bf;
    string last_key;
};
