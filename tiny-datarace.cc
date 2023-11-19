#include <cassert>
#include <iostream>
#include <thread>

namespace {
    const int kThreadCount = 100;
    const int kOpCount = 10000;

    int global = 0;
}

void atomic_write(int *value) {
    for (int i = 0; i < kOpCount; i++) {
        __atomic_store_n(value, i, __ATOMIC_SEQ_CST);
    }
}

#ifdef ATOMIC_ONLY

void read_value(int *value) {
    for (int i = 0; i < kOpCount; i++) {
        int v = __atomic_load_n(value, __ATOMIC_SEQ_CST);
        assert(v >= 0 && v <= kOpCount);
    }
}

#else

void read_value(int *value) {
    for (int i = 0; i < kOpCount; i++) {
        int v = *value;
        assert(v >= 0 && v <= kOpCount);
    }
}

#endif

int main(int argc [[maybe_unused]], char **argv [[maybe_unused]])
{
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
    return 0;
}
