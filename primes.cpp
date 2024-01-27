#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <fstream>

using namespace std;

const int upperBound = 100000001;  // 100 million
const int numThreads = 8;

// Min-heap PQ
priority_queue<int, vector<int>, greater<int>> globalPQ;
mutex largestPrimesMutex;

// Atomic number variables have only atomic operations
atomic<int> primeCount(0);
atomic<long> primeSum(0);

/**
 * Gets number of milliseconds since epoch time
 */
long long getTime() {
  auto tmp = chrono::system_clock::now().time_since_epoch();
  return chrono::duration_cast<chrono::milliseconds>(tmp).count();
}

/**
 * Returns whether a integer is prime or not
 */
bool isPrime(int num) {
  if (num < 2) return false;
  for (int i = 2; i * i <= num; i++) {
    if (num % i == 0) {
      return false;
    }
  }
  return true;
}

/**
 * Finds all needed information of primes in the given range and adds it to the
 * global aggregators.
 */
void countPrimesInRange(int start, int end) {
  priority_queue<int, vector<int>, greater<int>> localPQ;
  int localCount = 0;
  long localSum = 0;

  for (int i = start; i < end; i++) {
    if (isPrime(i)) {
      localCount++;
      localSum += i;

      localPQ.push(i);

      // Keep only the largest 10
      if (localPQ.size() > 10) {
        localPQ.pop();
      }
    }
  }

  // Atomic operations
  primeCount += localCount;
  primeSum += localSum;

  // Mutex locked
  lock_guard<mutex> lock(largestPrimesMutex);
  while (!localPQ.empty()) {
    globalPQ.push(localPQ.top());
    localPQ.pop();

    // Keep only the largest 10
    if (globalPQ.size() > 10) {
      globalPQ.pop();
    }
  }

  // Mutex automatically released when lock is out of scope.
}

int main() {
  long long startTime = getTime();

  vector<thread> threads;

  int rangeSize = upperBound / numThreads;
  for (int i = 0; i < numThreads; i++) {
    int start = i * rangeSize;
    int end = (i == numThreads - 1) ? upperBound : start + rangeSize;
    threads.emplace_back(countPrimesInRange, start, end);
  }

  for (auto& thread : threads) {
    thread.join();
  }

  long double executionTime = (getTime() - startTime) / 1000.0;

  ofstream outputFile;
  outputFile.open("primes.txt");
  outputFile << executionTime << "s " << primeCount << " " << primeSum << endl;

  for (int i = 0; i < 10 && !globalPQ.empty(); i++) {
    outputFile << globalPQ.top() << endl;
    globalPQ.pop();
  }

  outputFile.close();
  
  return 0;
}
