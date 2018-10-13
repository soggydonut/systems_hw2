#include "cache.hh"
#include <unordered_map>

using namespace std;

struct Cache::Impl {
public:
    unordered_map<string, void*> my_map;

    index_type maxmem_;
    evictor_type evictor_;
    hash_func hasher_;
    index_type memused_;

    Impl(index_type maxmem, evictor_type evictor, hash_func hasher) 
    : maxmem_(maxmem), evictor_(evictor), hasher_(hasher), memused_(0) {
    	my_map = {};
    }

    ~Impl() = default;

    void set(key_type key, val_type val, index_type size) {
    	val_type val_ = new val_type;
    	*val_ = *(val);
       my_map[key] = val_;
    }
    val_type get(key_type key, index_type& val_size) const {
    	val_type val_ = new val_type;
    	*val_ = *(my_map[key]);
    	return val_;
    }
    void del(key_type key) {
        my_map.erase(key);
    }
    index_type space_used() const {
        return memused_;
    }
};

// Create a new cache object with a given maximum memory capacity.
Cache::Cache(index_type maxmem, evictor_type evictor, hash_func hasher) {

}

Cache::~Cache() = default;

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// Both the key and the value are to be deep-copied (not just pointer copied).
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void Cache::set(key_type key, val_type val, index_type size) {
    //pImpl->privateCache_[key] = val;
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


int main() {
	return 0;
}





