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
#include <iostream>

using namespace std;

// Global Variables
/* Keys */
string key = "foo";
string key2 = "goo";

/* string Values */
string strVal = "bar";
string strVal2 = "car";

// == HELPER FUNCTIONS ==
Cache::val_type getBlobFromString(string *str) {
    Cache::val_type blobFromString = static_cast<Cache::val_type>(str);
    return blobFromString;
}
const char* getCStringFromBlob(Cache::val_type blob) {
    return static_cast<const char*>(blob);
}

/* == Cache::get  Tests== */
// Check that we can insert a string value and get back the same value and size.
void test_insert_string() {
    Cache c(100);
    Cache::val_type blob = getBlobFromString(&strVal);
    c.set(key, blob, strVal.size());
    Cache::index_type get_size;
    const char *get_val = getCStringFromBlob(c.get(key, get_size));
    string myStr = get_val;
    assert(strVal==string(get_val));
    assert(strVal.size()==get_size);
    delete get_val;
}
// Try to get a nonexistent value from the Cache. get() should return nullptr.
void test_get_nonexistent() {
    Cache c(100);
    Cache::index_type get_size;
    Cache::val_type get_val = c.get(key, get_size);
    assert(get_val==nullptr);
}

// Check that when you set a K-V pair with an existing key (one that's already in the Cache), you get back the latest value that was set.
void test_get_modified() {
    Cache c(100);
    Cache::val_type blob1 = getBlobFromString(&strVal);
    Cache::val_type blob2 = getBlobFromString(&strVal2);
    c.set(key, blob1, strVal.size());
    c.set(key, blob2, strVal2.size());
    Cache::index_type get_size;
    const char *get_val = getCStringFromBlob(c.get(key, get_size));
    assert(strVal2==get_val);
    assert(get_size==strVal2.size());
    delete get_val;
}

// Check that you can get some value after setting it and another one after it.
void test_get_after_set() {
    Cache c(100);
    Cache::val_type blob1 = getBlobFromString(&strVal);
    Cache::val_type blob2 = getBlobFromString(&strVal2);
    c.set(key, blob1, strVal.size());
    c.set(key2, blob2, strVal2.size());
    Cache::index_type get_size;
    const char *get_val = getCStringFromBlob(c.get(key, get_size));
    assert(strVal == string(get_val));
    assert(get_size == strVal.size());
    delete get_val;
}

// Check that you cannot get a value that has already been evicted.
void test_get_evicted() {
    Cache c(5);
    Cache::val_type blob1 = getBlobFromString(&strVal);
    Cache::val_type blob2 = getBlobFromString(&strVal2);
    c.set(key, blob1, strVal.size());
    c.set(key2, blob2, strVal2.size());
    Cache::index_type get_size;
    const char *get_val = getCStringFromBlob(c.get(key, get_size));
    assert(get_val == nullptr);
}

// Check that you can get a value that triggered an eviction.
void test_get_after_eviction() {
    Cache c(5);
    Cache::val_type blob1 = getBlobFromString(&strVal);
    Cache::val_type blob2 = getBlobFromString(&strVal2);
    c.set(key, blob1, strVal.size());
    c.set(key2, blob2, strVal2.size());
    Cache::index_type get_size;
    const char *get_val = getCStringFromBlob(c.get(key2, get_size));
    assert(strVal2==string(get_val));
    delete get_val;
}

/* == Cache::set  Tests== */
// Check that you can't set a value that is larger than the Cache's maxmem.
void test_too_big() {
    Cache c(1);
    Cache::val_type blob = getBlobFromString(&strVal);
    c.set(key, blob, strVal.size());
    Cache::index_type get_size;
    const char *get_val = getCStringFromBlob(c.get(key, get_size));
    assert(get_val==nullptr);
}

/* == Cache::del  Tests== */
// Check that you can't get a value that you have deleted.
void test_deleted() {
    Cache c(100);
    Cache::val_type blob1 = getBlobFromString(&strVal);
    c.set(key, blob1, strVal.size());
    c.del(key);
    Cache::index_type get_size;
    const char *get_val = getCStringFromBlob(c.get(key, get_size));
    assert(get_val==nullptr);
}

// Check that deleting a nonexistent key does nothing.
void test_del_nonexistent() {
    Cache c(100);
    c.del(key);
}

// Check that deleting an evicted key does nothing.
void test_del_evicted() {
    Cache c(5);
    Cache::val_type blob1 = getBlobFromString(&strVal);
    Cache::val_type blob2 = getBlobFromString(&strVal2);
    c.set(key, blob1, strVal.size());
    c.set(key2, blob2, strVal2.size());
    c.del(key);
}

/* == Cache::space_used Tests== */
// Check that the space used as reported by the Cache is consistent with the actual space used.
void check_space_used() {
    Cache c(100);
    Cache::val_type blob1 = getBlobFromString(&strVal);
    Cache::val_type blob2 = getBlobFromString(&strVal2);
    Cache::index_type totalSpace = 0;
    assert(c.space_used() == totalSpace);
    c.set(key, blob1, strVal.size());
    totalSpace += strVal.size();
    assert(c.space_used() == totalSpace);
    c.set(key2, blob2, strVal2.size());
    totalSpace += strVal2.size();
    assert(c.space_used() == totalSpace);
}

int main() {
    test_insert_string();
    test_get_nonexistent();
    test_get_modified();
    test_get_after_set();
    test_get_evicted();
    test_get_after_eviction();
    test_too_big();
    test_deleted();
    test_del_nonexistent();
    test_del_evicted();
    check_space_used();
    return 0;
} 