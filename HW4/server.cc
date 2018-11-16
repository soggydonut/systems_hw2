// Laura Yoshida & Lucas Yong
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>
#include <string>
#include "cache.hh"
#include <memory>
#include <functional>
 
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace std;

// HTTP Status Codes
static const int HTTP_SUCCESS                  = 200;
static const int HTTP_SUCCESS_NO_CONTENT       = 204;
static const int HTTP_CLIENT_ERROR_BAD_REQUEST = 400;
static const int HTTP_CLIENT_ERROR_NOT_FOUND   = 404;
static const int HTTP_SERVER_INTERNAL_ERROR    = 500;

http_listener listener;
Cache *serverCache;

// Helper function to split request paths into substrings, contained in a vector<string>.
vector<string> splittedString(const string& str, char delimiter) {
    istringstream iss(str);
    string substring;
    vector<string> substringCollection;
    size_t i = 0;
    while(getline(iss, substring, delimiter)) {
      // Paths always begin with '/', so we ignore the first one, if not it will save an empty string as the first item.
      if (i > 0) {
         substringCollection.push_back(substring);
      }
      i++;
    }
    return substringCollection;
}

// Returns a JSON tuple with { key: k, value: v } pair (where k is the resource in the request, 
// and value is the appropriate value from the cache), or an error message if the key isn't in the cache.
void handle_get_key(http_request request, Cache::key_type key) {
   auto response = http_response();
   Cache::index_type size;
   Cache::val_type value = serverCache->get(key, size);
   if (value == nullptr) {
      // Error message?
      response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
      response.set_body("Invalid GET Request: The key you looked up does not return a value.");
      request.reply(response);
   } else {
      const string *strValuePtr = static_cast<const string*>(value);
      json::value obj;
      cout << "GETVAL: " << *strValuePtr << endl;
      obj["key"] = json::value::string(key);
      obj["value"] = json::value::string(*strValuePtr);
      response.set_status_code(HTTP_SUCCESS);
      response.set_body(obj);
   }
   request.reply(response);
}

// Returns a JSON tuple with { memused: mem } with the value returned by the Cache's memused method.
void handle_get_memsize(http_request request) {
   json::value obj;
   auto space_used = serverCache->space_used();
   obj["memused"] = json::value::number(space_used);
   request.reply(HTTP_SUCCESS, obj);
}

void handle_get(http_request request) {
   auto response = http_response();
   string path = request.request_uri().path();
   auto requestKeywords = splittedString(path, '/');
   if (requestKeywords.empty()) {
      response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
      response.set_body("Empty GET Request: Please look up a key in the cache or the memsize.");
      request.reply(response);
      return;
   }
   string GETRequestType = requestKeywords[0];
   if (GETRequestType == "key") {
      if (requestKeywords.size() < 2) {
         response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
         response.set_body("Invalid Key: Your key cannot be an empty string.");
         request.reply(response);
      } else {
         auto key = requestKeywords[1];
         handle_get_key(request, key);
      }
   } else if (GETRequestType == "memsize") {
      handle_get_memsize(request);
   } else {
      response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
      response.set_body("Invalid GET Request: Please look up a key in the cache or the memsize.");
      request.reply(response);
   }
}

// Create or replace a k,v pair in the cache.
void handle_put(http_request request) {
   auto response = http_response();
   string path = request.request_uri().path();
   auto requestKeywords = splittedString(path, '/');
   if (requestKeywords.empty()) {
      response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
      response.set_body("Empty PUT Request: Please set a value with the path /key/k/v.");
      request.reply(response);
      return;
   }
   string PUTRequestType = requestKeywords[0];
   if (PUTRequestType == "key" && requestKeywords.size() >= 3) {
      string key = requestKeywords[1];
      const string val = requestKeywords[2];
      auto blobValue = static_cast<Cache::val_type>(&val);
      int status = serverCache->set(key, blobValue, sizeof(val));
      if (status == 0) {
         response.set_status_code(HTTP_SUCCESS_NO_CONTENT);
         request.reply(response);
      } else {
         // Unable to set object in the Cache (Cache::set returned nonzero status code)
         response.set_status_code(HTTP_SERVER_INTERNAL_ERROR);
         response.set_body("PUT failed, perhaps because the object you are trying to set in the Cache is larger than maxmem. Please try again!");
         request.reply(response);
      }
   } else {
      response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
      response.set_body("Invalid PUT Request: Please set a value with the path /key/k/v.");
      request.reply(response);
   }
}

void handle_del(http_request request) {
   auto response = http_response();
   string path = request.request_uri().path();
   auto requestKeywords = splittedString(path, '/');
   if (requestKeywords.empty()) {
      response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
      response.set_body("Empty DELETE Request: Please delete a value with the path /key/k.");
      request.reply(response);
      return;
   }
   string DELRequestType = requestKeywords[0];
   if (DELRequestType == "key" && requestKeywords.size() >= 2) {
      string key = requestKeywords[1];
      int status = serverCache->del(key);
      if (status == 0) {
         // Objected successfully deleted
         response.set_status_code(HTTP_SUCCESS_NO_CONTENT);
         request.reply(response);
      } else {
         // Unable to delete object from the Cache (Cache::del returned nonzero status code)
         response.set_status_code(HTTP_SERVER_INTERNAL_ERROR);
         response.set_body("DELETE failed, probably because the object you are trying to delete does not exist in the Cache. Please try again!");
         request.reply(response);
      }   
   } else {
      response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
      response.set_body("Invalid DELETE Request: Please set a value with the path /key/k.");
      request.reply(response);
   }
} 

// Return just a header, regardless of the resource (key) requested.
// Header lines are HTTP version, Date, Accept, and Content-Type with application/json.
void handle_head(http_request request) {
   auto response = http_response(HTTP_SUCCESS);
   response.headers().set_date(utility::datetime::utc_now());
   response.headers().set_content_type("application/json");
   response.headers().add("Accept", "text/html");
   request.reply(response);
}

// Upon receiving this message, the server stops accepting requests, 
// finishes up any requests in-flight, cleans up the cache (frees resources) and exits cleanly.
void handle_post(http_request request) {
   auto response = http_response();
   string path = request.request_uri().path();
   auto requestKeywords = splittedString(path, '/');
   if (requestKeywords.empty()) {
      response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
      response.set_body("Empty POST Request: Please shut down the server with /shutdown.");
      request.reply(response);
      return;
   }
   string POSTRequestType = requestKeywords[0];
   if (POSTRequestType == "shutdown" && requestKeywords.size() == 1) {
      response.set_status_code(HTTP_SUCCESS);
      response.set_body("Goodbye!");
      request.reply(response);
      delete serverCache;
      listener.close().wait();
      
   } else {
      response.set_status_code(HTTP_CLIENT_ERROR_BAD_REQUEST);
      response.set_body("Invalid POST Request:   Please shut down the server with /shutdown.");
      request.reply(response);
   }
}

int main(int argc, char **argv) {
   int opt;
   int maxmem = 100;
   int portnum = 9046;
   while ((opt = getopt(argc, argv, "m:t:")) != -1) {
       switch (opt) {
         case 'm':
            maxmem = atoi(optarg);
            break;
         case 't':
            portnum = atoi(optarg);
            break;
         default:
            fprintf(stderr, "Usage: %s [-m max cache memory] [-t port number]\n", argv[0]);
            exit(EXIT_FAILURE);
       }
   }
   std::cout << "maxmem: " << maxmem << ", portnum: " << portnum << "\n";

   serverCache = new Cache(maxmem);

   // Setting up the listener
   ostringstream oss;
   oss << "http://localhost:" << portnum;
   string uri = oss.str();
   listener = http_listener(uri);
   listener.support(methods::GET,  handle_get);
   listener.support(methods::PUT,  handle_put);
   listener.support(methods::DEL,  handle_del);
   listener.support(methods::HEAD, handle_head);
   listener.support(methods::POST, handle_post);
 
   try {
      listener
         .open()
         .wait();
 
      while (true);
   }
   catch (exception const & e) {
      wcout << e.what() << endl;
   }
   return 0;
}