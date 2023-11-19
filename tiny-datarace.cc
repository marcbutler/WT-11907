#include <cassert>
#include <iostream>
#include <thread>

namespace {
    const int kThreadCount = 100;
    const int kOpCount = 10000;

    volatile int global = 0;
    volatile int global2 = 0;
}

void atomic_write(volatile int *value) {
    for (int i = 0; i < kOpCount; i++) {
        __atomic_store_n(value, i, __ATOMIC_SEQ_CST);
    }
}

#ifdef ATOMIC_ONLY

void read_value(volatile int *value) {
    for (int i = 0; i < kOpCount; i++) {
        int v = __atomic_load_n(value, __ATOMIC_SEQ_CST);
        assert(v >= 0 && v <= kOpCount);
    }
}

#else

void read_value(volatile int *value) {
    for (int i = 0; i < kOpCount; i++) {
        int v = *value;
        assert(v >= 0 && v <= kOpCount);
    }
}

#endif


void test_multiple_threads()
{
    // Mutliple threads writing and reading the same memory location.

    std::thread thrs[kThreadCount];

    for (auto i = 0; i < kThreadCount; i++) {
        if (i & 1)
            thrs[i] = std::thread(atomic_write, &global);
        else
            thrs[i] = std::thread(read_value, &global);
    }

    for (auto i = 0; i < kThreadCount; i++) {
        thrs[i].join();
    }

}


void singular_access(volatile int *value) {
    int v;

    __atomic_store_n(value, -1, __ATOMIC_SEQ_CST);
#if ATOMIC_ONLY
    v = __atomic_load_n(value, __ATOMIC_SEQ_CST);
    assert(v == -1);
#else
    v = global;
    assert(v == -1);
#endif
    __atomic_store_n(value, 0, __ATOMIC_SEQ_CST);
}


void test_single_thread_only()
{
    // If only a single thread issues a write then read against a single location is a data race
    // still detected: it shouldn't be as there is no possibility of simultaneous access.

    std::thread thrs[kThreadCount];

    for (int i = 0; i < kThreadCount; i++) {
        if (i != kThreadCount / 2)
            thrs[i] = std::thread(read_value, &global);
        else
            thrs[i] = std::thread(read_value, &global2);
    }

    for (auto&& t : thrs) {
        t.join();
    }
}


int main(int argc [[maybe_unused]], char **argv [[maybe_unused]])
{
    std::cout << "\nACCESS FROM MULTIPLE THREADS\n" << std::endl;
    test_multiple_threads();

    std::cout << "\nACCESS FROM A SINGLE THREAD ONLY\n" << std::endl;
    test_single_thread_only();

    return 0;
}
