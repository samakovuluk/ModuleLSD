#pragma once

#include <array>
#include <string>
#include <boost/asio.hpp>
#include <prism/s6/ls2d_settings.h>
#include <prism/s6/sensor_info.h>


namespace prism {
namespace s6 {


struct data_point {
    double x;
    double y;
};

struct result_info {
    uint32_t raw_info_1;
    uint32_t raw_info_2;
    unsigned lines;
    bool cal_en;
};


class ls2d_device {
public:
    ls2d_device();
    bool connect(std::string ip_addr);
    void disconnect();
    bool is_connected();
    bool laser_enable(bool on);
    bool sensor_info_read(sensor_info& info);
    const sensor_info& info() const;
    bool settings_read();
    bool settings_read(ls2d_settings& s);
    bool settings_write(const ls2d_settings& s);
    const ls2d_settings& settings() const;
    bool latch_save();
    bool latch_read_res(result_info* info, data_point* points);
    bool latch_and_send_image();
    bool stream_enable(bool en);
    bool stream_read(result_info* info, data_point* points);
    bool image_read_line(uint8_t* buffer);

private:
    bool send(boost::asio::ip::udp::socket& udp_port, size_t length);
    bool send_wait_resp(boost::asio::ip::udp::socket& udp_port, size_t length, size_t resp_length);
    bool get_packet(boost::asio::ip::udp::socket& udp_port, size_t length, int first_byte = -1);
    void clear_input_buffer(boost::asio::ip::udp::socket& udp_port);
    bool wait_packet(boost::asio::ip::udp::socket& udp_port, size_t length, int first_byte = -1);
    bool res_parse(result_info* info, data_point* points);

private:
    boost::asio::io_service _io_service;
    boost::asio::ip::udp::endpoint _remote_endpoint;
    boost::asio::ip::udp::socket _udp_cmd;
    boost::asio::ip::udp::socket _udp_res;
    boost::asio::ip::udp::socket _udp_img;
    std::array<uint8_t, 64 * 1024> _buf;
    bool _connected = false;
    sensor_info _info;
    ls2d_settings _settings;
    int _default_responce_timeout_ms = 200;

};


}
}