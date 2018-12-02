# CSCI389 Cache Project
### by Laura Yoshida & Lucas Yong

## Description
This project was undertaken for the "CSCI389 - Computer Systems" class at Reed College in the Fall 2018 semester, in which we write a look-aside Cache with key-value storage, and progressively add capabilities along the way.

## HW5
### Jain's Ten Steps
#### I. Goals & Definition of System Boundaries
The goal of this benchmark effort is to determine the impact of both the volume of Cache requests (stress) and the space used in the Cache (load) on the response time (in milliseconds). The Cache in question is the one we have developed as of HW4, where a client interacts with the Client Cache API (in ```client_cache.cc```) that makes calls to a Server (in ```server.cc```), which in turn stores keys and values in its Backing Store (in ```server_cache.cc```). With the goals and the basic outline of the system in mind, our system boundaries include all the aforementioned components that comprise our Cache, internal to the system we have created.

#### II. System Services & Possible Outcomes
##### Our Cache has the following system services (with outcomes described for each service):
1. ```Cache::get```
  * If the get query is successful, this service retrieves a value from the Cache based on the associated key (as input by the user), and also returns the size of that value. If it is unsuccessful, it will simply return a nullptr (with size 0).
2. ```Cache::set```
  * A successful set query will add a key-value pair to the Cache, overwriting the old value if the key already exists and evicting (using FIFO) an old value if the new addition causes the maximum memory to be exceeded. If it is unsuccessful, it will simply do nothing and return a nonzero value.
3. ```Cache::del```
  * A successful delete query removes a key-value pair, if it is in the cache. If it does not exist or some other error occurs, it will do nothing and return zero.
4. ```Cache::space_used```
  * This query returns the current amount of memory being used, which is the only possible outcome.

#### III. Performance Metrics
1. **Sustained Throughput**, defined (by Eitan) as the maximum offered load (in requests per second) at which the mean response time remains under 1 millisecond.
2. **Load Testing**: We want to test the impact of spaced used in the Cache on the speed of response times for GET. GET was chosen because it is a crucial method that does not increase or decrease the space used in itself and thus we can effectively and independently measure how the space used affects its speed.

#### IV. System & Workload Parameters
* System Parameters
  * System Processor:        2.6 GHz Intel Core i5
  * System Memory:           8 GB 1600 MHz DDR3
  * Virtual Machine (Linux): VirtualBox Ubuntu 64-bit
* Workload Parameters
  * Volume of User requests
  * Cache Space Used

#### V. Factors & Their Values
* Volume of User Requests
  * We varied this from 1 to 2^17 requests per second.
* Cache Space Used
  * We varied this from 1 to 2^17 items in the Cache.

These values were selected because we could see a natural progression by 'doubling' each next value.

#### VI. Evaluation Techniques
The evaluation technique we are using is **Simulation**, since we are not testing with "real" users.

#### VII. Workload Selection
We have the following workload choice for our Sustained Throughput metric:
* 70% GETs
* 29% DELs
* 1%  SETs

This was to mimic the ETC workload in the memcached paper.

#### VIII. Experiment Design
* Sustained Throughput
    * For this test, we varied the requests per second as described in Part V. For each load (requests per second), we inverted it to obtain the number of milliseconds to run per query. Then we would pick a query with probability based on the workload in VII, and run it. Any 'extra time' would be waited for, before repeating the experiment. Code can be seen in ```benchmark.cc```.
* Load Testing
    * For this test, we varied the number of items in the Cache as described in Part V. For each amount, we would perform 1000 GETs, and see how long it took per request. Code can be seen in ```benchmark.cc```.

For each test, data was extracted and plotted using gnuplot.

#### IX. Data Analysis & Interpretation
This part was quite suprising!
* For ** Sustained Throughput**, we noticed that contrary to what we thought, the mean response time was initially very large (much more than 1ms), but quickly dropped to values far lower than one for higher load. Initially it was expected that the reverse would be true, but looking at the data it would appear that some overhead is overcome on average by making many calls instead of few.
* For our **Load Test**, we were also surprised to see that average response time remained static at around 44ms no matter how many values were in the Cache. As such, it seems we can conclude that the number of items in the Cache does not affect average response time, but we are unsure if maybe there are other factors at work here.

#### X. Results Presentation
Results are presented in the following data and graph files:
* Sustained Throughput
    * ```st_data.csv```
    * ```sus_tp.gif```
* Load Testing
    * ```lt_data.csv```
    * ```load_test.gif```

## HW4
In HW4 we modified the Cache so that it could be accessed using RESTful API over a network. To support this functionality we used [C++ REST SDK](https://github.com/Microsoft/cpprestsdk) (make sure to install this and run it with the appropriate linkers!).
Currently, we do not support the client and server running on different machines. To run our code for HW4 you will need to test it in two separate terminals as follows:

### 'Server Terminal':
#### To compile:
```g++ -g -O3 -Wextra -pedantic -Wall --std=c++14 -o server server_cache.cc server.cc -lboost_system -lcrypto -lssl -lcpprest```
#### To run:
```./server -m maxmem -t portnum```

### 'Client Terminal':
#### To compile:
```g++ -g -O3 -Wextra -pedantic -Wall --std=c++14 -o client client_cache.cc client_test.cc -lboost_system -lcrypto -lssl -lcpprest```
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