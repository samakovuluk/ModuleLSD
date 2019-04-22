#pragma once

#include <chrono>


namespace prism {
namespace utils {


class timer {
public:
    timer(std::chrono::nanoseconds timeout = std::chrono::nanoseconds(0));
    void restart(std::chrono::nanoseconds timeout = std::chrono::nanoseconds(0));
    bool is_expired();
    std::chrono::nanoseconds elapsed();
    static void sleep(std::chrono::nanoseconds timeout);
private:
    std::chrono::steady_clock::time_point _start_point;
    std::chrono::steady_clock::time_point _end_point;
};


}
}