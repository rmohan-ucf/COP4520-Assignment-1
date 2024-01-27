
# Multithreaded Primality Tester

I began my approach with the square root primality testing method. In this method, 
if a number is prime, then it must have a factor less than or equal to its square 
root. This reduces number of integers we must check before confirming a number is 
prime compared to the tradition 2 through n-1 approach. This method can also be 
independently repeated for any number and does not require any prior data.

I also chose this method because of how easily it could be partitioned into
separate chunks. For each thread, the only shared data it needs to access/modify
is the prime count (keeps track of the total number of prime integers), the prime
sum (keeps track of the sum of all prime integers), and the largest primes min-heap
priority queue (keeps track of the all the largest primes). And because the square
root method is a proven way to determine prime numbers and the multithreaded approach
doesn't modify this original method, it must be accurate. And as long as the aggregates
are handled correctly, our final results should also be accurate.

I partitioned our range of numbers into 8 equal chunks and created threads for each
chunk. Each thread will loop over all the integers in its chunk and determine if
they are prime. Each thread will also include a local count, local sum, and local largest
primes min-heap priority queue. If a number is prime, it will be added to the count, sum,
and pq, respectively. If the pq is greater than 10 items, the top number is popped,
and since it is a min-heap pq, the smallest number will be removed, retaining the 
top 10.

After the entire chunk is processed, with atomic operations, the local count and
sum are added to the global count and sum, respectively. And with a mutex lock, 
all of the items in the local pq are emptied out into the global pq. And same as
before, removing the top item as long as the pq is greater than 10 items.

After all the threads have been completed, the final results are outputted. This
approach should be considerable quicker than if only one thread was used, thus
properly leveraging parallelism for efficiency and speed. I confirmed this by
changing "numThreads" in line 10 from 8 threads to 1 thread and finding a noticeable
performance drop between the two runs.

## Execution Instructions

To compile:
```
g++ -std=gnu++20 -O2 primes.cpp
```

To execute:
```
./a.out
```
