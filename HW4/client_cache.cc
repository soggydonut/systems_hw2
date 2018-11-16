// Laura Yoshida & Lucas Yong
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include "cache.hh"
#include <iostream>
 
using namespace web;
using namespace web::http;
using namespace web::http::client;

using namespace std;

// HTTP Status Codes
static const int HTTP_SUCCESS                  = 200;
static const int HTTP_SUCCESS_NO_CONTENT       = 204;
static const int HTTP_CLIENT_ERROR_BAD_REQUEST = 400;
static const int HTTP_CLIENT_ERROR_NOT_FOUND   = 404;
static const int HTTP_SERVER_INTERNAL_ERROR    = 500;

int maxmem = 200;
int portnum = 9046;

struct Cache::Impl {
    index_type maxmem_;
    hash_func hasher_;
    mutable http_client client;

    string URI() {
        ostringstream oss;
        oss << "http://localhost:" << portnum;
        return oss.str();
    }
public:
    Impl(index_type maxmem, hash_func hasher) 
    : maxmem_(maxmem), hasher_(hasher), client(http_client(URI())) {
        
    }

    ~Impl(){}

    int set(key_type key, val_type val, index_type size){
        const string *strPtr = static_cast<const string*>(val);
        ostringstream oss;
        oss << "/key/" << key << "/" << *strPtr;
        auto path = oss.str();
        uri_builder builder(path);
        auto response = client.request(methods::PUT, builder.to_string()).get();
        
        int status_code = response.status_code();
        if (status_code == HTTP_SUCCESS_NO_CONTENT) {
            return 0;
        } else {
            string message = response.extract_string().get();
            cout << message << endl;
            return -1;
        }
    }

    val_type get(key_type key, index_type& val_size) const {
        ostringstream oss;
        oss << "/key/" << key;
        auto path = oss.str();
        uri_builder builder(path);
        auto response = client.request(methods::GET, builder.to_string()).get();
        
        int status_code = response.status_code();
        if (status_code == HTTP_SUCCESS) {
            auto json = response.extract_json().get();
            string jsonStr = json["value"].as_string();
            val_size = sizeof(jsonStr)+1;

            val_type copy = new char[val_size];
            memcpy((void*)copy, &jsonStr, val_size);
            return copy;
        } else {
            string message = response.extract_string().get();
            cout << message << endl;
            return nullptr;
        }
    }
    
    // TRY THIS!!!
    int del(key_type key){
        ostringstream oss;
        oss << "/key/" << key;
        auto path = oss.str();
        uri_builder builder(path);
        auto response = client.request(methods::DEL, builder.to_string()).get();

        int status_code = response.status_code();

        if (status_code == HTTP_SUCCESS_NO_CONTENT) {
            return 0;
        } else {
            string message = response.extract_string().get();
            cout << message << endl;
            return -1;
        }
    }

    index_type space_used() const{
        auto path = "memsize";
        uri_builder builder(path);
        auto response = client.request(methods::GET, builder.to_string()).get();

        auto json = response.extract_json().get();
        index_type memused = json["memused"].as_integer();

        return memused;
    }
};

Cache::Cache(index_type maxmem, hash_func hasher) 
    : pImpl_(new Impl(maxmem,hasher)){}

Cache::~Cache(){
    // we don't need anything here because pimpl is a unique_ptr
}

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// Both the key and the value are to be deep-copied (not just pointer copied).
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
int Cache::set(key_type key, val_type val, index_type size){
    return pImpl_->set(key,val,size);
}

// Retrieve a pointer to the value associated with key in the cache,
// or NULL if not found.
// Sets the actual size of the returned value (in bytes) in val_size
Cache::val_type Cache::get(key_type key, index_type& val_size) const {
    return pImpl_->get(key,val_size);
}

// Delete an object from the cache, if it's still there
int Cache::del(key_type key){
    return pImpl_->del(key);
}

// Compute the total amount of memory used up by all cache values (not keys)
Cache::index_type Cache::space_used() const {
    return pImpl_->space_used();
}

int main() {
    Cache c(100);
    string myval = "value";
    c.set("key", &myval, sizeof(myval));
    Cache::index_type size;
    auto ptr = static_cast<const string*>(c.get("key", size));
    cout << "Val is: " << *ptr << endl;
    cout << "memused: " << c.space_used() << endl;
    return 0;
}