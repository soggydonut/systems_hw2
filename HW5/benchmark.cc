#include "cache.hh"
#include <chrono>
#include <random>
#include <iostream>
#include <vector>
#include <thread>
#include <math.h>
#include <algorithm>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

static const Cache::index_type MAXMEM = 100000000;
static const char ALPHANUMERIC[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const int CACHE_START_SIZE = 100000;

vector<string> keys;
Cache *benchmarkCache;

Cache::val_type getBlobFromString(string *str) {
    Cache::val_type blobFromString = static_cast<Cache::val_type>(str);
    return blobFromString;
}

string randomString(int length) {
    string rStr = "";
    unsigned seed = system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(0, sizeof(ALPHANUMERIC) - 1);
    for (int i = 0; i < length; ++i) {
        char nextLetter = ALPHANUMERIC[distribution(generator)];
        while (!isalnum(nextLetter)) {
            nextLetter = ALPHANUMERIC[distribution(generator)];
        }
        rStr += nextLetter;
    }
    return rStr;
}

// Populate the Cache with random keys and values, but store the keys in our key vector (so we can use them later).
void populateCache(int amount) {
    int i = 0;
    while (i < amount) {
        string key = randomString(5);
        // check key unique?
        string valStr = randomString(5);
        Cache::val_type value = getBlobFromString(&valStr);
        int success = benchmarkCache->set(key, value, valStr.size());
        if (success == 0) {
            keys.push_back(key);
        }
        ++i;
    }
}
//Determine the mean response time in milliseconds based on a particular load amount (in requests per second).
void sustainedThroughput(int load) {
    // Set up for random distribution from 1 to 100, so we can run queries based on ETC in the memcached paper as follows:
    // GET: 70%
    // DEL: 29%
    // SET:  1%
    unsigned seed = system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(1, 100);
    int i = 0;
    Cache::index_type val_size;
    double millisecondsPerRequest = (1/double(load)) * 1000;
    double totalTime = 0.0;
    const auto startTotal = steady_clock::now();
    while (i < load) {
        int operation = distribution(generator);
        const auto startSingle = steady_clock::now();
        auto keyIndex = i % keys.size();
        if (operation <= 70) {
            benchmarkCache->get(keys[keyIndex], val_size);
        } else if (operation > 70 && operation <= 99) {
            benchmarkCache->del(keys[keyIndex]);
            keys.erase(keys.begin()+keyIndex);
        } else {
            benchmarkCache->set(keys[keyIndex], &keys[keyIndex], keys[keyIndex].size());
        }
        const auto endSingle = steady_clock::now();
        duration<double, milli> timeForOneRequest = endSingle - startSingle;
        // Wait if there is still time left over
        double extraTime = millisecondsPerRequest - timeForOneRequest.count();
        if (extraTime > 0) {
            std::this_thread::sleep_for(duration<double, milli>(extraTime));
        }
        totalTime += timeForOneRequest.count();
        ++i;
    }
    const auto endTotal = steady_clock::now();
    duration<double, milli> totalMilliseconds = endTotal - startTotal;
    double meanResponseTime = totalMilliseconds.count() / (double(load) * totalTime);

    cout << "Request Per Second (Load): " << load << endl;
    cout << "       Mean Response Time: " << meanResponseTime << endl;
}

// Get the mean time for 1000 GET operations. Cache size (# of items) will be varied in main.
void loadTesting() {
    int numberOfGets = 1000;
    int i = 0;
    Cache::index_type val_size;
    random_shuffle(keys.begin(), keys.end());
    const auto start = steady_clock::now();
    while (i < numberOfGets) {
        benchmarkCache->get(keys[i % keys.size()], val_size);
        ++i;
    }
    const auto end = steady_clock::now();
    duration<double, std::milli> totalMilliseconds = end - start;
    cout << "Items in Cache: " << keys.size() << endl;
    cout << "Mean Response Time: " << (totalMilliseconds.count() / numberOfGets) << endl;
}

int main() {
    benchmarkCache = new Cache(MAXMEM);
    int alreadyPopulated = 0;
    for (int i = 0; i < 17; ++i) {
        populateCache(pow(2, i));
        alreadyPopulated += pow(2,i);
        loadTesting();
    }
    populateCache(CACHE_START_SIZE - alreadyPopulated);
    for (int i = 0; i < 17; ++i) {
        sustainedThroughput(pow(2, i));
    }
    delete benchmarkCache;
}