#include "cache.hh"
#include <unordered_map>
#include <iostream>

using namespace std;

// Our Hash Function, implemented as a functor.
struct MyHashFunc {
	uint32_t operator() (Cache::key_type &key) const {
		return hash<string>()(key);
	}
};

struct Cache::Impl {
public:
    // Struct members, mirroring those in the Cache class
    index_type maxmem_;
    evictor_type evictor_;
    hash_func hasher_;
    index_type memused_;

    // Unordered map, which is the data structure we are currently using for our cache (this may change? :0)
    unordered_map<std::string, void*, MyHashFunc> my_map;

    Impl(index_type maxmem, evictor_type evictor, hash_func hasher) 
    : maxmem_(maxmem), evictor_(evictor), hasher_(hasher), memused_(0) {}

    ~Impl() = default;

    // Implements Cache::set
    // Sets my_map[key] = val
    void set(key_type key, val_type val, index_type size) {
        string key_ = key;
        void *val_ = const_cast<void*>(val);
        my_map.emplace(key_, val_);
        memused_ += size;
    }
    // Implements Cache::get
    // Returns the value at my_map[key]
    val_type get(key_type key, index_type& val_size) const {
        string key_ = key;
        auto val = my_map.at(key_);
        return val;
    }
    // Deletes value (and key) at my_map[key]
    void del(key_type key) {
        my_map.erase(key);
    }
    // Returns the total memory used
    index_type space_used() const {
        return memused_;
    }
};

// Create a new cache object with a given maximum memory capacity.
Cache::Cache(index_type maxmem, evictor_type evictor, hash_func hasher)
: pImpl_(new Impl(maxmem, evictor, hasher)){
}

Cache::~Cache() {
	// I don't really even know if we need to delete the pImpl because they didn't do it in the example But we should ask Eitan.
	Impl *pImpl__ = pImpl_.release();
	delete pImpl__;
}

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// Both the key and the value are to be deep-copied (not just pointer copied).
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void Cache::set(key_type key, val_type val, index_type size) {
    pImpl_->set(key, val, size);
}

// Retrieve a pointer to the value associated with key in the cache,
// or NULL if not found.
// Sets the actual size of the returned value (in bytes) in val_size.
Cache::val_type Cache::get(key_type key, index_type& val_size) const {
    return pImpl_->get(key, val_size);
}

// Delete an object from the cache, if it's still there
void Cache::del(key_type key) {
    pImpl_->del(key);
}

// Compute the total amount of memory used up by all cache values (not keys)
Cache::index_type Cache::space_used() const {
    return pImpl_->space_used();
}

// I just ran a bunch of tests in main to make sure the cache works (which I think it does for now).
int main() {
    auto myCache = new Cache(10);
    uint32_t num = 55;
    uint32_t num2 = 99;
    
    myCache->set("test", &num, 3);
    cout << myCache->space_used() << endl;
    myCache->set("test2", &num2, 33);
    cout << myCache->space_used() << endl;
    auto prt = myCache->get("test", num);
    auto prt2 = myCache->get("test2", num);
    // We need this 'reinterpret casts' to change void* to uint32_t* 
    //(I'm just assuming the values are gonna be numbers, but I guess they could be anything?? Maybe we should ask Eitan )
    auto *pInt = reinterpret_cast<const uint32_t*>(prt);
    auto *pInt2 = reinterpret_cast<const uint32_t*>(prt2);
    cout << "val:" << *pInt << endl;
    cout << "val2:" << *pInt2 << endl;
    delete myCache;
	return 0;
}





