// Lucas Yong & Laura Yoshida
/*
 * Interface for a generic cache object.
 * Data is given as blobs (void *) of a given size,
 * and indexed by a string key.
 */

#include <functional>
#include <memory>
#include <string>

class Cache {
 private:
   // All internal data and functionality is hidden using the Pimpl idiom
   // (see here: https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/)
  struct Impl;
  std::unique_ptr<Impl> pImpl_;

 public:
  using key_type = const std::string;  // Keys for K-V pairs
  using val_type = const void*;        // Values for K-V pairs
  using index_type = uint32_t;         // Internal indexing to K-V elements

  // A function that takes a key and returns an index to the internal data
  using hash_func = std::function<uint32_t(key_type)>;

  // Create a new cache object with a given maximum memory capacity.
  Cache(index_type maxmem,
        hash_func hasher = std::hash<std::string>());

  ~Cache();

  // Disallow cache copies, to simplify memory management.
  Cache(const Cache&) = delete;
  Cache& operator=(const Cache&) = delete;

  // Add a <key, value> pair to the cache.
  // If key already exists, it will overwrite the old value.
  // Both the key and the value are to be deep-copied (not just pointer copied).
  // If maxmem capacity is exceeded, sufficient values will be removed
  // from the cache to accomodate the new value.
  // Returns 0 if no errors ocurred, Some nonzero code otherwise.
  int set(key_type key, val_type val, index_type size);

  // Retrieve a pointer to the value associated with key in the cache,
  // or nullptr if not found.
  // Sets the actual size of the returned value (in bytes) in val_size.
  // In case of an error, returns nullptr, and sets val_size to 0.
  val_type get(key_type key, index_type& val_size) const;

  // Delete an object from the cache, if it's still there
  // Returns 0 if no errors ocurred, Some nonzero code otherwise.
  int del(key_type key);

  // Compute the total amount of memory used up by all cache values (not keys)
  index_type space_used() const;
};
