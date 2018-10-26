// test to delete a key
#include "cache.hh"
#include <functional>
#include <memory>
#include <string>
#include <assert.h>

std::string key = "foo";
Cache::val_type val = "bar";
Cache::val_type val2 = "car";
Cache::index_type size = 3;

void test_delete() {
    Cache c(10);
    c.set(key, val, size);
    c.del(key);
    Cache::val_type val = c.get(key,size);
    assert(val==nullptr);
}

void  test_insert() {
    Cache c(10);
    c.set(key, val, size);
    Cache::val_type get_val = c.get(key, size);
    assert(val==get_val);
}

void test_del_nonexistent() {
    Cache c(10);
    c.del(key);
    // what should happen here
    //assert(val==nullptr);
}

void  test_get_nonexistent() {
    Cache c(10);
    Cache::val_type get_val = c.get(key, size);
    assert(get_val==nullptr);
}


void test_get_modified() {
    Cache c(10);
    c.set(key, val, size);
    c.set(key, val2, size);
    Cache::val_type get_val = c.get(key, size);
    assert(get_val==val2);
}

/*
void test_get_evicted() {
    
}
*/

int main() {
    test_delete();
    test_insert();
    test_del_nonexistent();
    test_get_nonexistent();
    test_get_modified();
    return 0;
}