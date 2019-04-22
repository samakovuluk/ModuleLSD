#pragma once

#include <thread>
#include <vector>
#include <mutex>

#include <boost/asio.hpp>

#include "sensor_info.h"

namespace prism {
namespace s6 {

class sensor_search {
public:
    bool begin(std::string ip_addr);
    void end();
    bool is_active();
    std::vector<sensor_info> get_list();
    size_t get_list_size();

private:
    bool _stop_thread = true;
    boost::asio::io_service _io_service;
    boost::asio::ip::udp::socket _socket = boost::asio::ip::udp::socket(_io_service);
    boost::asio::ip::udp::endpoint _remote;
    std::thread _thread;
    std::mutex _mutex;
    std::vector<sensor_info> _sensors;
    std::atomic_size_t _sensors_size_cached;
};

}
}