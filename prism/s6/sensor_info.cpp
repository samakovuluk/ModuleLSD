#include "sensor_info.h"
#include <algorithm>
#include <cmath>


namespace prism {
namespace s6 {


bool sensor_info::from_buffer(const uint8_t* buf) {
    uint8_t data[256];
    for (int i = 0; i < 32; i++) data[i] = buf[i];

    //request_addr = ip_addr_v4::from_byte_array(&data[0]);       // IP-адрес, с которого посылался запрос датчику
    //request_port = make_uint16_t(&data[4]);                     // UDP-порт, с которого посылался запрос датчику

    ip_addr = ""; // IP-адрес датчика
    for (auto i = 0; i < 4; i++) {
        if (i != 0) ip_addr += ".";
        ip_addr += std::to_string(data[6 + i]);
    }

    std::string version_str = "";
    for (int i = 0; i < 5; i++) version_str.push_back(data[i + 16]);
    version_str += ".";
    for (int i = 0; i < 3; i++) version_str.push_back(data[i + 16 + 5]);
    std::string type_str = version_str.substr(6, 3);

    firmware_ver = version_str;                                 // Версия прошивки датчика
    //type = StringToDevType(type_str);
    //type_descr = StringToDevDescription(type_str);

    //===== Тип матрицы =====
    //matrix_type = MatrixType::NOT_SUPPORTED;
    //const bool onsemi_matrix = (type == DeviceType::Sensor_2d || type == DeviceType::Saratov_scanner_2d || type == DeviceType::Saratov_camera || type == DeviceType::Saratov_camera_color);
    //if (firmware_ver >= FirmwareVersion("32a") && type == DeviceType::ELIS_1D) matrix_type = MatrixType::ELIS;
    //if (firmware_ver >= FirmwareVersion("30d") && onsemi_matrix){
    //    matrix_type = MatrixType::UNKNOWN;
    //    if (data[10] == 0x01) matrix_type = MatrixType::VITA;
    //    if (data[10] == 0x02) matrix_type = MatrixType::PYTHON;
    //}

    //===== Настройки, читаемые из флеш-памяти. =====
    constexpr int PARAMS_COUNT = 118;
    std::copy_n(&buf[32], 128, data);
    if (data[0] != 'S') return false;
    if (data[1] != '6') return false;
    if (data[2] != 'V') return false;
    if (data[3] != '!') return false;
    if (data[PARAMS_COUNT + 4] != 'S') return false;
    if (data[PARAMS_COUNT + 5] != '6') return false;
    if (data[PARAMS_COUNT + 6] != 'V') return false;
    if (data[PARAMS_COUNT + 7] != '!') return false;
    std::copy_n(&buf[36], 124, data);

    // description 0-99
    description = reinterpret_cast<char*>(data);
    // serial 100-104
    serial = "";
    for (int i = 100; i < 105; i++) serial += data[i];
    // fractional bits 105
    fractional_bits = data[105];
    // ----- range 106-117 -----
    auto range_conv = [&](uint16_t v) { return double(v) / pow(2.0, fractional_bits); };
    auto range_conv_sign = [&](int16_t v) { return double(v) / pow(2.0, fractional_bits); };
    range_xs = range_conv(data[106] << 8 | data[107]);
    range_xe = range_conv(data[108] << 8 | data[109]);
    range_ysb = range_conv_sign(data[110] << 8 | data[111]);
    range_yst = range_conv_sign(data[112] << 8 | data[113]);
    range_yeb = range_conv_sign(data[114] << 8 | data[115]);
    range_yet = range_conv_sign(data[116] << 8 | data[117]);

    return true;
}


}
}