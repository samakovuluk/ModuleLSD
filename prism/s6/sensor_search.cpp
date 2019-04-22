#include "sensor_search.h"
#include <prism/utils/timer.h>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std::chrono_literals;

namespace prism {
namespace s6 {


bool sensor_search::begin(std::string ip_addr) {
    _socket.open(udp::v4());
    _socket.set_option(boost::asio::socket_base::receive_buffer_size(30 * 1024));
    _socket.bind(udp::endpoint(udp::v4(), 0));
    _socket.set_option(udp::socket::reuse_address(true));
    _socket.set_option(socket_base::broadcast(true));
    _remote = udp::endpoint(address_v4::from_string(ip_addr), 0x2DA1);

    _sensors.clear();
    _sensors_size_cached = 0;
    _stop_thread = false;

    _thread = std::thread([this]() {
        constexpr uint8_t CMD = 0xF4;   // Команда поиска датчиков
        constexpr size_t R_SIZE = 257;  // Длина ответа
        uint8_t buf[8 * 1024];
        sensor_info info;
        utils::timer timer;

        while (!_stop_thread) {
            buf[0] = CMD;
            _socket.send_to(buffer(buf, 1), _remote);
            timer.restart();
            while (!_stop_thread && timer.elapsed() < 2s) { // Период отправки запросов
                auto avail = _socket.available();
                if (avail < R_SIZE) {
                    std::this_thread::sleep_for(100ms);
                    continue;
                }
                auto received = _socket.receive(boost::asio::buffer(buf, R_SIZE));
                if (received != R_SIZE || buf[0] != CMD) continue;
                if (!info.from_buffer(&buf[1])) continue;

                std::lock_guard<std::mutex> lock(_mutex);
                if (std::find_if(_sensors.begin(), _sensors.end(), [&](const sensor_info& v) { return info.ip_addr == v.ip_addr; }) != _sensors.end()) continue;
                _sensors.push_back(info);
                _sensors_size_cached = _sensors.size();
                std::sort(_sensors.begin(), _sensors.end(), [](const sensor_info& a, const sensor_info& b) { return a.ip_addr < b.ip_addr; });
            }
        }
    });
    return true;
}

void sensor_search::end() {
    _stop_thread = true;
    if (_thread.joinable()) _thread.join();
    _socket.close();
}

bool sensor_search::is_active() {
    return !_stop_thread;
}

std::vector<sensor_info> sensor_search::get_list() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _sensors;
}

size_t sensor_search::get_list_size() {
    return _sensors_size_cached;
}


}
}