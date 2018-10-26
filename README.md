CSCI389 HW2: Hash It Out - Laura Yoshida & Lucas Yong

Performance (Part 3)
The hash function is implemented as follows:
- Split the string key into an array of characters.
- Iterate over the array and take the product of the prime numbers that correspond to each ASCII char.
   (e.g. 'C' is 67th on the ASCII table, and the 67th prime is 317).
- Once we have the product, return it as the bucket number to store value associated with the key.
Since any given integer is a product of primes, the only K-V pairs sharing the same bucket would be those whose keys are permutations of each other.

Collision Resolution (Part 4)
Taken care of by unordered_map.

Dynamic Resizing (Part 5)
Implemented in the cache constructor, using unordered_map.

Eviction Policy (Part 6)
Implemented FIFO in cache::set.
