#pragma once

#include <cstdint>


namespace prism {
namespace utils {


uint16_t inline make_uint16(uint8_t* buf) {
    return (buf[0] << 8) + buf[1];
}

uint16_t inline make_uint16(uint8_t b0, uint8_t b1) {
    return (b0 << 8) + b1;
}

int16_t inline make_int16(uint8_t* buf) {
    return (buf[0] << 8) + buf[1];
}

uint32_t inline make_uint32(uint8_t* buf) {
    return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

int32_t inline make_int32(uint8_t* buf) {
    return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}


uint32_t inline make_uint32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
    return (b0 << 24) + (b1 << 16) + (b2 << 8) + b3;
}


}
}