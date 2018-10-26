// Laura Yoshida & Lucas Yong
#include "cache.hh"
#include <unordered_map>
#include <iostream>
#include <vector>

using namespace std;

struct Cache::Impl {
public:
    // Impl Struct members, mirroring those in the Cache class.
    index_type maxmem_;
    evictor_type evictor_;
    index_type memused_;

    // Keep track of keys so that we can implement FIFO Eviction
    vector<string> keys;

    // Unordered Map, which is the data structure we are use to implement our cache.
    unordered_map<std::string, void*, hash_func> my_map;

    // Constructor
    Impl(index_type maxmem, evictor_type evictor, hash_func hasher) 
    : maxmem_(maxmem), evictor_(evictor), memused_(0), my_map(0, hasher) {
        // Dynamic Resizing: Resize the hash table when load factor > 0.5.
        my_map.max_load_factor(0.5);
    }

    // Default Destructor
    ~Impl() = default;

    // Implements Cache::set
    void set(key_type key, val_type val, index_type size) {
        // Needs work!! implement FIFO evictor
        string key_ = key;
        void *val_ = const_cast<void*>(val);
        memused_ += size;
        if (memused_ > maxmem_) {
            key_type deleteKey = keys[0];
            del(deleteKey);
            keys.erase(keys.begin());
        }
        my_map.emplace(key_, val_);
        keys.push_back(key_);
         
    }
    // Implements Cache::get
    val_type get(key_type key, index_type& val_size) const {
        string key_ = key;
        unordered_map<std::string, void*>::const_iterator haveKey = my_map.find(key_);
        if (haveKey == my_map.end()) {return nullptr;}
        auto val = my_map.at(key_);
        val_size = sizeof(val);
        return val;
    }
    // Implements Cache::del
    void del(key_type key) {
        my_map.erase(key);
    }
    // Implements Cache::space_used
    index_type space_used() const {
        return memused_;
    }
};

// Create a new cache object with a given maximum memory capacity.
Cache::Cache(index_type maxmem, evictor_type evictor, hash_func hasher)
: pImpl_(std::make_unique<Impl>(maxmem, evictor, hasher)){
}

Cache::~Cache() {
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
