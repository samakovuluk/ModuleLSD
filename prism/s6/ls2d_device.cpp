#include "ls2d_device.h"
#include <cmath>
#include <prism/utils/timer.h>
#include <prism/utils/byte_manip.h>

using namespace boost::asio::ip;
using namespace std::chrono_literals;


namespace prism {
namespace s6 {


ls2d_device::ls2d_device() :
    _udp_cmd(_io_service),
    _udp_res(_io_service),
    _udp_img(_io_service)
{
}

bool ls2d_device::connect(std::string ip_addr) {
    disconnect();
    _remote_endpoint = udp::endpoint(address_v4::from_string(ip_addr), 0x2DA1);

    _udp_cmd.open(udp::v4());
    _udp_cmd.set_option(boost::asio::socket_base::receive_buffer_size(50 * 1024));
    _udp_cmd.bind(udp::endpoint(udp::v4(), 0));

    _udp_res.open(udp::v4());
    _udp_res.set_option(boost::asio::socket_base::receive_buffer_size(1* 1024 * 1024));
    _udp_res.bind(udp::endpoint(udp::v4(), 0));

    _udp_img.open(udp::v4());
    _udp_img.set_option(boost::asio::socket_base::receive_buffer_size(5 * 1024 * 1024));
    _udp_img.bind(udp::endpoint(udp::v4(), 0));

    if (!sensor_info_read(_info)) return false;
    if (!settings_read(_settings)) return false;

    return _connected = true;
}

void ls2d_device::disconnect() {
    _connected = false;
    _udp_cmd.close();
    _udp_res.close();
    _udp_img.close();
}

bool ls2d_device::is_connected() {
    return _connected;
}

bool ls2d_device::laser_enable(bool on) {
    _buf[0] = 0x81;
    _buf[1] = 0x20;
    _buf[2] = 0x01;
    _buf[3] = on ? 0x01 : 0x00;
    return send_wait_resp(_udp_cmd, 4, 1);
}

bool ls2d_device::sensor_info_read(sensor_info& info) {
    _buf[0] = 0xF4;
    send_wait_resp(_udp_cmd, 1, 257);
    return info.from_buffer(&_buf[1]);
}

const sensor_info& ls2d_device::info() const {
    return _info;
}

bool ls2d_device::settings_read() {
    return settings_read(_settings);
}

bool ls2d_device::settings_read(ls2d_settings& s) {
    _buf[0] = 0x80;
    _buf[1] = 0x00; // offset
    _buf[2] = 0xFF; // size
    if (!send_wait_resp(_udp_cmd, 3, 256)) return false;
    if (!s.from_buffer(&_buf[1])) return false;
    _settings = s;
    return true;
}

bool ls2d_device::settings_write(const ls2d_settings& s) {
    _buf[0] = 0x81;
    _buf[1] = 0x00; // offset
    _buf[2] = 0xFF; // size
    if (!s.to_buffer(&_buf[3])) return false;
    if (!send_wait_resp(_udp_cmd, 258, 1)) return false;
    _settings = s;
    return true;
}

const ls2d_settings& ls2d_device::settings() const {
    return _settings;
}

bool ls2d_device::latch_save() {
    _buf[0] = 0x20;
    return send(_udp_res, 1);
}

bool ls2d_device::latch_read_res(result_info * info, data_point * points) {
    _buf[0] = 0x22;
    send(_udp_res, 1);
    if (!wait_packet(_udp_res, 4104)) return false;
    return res_parse(info, points);
}

bool ls2d_device::stream_enable(bool en) {
    _buf[0] = 0x23;
    _buf[1] = en;
    return send_wait_resp(_udp_res, 2, 1);
}

bool ls2d_device::stream_read(result_info* info, data_point* points) {
    if (!get_packet(_udp_res, 4104)) return false;
    return res_parse(info, points);
}

bool ls2d_device::send(udp::socket& udp_port, size_t length) {
    boost::system::error_code ec;
    udp_port.send_to(boost::asio::buffer(_buf, length), _remote_endpoint, 0, ec);
    return !ec;
}

bool ls2d_device::send_wait_resp(udp::socket& udp_port, size_t length, size_t resp_length) {
    utils::timer timer;
    send(udp_port, length);
    uint8_t cmd = _buf[0];
    return wait_packet(udp_port, resp_length, cmd);
}

bool ls2d_device::get_packet(udp::socket& udp_port, size_t length, int first_byte) {
    boost::system::error_code ec;
    while (udp_port.available() >= length) {
        auto received_size = udp_port.receive(boost::asio::buffer(_buf, length), 0, ec);
        if (!ec && received_size == length && (first_byte < 0 || first_byte == _buf[0])) return true;
    }
    return false;
}

bool ls2d_device::wait_packet(udp::socket& udp_port, size_t length, int first_byte) {
    utils::timer timer;
    while (timer.elapsed() < std::chrono::milliseconds(_default_responce_timeout_ms)) {
        utils::timer::sleep(1ms);
        if (get_packet(udp_port, length, first_byte)) return true;
    }
    return false;
}

void ls2d_device::clear_input_buffer(boost::asio::ip::udp::socket &udp_port) {
    boost::system::error_code ec;
    while (udp_port.available() > 0) udp_port.receive(boost::asio::buffer(_buf, 1), 0, ec);
}

bool ls2d_device::res_parse(result_info * info, data_point * points) {
    using namespace utils;
    const uint16_t lines = _settings.sub_ye - _settings.sub_ys + 1;
    const bool cal_en = (_settings.cal_bypass == 0);
    const size_t D_OFFSET = 8;
    const double denom = cal_en ? pow(2.0, _info.fractional_bits) : 32.0;
    //-----
    info->raw_info_1 = make_uint32(&_buf[0]);
    info->raw_info_2 = make_uint32(&_buf[4]);
    info->lines = lines;
    info->cal_en = cal_en;
    //-----
    for (unsigned i = 0; i < lines; i++) {
        uint16_t x16 = make_uint16(&_buf[D_OFFSET + i * 4 + 0]);
        bool out_of_range = (x16 >= 0xFF00) || (!cal_en && x16 < 64);
        double x = out_of_range ? -1 : x16 / denom;
        int16_t y16 = make_uint16(&_buf[D_OFFSET + i * 4 + 2]);
        double y = cal_en ? y16 / denom : i;
        points[i].x = x;
        points[i].y = y;
    }
    return true;
}

bool ls2d_device::latch_and_send_image() {
    clear_input_buffer(_udp_img);
    _buf[0] = 0x21;
    return send(_udp_img, 1);
}

bool ls2d_device::image_read_line(uint8_t *buffer) {
    const size_t length = 1280;
    boost::system::error_code ec;
    while (_udp_img.available() >= length) {
        auto received_size = _udp_img.receive(boost::asio::buffer(buffer, length), 0, ec);
        if (!ec && received_size == length) return true;
    }
    return false;
}


}
}