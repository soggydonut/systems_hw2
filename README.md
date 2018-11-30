# CSCI389 Cache Project
### by Laura Yoshida & Lucas Yong

## Description
This project was undertaken for the "CSCI389 - Computer Systems" class at Reed College in the Fall 2018 semester, in which we write a look-aside Cache with key-value storage, and progressively add capabilities along the way.

## HW5
### Jain's Ten Steps
#### 1) Goals & Definition of System Boundaries
The goal of this benchmark effort is to determine the impact of both the volume of Cache requests (stress) and the space used in the Cache (load) on the response time (in milliseconds). The Cache in question is the one we have developed as of HW4, where a client interacts with the Client Cache API (in ```client_cache.cc```) that makes calls to a Server (in ```server.cc```), which in turn stores keys and values in its Backing Store (in ```server_cache.cc```). With the goals and the basic outline of the system in mind, our system boundaries include all the aforementioned components that comprise our Cache, internal to the system we have created.

#### 2) System Services & Possible Outcomes
##### Our Cache has the following system services (with outcomes described for each service):
(i)   ```Cache::get```
If the get query is successful, this service retrieves a value from the Cache based on the associated key (as input by the user), and also returns the size of that value. If it is unsuccessful, it will simply return a nullptr (with size 0).
(ii)  ```Cache::set```
A successful set query will add a key-value pair to the Cache, overwriting the old value if the key already exists and evicting (using FIFO) an old value if the new addition causes the maximum memory to be exceeded. If it is unsuccessful, it will simply do nothing and return a nonzero value.
(iii) ```Cache::del```
A successful delete query removes a key-value pair, if it is in the cache. If it does not exist or some other error occurs, it will do nothing and return zero.
(iv)  ```Cache::space_used```
This query returns the current amount of memory being used, which is the only possible outcome.

#### 3) Performance Metrics
(i) **Sustained Throughput**, defined (by Eitan) as the maximum offered load (in requests per second) at which the mean response time remains under 1 millisecond.
(ii) **Load Testing**: We want to test the impact of spaced used in the Cache on the speed of response times for GET. GET was chosen because it is crucial method that does not increase or decrease the space used in itself and thus we can effectively and independently measure how the space used effects its speed.

#### 4) System & Workload Parameters

#### 5) Factors & Their Values

#### 6) Evaluation Techniques

#### 7) Workload Selection

#### 8) Experiment Design

#### 9) Data Analysis & Interpretation

#### 10) Results Presentation

## HW4
In HW4 we modified the Cache so that it could be accessed using RESTful API over a network. To support this functionality we used [C++ REST SDK](https://github.com/Microsoft/cpprestsdk) (make sure to install this and run it with the appropriate linkers!).
Currently, we do not support the client and server running on different machines. To run our code for HW4 you will need to test it in two separate terminals as follows:

### 'Server Terminal':
#### To compile:
```g++ -g -O3 -Wextra -pedantic -Wall --std=c++14 -o server server_Cache.cc server.cc -lboost_system -lcrypto -lssl -lcpprest```
#### To run:
```./server -m maxmem -t portnum```

### 'Client Terminal':
#### To compile:
```g++ -g -O3 -Wextra -pedantic -Wall --std=c++14 -o client client_Cache.cc client_test.cc -lboost_system -lcrypto -lssl -lcpprest```
#### To run:
```./client```

### Tests
We ran similar tests to the one in HW3, but updated to suit the requirements of our client Cache (especially the need to free the const char* pointer returned by Cache::get). One of the issues we faced is that we realised that the tests from HW3, which mostly made use of unsigned integer values, would not work because RESTful API and JSON only work with strings. As such, we had to make these changes to our original testing code.
Currently client_Cache shuts down the Cache in the Impl destructor, and we are not sure how to reboot the server in the constructor. As such, each test can only be run once (if not only one will pass). This is not ideal, but we hope this will be fixed soon!

### Acknowledgements
We would like to thank Jillian James & Marika Swanberg for allowing us to use their Cache code (from HW4 onwards).

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
Implemented in the Cache constructor, using unordered_map.

### Eviction Policy (HW2 Part 6)
Implemented FIFO in Cache::set.

### Test (HW3)
For HW3, we developed unit tests and ran them on our code as well as code from three other groups. Test results were uploaded to Moodle.