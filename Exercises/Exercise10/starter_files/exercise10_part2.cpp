// Copyright (C) 2020 raodm@miamioh.edu

#include <iostream>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <deque>
#include <string>
#include <condition_variable>

// Prototypes to keep compiler happy. Normally, these should NOT be
// used. But they are used to sturcture the lab into portions that
// students should not modify.
void processStr(std::string value);
std::string createStr();

/**
   Maximum number of elements that can be held by the shared queue at
   any given instant of time.
*/
const size_t MaxQSize = 5;

// A namespace to wrap shared information so that we don't pollute the
// global namespace.
namespace prod_con {
    /** As you may recollect vividly from CSE-274, deque (usually
        pronounced like "deck") is an irregular acronym of double-ended
        queue. Double-ended queues are a kind of sequence container. As
        such, their elements are ordered following a strict linear
        sequence.
     */
    std::deque<std::string> queue;

    /**
       A mutex to arbitrate access to the queue by the producers and
       consumers.
     */
    std::mutex queueMutex;

}  // namespace prod_con

/**
   The consumer method that processes StrCount strings from the queue
   and prints the result to standard output.

   \param[in] StrCount The number of strings to be consumed by this method.
*/
void consumer(const int StrCount) {
    int i = 0;
    while (i < StrCount) {
        std::string value;
        bool haveStr = false;

        // The logic below locks/unlocks until it gets a string to
        // process. Hence it is a "busy wait" or "spin lock" approach.
        {  // Begin critical Section
            std::lock_guard<std::mutex> lock(prod_con::queueMutex);
            if (!prod_con::queue.empty()) {
                value = prod_con::queue.front();
                prod_con::queue.pop_front();  // Remove from queue
                haveStr = true;
            }
        }  // End Critical Section

        // Process string value if one was obtained.
        // Process outside critical section to keep it short
        if (haveStr) {
            processStr(value);
            i++;  // track number of strings processed.
        }
    }
}

/**
   The consumer method that generates StrCount strings and adds it to
   the shared queue (for the cosumer to process)

   \param[in] StrCount The number of strings to be consumed by this method.
*/
void producer(const int StrCount) {
    for (int i = 0; (i < StrCount); i++) {
        // Create value outside the critical secion.
        std::string value = createStr();
        bool stringAdded  = false;
        // Try to add the string to the queue. If the queue is full,
        // try again. The loop below spins until the value is
        // added. Hence it is called a "spin lock" or a "busy wait"
        // approach.
        while (!stringAdded) {
            // Begin critical Section
            std::lock_guard<std::mutex> lock(prod_con::queueMutex);
            // Add new value only if there is space in the queue
            if (prod_con::queue.size() < MaxQSize) {
                prod_con::queue.push_back(value);
                stringAdded = true;
            }
        }  // End Critical Section
    }
}

//--------------------------------------------------------------------
//  DO  NOT  MODIFY  CODE  BELOW  THIS  LINE
//  DO  NOT  MODIFY  CODE  BELOW  THIS  LINE
//--------------------------------------------------------------------

/**
 * Just do some basic processing to put some load on the CPU.
 *
 * \param[in] value The string to be rotated
 *
 * \param[in] seed The random seed value to be used.
 *
 * \param[in] range The substring that should be modified.
 */
std::string rotate(std::string value, unsigned int *seed, int range) {
    const int times = rand_r(seed) % range;
    for (int i = 0; (i < times); i++) {
        std::rotate(value.begin(), value.begin() + 1, value.end());
    }
    return value;
}

/**
 * The string to be processed. This one just some basic operations.
 */
void processStr(std::string value) {
    static unsigned int seed = 9059;
    // Run the roulette and print value to check luck
    std::cout << rotate(value, &seed, 10000) << std::endl;
}

/**
 * The string to be processed. This one just some basic operations.
 */
std::string createStr() {
    static unsigned int seed = 8419;
    // Generate using a roulette.
    return rotate("abcdefghijklmnopqrstuvwxyz", &seed, 100);
}

/**
 * The main method just spins-up 2 threads, 1 for producer and 1 for
 * consumer.
 */
int main() {
    std::thread prodThread(producer, 5000);
    std::thread consThread(consumer, 5000);
    prodThread.join();
    consThread.join();
    return 0;
}
