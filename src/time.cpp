#include "time.h"
#include <chrono>

uint64_t get_time_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
} //got this off chatgpt ngl, returns time relative to some value, so time btwn call 1 and 2 is just 2-1
