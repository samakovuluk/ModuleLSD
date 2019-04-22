#include "ls2d_settings.h"
#include <cassert>
#include <prism/utils/byte_manip.h>

using namespace std;
using namespace prism::utils;


namespace prism {
namespace s6 {


bool ls2d_settings::to_buffer(uint8_t buf[255]) const {
    for (size_t i = 0; i < 255; i++) buf[i] = 0x00;
    for (size_t i = 0; i < 6; i++) buf[0x00 + i] = eth_mac[i];
    buf[0x06] = uint8_t(eth_ip >> 24);
    buf[0x07] = uint8_t(eth_ip >> 16);
    buf[0x08] = uint8_t(eth_ip >> 8);
    buf[0x09] = uint8_t(eth_ip >> 0);

    buf[0x20] = laser;
    buf[0x22] = sync_mode;
    buf[0x25] = uint8_t(period >> 16);
    buf[0x26] = uint8_t(period >> 8);
    buf[0x27] = uint8_t(period >> 0);
    buf[0x28] = uint8_t(expo_1 >> 8);
    buf[0x29] = uint8_t(expo_1 >> 0);
    buf[0x2C] = reset_level_1;

    buf[0x30] = sens_gain;
    buf[0x33] = sens_mode;
    buf[0x34] = sub_xs;
    buf[0x35] = sub_xe;
    buf[0x36] = uint8_t(sub_ys >> 8);
    buf[0x37] = uint8_t(sub_ys >> 0);
    buf[0x38] = uint8_t(sub_ye >> 8);
    buf[0x39] = uint8_t(sub_ye >> 0);

    buf[0x40] = conv_bypass;
    buf[0x41] = img_subtr;
    buf[0x42] = sens_restart;

    buf[0x50] = cal_bypass;
    buf[0x51] = subpix_side_en;
    buf[0x52] = subpix_side_thres;
    buf[0x53] = find_max_mult;
    buf[0x54] = locmax_dist[0];
    buf[0x55] = locmax_dist[1];
    buf[0x56] = locmax_dist[2];
    buf[0x57] = locmax_thres[0];
    buf[0x58] = locmax_thres[1];
    buf[0x59] = locmax_thres[2];

    buf[0x60] = median_window;
    buf[0x64] = detach_window;
    buf[0x65] = detach_points;
    buf[0x66] = uint8_t(detach_distance >> 8);
    buf[0x67] = uint8_t(detach_distance >> 0);

    for (size_t i = 0; i < 8; i++) {
        buf[0x70 + i * 2 + 0] = uint8_t(xmod[i] >> 8);
        buf[0x70 + i * 2 + 1] = uint8_t(xmod[i] >> 0);
    }

    for (int i = 0; i < 16; i++) {
        buf[0xDE - i * 2 + 0] = uint8_t(conv_coef[i] >> 8);
        buf[0xDE - i * 2 + 1] = uint8_t(conv_coef[i] >> 0);
    }

    return true;
}

bool ls2d_settings::from_buffer(uint8_t buf[255]) {
    for (size_t i = 0; i < 6; i++) eth_mac[i] = buf[i];
    eth_ip = make_uint32(&buf[6]);

    laser = buf[0x20];
    sync_mode = buf[0x22];
    period = (buf[0x25] << 16) + (buf[0x26] << 8) + (buf[0x27] << 0);
    expo_1 = make_uint16(&buf[0x28]);
    expo_2 = make_uint16(&buf[0x2A]);
    reset_level_1 = uint8_t(buf[0x2C] & 0x0F);
    sens_gain = buf[0x30];
    sens_mode = buf[0x33];

    sub_xs = buf[0x34];
    sub_xe = buf[0x35];
    sub_ys = make_uint16(&buf[0x36]);
    sub_ye = make_uint16(&buf[0x38]);

    conv_bypass = buf[0x40];
    img_subtr = buf[0x41];
    sens_restart = buf[0x42];

    cal_bypass = buf[0x50];
    subpix_side_en = buf[0x51];
    subpix_side_thres = buf[0x52];
    find_max_mult = buf[0x53];
    locmax_dist[0] = buf[0x54];
    locmax_dist[1] = buf[0x55];
    locmax_dist[2] = buf[0x56];
    locmax_thres[0] = buf[0x57];
    locmax_thres[1] = buf[0x58];
    locmax_thres[2] = buf[0x59];

    median_window = buf[0x60];

    detach_window = buf[0x64];
    detach_points = buf[0x65];
    detach_distance = make_uint16(&buf[66]);

    for (size_t i = 0; i < 8; i++) xmod[i] = make_uint16(&buf[0x70 + i * 2]);
    for (int i = 0; i < 16; i++) conv_coef[i] = make_uint16(&buf[0xDE - i * 2]);

    return true;
}


}
}