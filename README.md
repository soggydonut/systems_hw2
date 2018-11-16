# CSCI389 Cache Project ### by Laura Yoshida & Lucas Yong

## Description
This project was undertaken for the "CSCI389 - Computer Systems" class at Reed College in the Fall 2018 semester, in which we write a look-aside cache with key-value storage, and progressively add capabilities along the way.

## HW2 and 3

### Performance (HW2 Part 3)
The hash function is implemented as follows:
- Split the string key into an array of characters.
- Iterate over the array and take the product of the prime numbers that correspond to each ASCII char.
   (e.g. 'C' is 67th on the ASCII table, and the 67th prime is 317).
- Once we have the product, return it as the bucket number to store value associated with the key.
Since any given integer is a product of primes, the only K-V pairs sharing the same bucket would be those whose keys are permutations of each other.

### Collision Resolution (HW2 Part 4)
Taken care of by unordered_map.

### Dynamic Resizing (HW2 Part 5)
Implemented in the cache constructor, using unordered_map.

### Eviction Policy (HW2 Part 6)
Implemented FIFO in cache::set.

### Test (HW3)
For HW3, we developed unit tests and ran them on our code as well as code from three other groups. Test results were uploaded to Moodle.

## HW4
In HW4 we modified the cache so that it could be accessed using RESTful API over a network. To support this functionality we used [C++ REST SDK](https://github.com/Microsoft/cpprestsdk) (make sure to install this and run it with the appropriate linkers!).
Currently, we do not support the client and server running on different machines. To run our code for HW4 you will need to test it in two separate terminals as follows:

### 'Server Terminal':
```g++ -g -O3 -Wextra -pedantic -Wall --std=c++14 -o server server_cache.cc server.cc -lboost_system -lcrypto -lssl -lcpprest```

### 'Client Terminal':
```g++ -g -O3 -Wextra -pedantic -Wall --std=c++14 -o client client_cache.cc test.cc -lboost_system -lcrypto -lssl -lcpprest```

### Tests

### Problems/Bugs
As of now, there is an issue with our Cache where Cache::get _sometimes_ doesn't return the correct value for a partciular key. We're not sure why, but we hope to figure this out soon!
