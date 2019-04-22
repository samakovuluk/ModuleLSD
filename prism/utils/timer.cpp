#include "timer.h"

#include <thread>

using namespace std::chrono;

namespace prism {
namespace utils {


timer::timer(std::chrono::nanoseconds timeout) {
    restart(timeout);
}

void timer::restart(std::chrono::nanoseconds timeout) {
    _start_point = steady_clock::now();
    _end_point = _start_point + timeout;
}

bool timer::is_expired() {
    return steady_clock::now() > _end_point;
}

std::chrono::nanoseconds timer::elapsed() {
    return steady_clock::now() - _start_point;
}

void timer::sleep(std::chrono::nanoseconds timeout) {
    std::this_thread::sleep_for(timeout);
}


}
}