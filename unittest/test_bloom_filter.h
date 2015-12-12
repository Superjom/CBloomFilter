#include "gtest/gtest.h"
#include "../bloom_filter.h"

TEST(Bits, init) {
    size_t size = 1024;
    Bits bits(size);
    ASSERT_EQ(size, bits.size());
}
TEST(Bits, set1) {
    size_t size = 10;
    Bits bits(size);
    for (int i = 0; i < size; i++) 
        ASSERT_FALSE(bits.test(i));
}
TEST(Bits, set2) {
    size_t size = 10;
    Bits bits(size);
    int i = 0;

    byte_t byte = 0;
    byte |= (byte_t)1;

    for (int i = 0; i < size; i ++) {
        ASSERT_FALSE(bits.test(i));
        bits.set(i);
        ASSERT_TRUE(bits.test(i));
    }
}
void test_and_set(Bits& bits, int id) {
    ASSERT_FALSE(bits.test(id));
    bits.set(id);
    ASSERT_TRUE(bits.test(id));
    bits.reset(id);
    ASSERT_FALSE(bits.test(id));
}
TEST(Bits, set) {
    size_t size = 10;
    Bits bits(size);
    for(int i = 0; i < size; i ++) 
        test_and_set(bits, i);
    //cout << "after all set\t" << bits.tostring();

}
// BloomFilter
TEST(BloomFilter, init) {
    size_t bits_num {1024};
    size_t hash_num {20};
    BloomFilter bf {bits_num, bits_num};
}
TEST(BloomFilter, set) {
    size_t bits_num {1024};
    size_t hash_num {20};
    BloomFilter bf {bits_num, bits_num};
    char q1[] = "hello world";
    ASSERT_FALSE(bf.test(q1, strlen(q1)));
    bf.set(q1, strlen(q1));
    ASSERT_TRUE(bf.test(q1, strlen(q1)));
}
// BFdb
TEST(BFdb, init) {
    size_t bits_num {1024};
    size_t hash_num {20};
    char q1[] = "hello world";

    BFdb bfdb(bits_num, hash_num);
    ASSERT_FALSE(bfdb.test(q1));
    bfdb.set(q1);
    ASSERT_TRUE(bfdb.test(q1));
}
TEST(BFdb, save) {
    size_t bits_num {10240};
    size_t hash_num {20};
    BFdb db(bits_num, hash_num);
    db.set((char*)"hello world");
    db.save("1.db");
    db.save("1.str.db", false);
}
TEST(BFdb, load) {
    BFdb db; 
    db.load("1.db");
}
