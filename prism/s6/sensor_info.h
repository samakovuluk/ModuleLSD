#pragma once

#include <string>

namespace prism {
namespace s6 {


struct sensor_info {
    // Информация, получаемая без использования флеш-памяти.
    //std::string src_addr;        // IP-адрес, с которого посылался запрос датчику
    //uint16_t src_port;          // UDP-порт, с которого посылался запрос датчику
    std::string ip_addr;             // IP-адрес датчика
    //DeviceType type;                // Тип датчика
    std::string firmware_ver;   // Версия прошивки датчика
    //MatrixType matrix_type;         // Тип матрицы, используемый в датчике.
    //std::wstring type_descr;        // Описание типа датчика

    //===== Информация из флеш-памяти =====
    //bool ident_valid = false;
    std::string description;        // 0-99
    std::string serial;             // 100-104
    uint8_t fractional_bits = 5;    // 105
    //----- range 106-117 -----
    double range_xs;
    double range_xe;
    double range_ysb;
    double range_yst;
    double range_yeb;
    double range_yet;

    bool from_buffer(const uint8_t* buf);
    //void ToBuffer(uint8_t* buf) const;
};


}
}
