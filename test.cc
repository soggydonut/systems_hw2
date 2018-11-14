/*
CSCI389 Homework 3
By Laura Yoshida & Lucas Yong

Description
Unit Tests for a Cache Implementation in C++

This series of unit tests is grouped according to the four methods in the Cache API, namely get, set, del, and space_used.
Each group will test a specific case of its method. The test passes if the sole logical assert in the test function passes, and vice-versa.

Some helper functions are written in the 'helper' section so that we don't need to keep rewriting certain lines of code across different unit tests.

We are testing:
1) Kathy & Rhody
2) Ezra & Joe
3) Henry & Noah
4) Our own code
*/
#include "cache.hh"
#include <functional>
#include <memory>
#include <cstring>
#include <assert.h>

using namespace std;

// Global Variables
/* Keys */
string key = "foo";
string key2 = "goo";

/* uint32_t Values */
uint32_t numVal = 57;
uint32_t numVal2 = 85;

/* string Values */
string strVal = "bar";
string strVal2 = "car";

// Default Hash Function 
/*
Implemented as a functor by overloading the operator ().
The algorithm is:
- Split the string key into an array of characters
- Iterate over the array and take the product of the prime numbers that correspond to each ASCII char
   (e.g. 'C' is 67th on the ASCII table, and the 67th prime is 317)
Once we have the product, return it as the bucket number to store value associated with the key
Since any given integer is a product of primes, the only K-V pairs sharing the same bucket would be those whose keys are permutations of each other
*/
struct MyHashFunc {
    // An array of the first 255 primes
    const uint32_t PRIME_NUMBERS[255] = {  2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89,
                                        97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 
                                        193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293,
                                        307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 
                                        421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 
                                        547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 
                                        661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 
                                        811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 
                                        947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 
                                        1063, 1069, 1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 
                                        1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 
                                        1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 
                                        1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 
                                        1583, 1597, 1601, 1607, 1609, 1613};

    uint32_t operator() (Cache::key_type &key) const {
        const uint32_t keyLength = key.length();
        char keyArr[keyLength];
        strcpy(keyArr, key.c_str());
        uint32_t hashVal = 1;
        for (uint32_t i=0; i < keyLength; i++) {
            uint32_t charNum = keyArr[i];
            hashVal *= PRIME_NUMBERS[charNum];
        }
        return hashVal;
    }
};

// == HELPER FUNCTIONS ==
Cache::val_type getBlobFromNum(uint32_t *num) {
    Cache::val_type blobFromNum = reinterpret_cast<Cache::val_type>(num);
    return blobFromNum;
}
Cache::val_type getBlobFromString(string *str) {
    Cache::val_type blobFromString = reinterpret_cast<Cache::val_type>(str);
    return blobFromString;
}
uint32_t getNumFromBlob(Cache::val_type blob) {
    const uint32_t *numFromBlob = reinterpret_cast<const uint32_t*>(blob);
    return *numFromBlob;
}
string getStringFromBlob(Cache::val_type blob) {
    const string *stringFromBlob = reinterpret_cast<const string*>(blob);
    return *stringFromBlob;
}

/* == Cache::get  Tests== */
// Check that we can insert a string value and get back the same value and size.
void test_insert_string() {
    Cache c(100, MyHashFunc());
    Cache::val_type blob = getBlobFromString(&strVal);
    c.set(key, blob, sizeof(strVal));
    Cache::index_type get_size;
    string get_val = getStringFromBlob(c.get(key, get_size));
    assert(strVal==get_val);
    assert(sizeof(strVal)==get_size);
}

// Check that we can insert an integer value and get back the same value and size.
// *** changed to num instead of char ***
void test_insert_num() {
    Cache c(100, MyHashFunc());
    Cache::val_type blob = getBlobFromNum(&numVal);
    c.set(key, blob, sizeof(numVal));
    Cache::index_type get_size;
    uint32_t get_val = getNumFromBlob(c.get(key, get_size));
    assert(numVal==get_val);
    assert(sizeof(numVal)==get_size);
}

// Try to get a nonexistent value from the Cache. get() should return nullptr.
void test_get_nonexistent() {
    Cache c(100, MyHashFunc());
    Cache::index_type get_size;
    Cache::val_type get_val = c.get(key, get_size);
    assert(get_val==nullptr);
}

// Check that when you set a K-V pair with an existing key (one that's already in the Cache), you get back the latest value that was set.
void test_get_modified() {
    Cache c(100, MyHashFunc());
    Cache::val_type blob1 = getBlobFromNum(&numVal);
    Cache::val_type blob2 = getBlobFromNum(&numVal2);
    c.set(key, blob1, sizeof(numVal));
    c.set(key, blob2, sizeof(numVal2));
    Cache::index_type get_size;
    uint32_t get_val = getNumFromBlob(c.get(key, get_size));
    assert(get_val==numVal2);
    assert(get_size==sizeof(numVal2));
}

// Check that you can get some value after setting it and another one after it.
void test_get_after_set() {
    Cache c(100, MyHashFunc());
    Cache::val_type blob1 = getBlobFromNum(&numVal);
    Cache::val_type blob2 = getBlobFromNum(&numVal2);
    c.set(key, blob1, sizeof(numVal));
    c.set(key2, blob2, sizeof(numVal2));
    Cache::index_type get_size;
    uint32_t get_val = getNumFromBlob(c.get(key, get_size));
    assert(get_val==numVal);
    assert(get_size==sizeof(numVal));
}

// Check that you cannot get a value that has already been evicted.
void test_get_evicted() {
    Cache c(5, MyHashFunc());
    Cache::val_type blob1 = getBlobFromNum(&numVal);
    Cache::val_type blob2 = getBlobFromNum(&numVal2);
    c.set(key, blob1, sizeof(numVal));
    c.set(key2, blob2, sizeof(numVal2));
    Cache::index_type get_size;
    Cache::val_type get_val = c.get(key, get_size);
    assert(get_val == nullptr);
}

// Check that you can get a value that triggered an eviction.
void test_get_after_eviction() {
    Cache c(5, MyHashFunc());
    Cache::val_type blob1 = getBlobFromNum(&numVal);
    Cache::val_type blob2 = getBlobFromNum(&numVal2);
    c.set(key, blob1, sizeof(numVal));
    c.set(key2, blob2, sizeof(numVal2));
    Cache::index_type get_size;
    uint32_t get_val = getNumFromBlob(c.get(key2, get_size));
    assert(get_val == numVal2);
}

/* == Cache::set  Tests== */
// Check that you can't set a value that is larger than the Cache's maxmem.
void test_too_big() {
    Cache c(1, MyHashFunc());
    Cache::val_type blob = getBlobFromNum(&numVal);
    c.set(key, blob, sizeof(numVal));
    Cache::index_type get_size;
    Cache::val_type get_val = c.get(key, get_size);
    assert(get_val==nullptr);
}

/* == Cache::del  Tests== */
// Check that you can't get a value that you have deleted.
void test_delete() {
    Cache c(100, MyHashFunc());
    Cache::val_type blob1 = getBlobFromNum(&numVal);
    c.set(key, blob1, sizeof(numVal));
    c.del(key);
    Cache::index_type get_size;
    Cache::val_type get_val = c.get(key,get_size);
    assert(get_val==nullptr);
}

// Check that deleting a nonexistent key does nothing.
void test_del_nonexistent() {
    Cache c(100, MyHashFunc());
    c.del(key);
}

// Check that deleting an evicted key does nothing.
void test_del_evicted() {
    Cache c(5, MyHashFunc());
    Cache::val_type blob1 = getBlobFromNum(&numVal);
    Cache::val_type blob2 = getBlobFromNum(&numVal2);
    c.set(key, blob1, sizeof(numVal));
    c.set(key2, blob2, sizeof(numVal2));
    c.del(key);
}

/* == Cache::space_used Tests== */
// Check that the space used as reported by the Cache is consistent with the actual space used.
void check_space_used() {
    Cache c(100, MyHashFunc());
    Cache::val_type blob1 = getBlobFromNum(&numVal);
    Cache::val_type blob2 = getBlobFromNum(&numVal2);
    Cache::index_type totalSpace = 0;
    assert(c.space_used() == totalSpace);
    c.set(key, blob1, sizeof(numVal));
    totalSpace += sizeof(numVal);
    assert(c.space_used() == totalSpace);
    c.set(key2, blob2, sizeof(numVal2));
    totalSpace += sizeof(numVal2);
    assert(c.space_used() == totalSpace);
}

int main() {
    test_insert_string(); // FAIL
    test_insert_num(); // FAIL
    test_get_nonexistent(); // PASS
    test_get_modified(); // FAIL
    test_get_after_set(); // FAIL
    test_get_evicted(); // PASS
    test_get_after_eviction(); // PASS
    test_too_big(); // FAIL
    test_delete(); // PASS
    test_del_nonexistent(); // PASS
    test_del_evicted(); //PASS
    check_space_used(); // PASS
    return 0;
} 