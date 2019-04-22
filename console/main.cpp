#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <prism/s6/ls2d_device.h>
#include <prism/s6/sensor_search.h>
#include <prism/utils/timer.h>

using namespace std;
using namespace prism::s6;

sensor_search sens_search;
ls2d_device sens;

void press_any_key() {
    //if (!system("pause")) return;
    cout << "Press enter to continue...\n";
    getchar();
}

void search_sensors(std::string addr) {
    sens_search.begin(addr);
    this_thread::sleep_for(4s);
    sens_search.end();
    auto list = sens_search.get_list();
    for (auto& e : list) {
        cout << e.ip_addr << " - " << e.serial << " - " << e.description << "\n";
    }
}

void laser_on_off() {
    static bool en;
    en = !en;
    sens.laser_enable(en);
    cout << "Laser state: " << (en ? "on" : "off") << endl;
}

void latch_read() {
    result_info info;
    array<data_point, 1024> pts;
    sens.latch_save();
    this_thread::sleep_for(50ms);
    if (!sens.latch_read_res(&info, pts.data())) return;

    ofstream fs("out_one.txt", ios_base::trunc);
    fs << fixed << left << setprecision(3);
    for (unsigned i = 0; i < info.lines; i++) {
        fs << setw(9) << pts[i].x << pts[i].y << "\n";
    }
}

void stream_read() {
    result_info info;
    array<data_point, 1024> points;
    vector<pair<result_info, array<data_point, 1024>>> data;

    sens.stream_enable(true);
    for (unsigned i = 0; i < 10; i++) {
        this_thread::sleep_for(100ms);
        while (sens.stream_read(&info, points.data()))
            data.push_back(make_pair(info, points));
    }
    sens.stream_enable(false);

    cout << "saving to file...\n";
    ofstream fs("out_stream.txt", ios_base::trunc);
    fs << fixed << left << setprecision(3);
    for (unsigned k = 0; k< data.size(); k++) {
        for (unsigned i = 0; i < data[k].first.lines; i++) {
            fs << setw(6) << k << setw(9) << data[k].second[i].x << data[k].second[i].y << "\n";
        }
        fs << "\n";
    }
}


int main(){
    const std::string sensor_search_addr = "192.168.1.255";
    const std::string sensor_ip_addr = "192.168.1.146";
    std::string cmd;

	cout << "LS2D sensor example\n";
    cout << "Connecting to IP " + sensor_ip_addr << "   ...   ";
    

    if (!sens.connect(sensor_ip_addr)) {
        cout << "FAILED" << endl;
        press_any_key();
        return 0;
    }
    else {
        cout << "OK\n";
    }

    auto info = sens.info();
    cout << "ip addr   : " + sens.info().ip_addr << endl;
    cout << "descr     : " + sens.info().description << endl;
    cout << "s/n       : " + sens.info().serial << endl;
    cout << "fw_ver    : " + sens.info().firmware_ver << endl;
    cout << "frac bits : " + to_string(sens.info().fractional_bits) << endl;


    std::string in;
    do {
        cout << "\n------------------------------------------------\n";
        cout << "1 - search sensors\n";
        cout << "2 - laser on/off\n";
        cout << "3 - write one measurement to file\n";
        cout << "4 - write stream to file\n";
        cout << "";
        cout << "Enter - exit.\n";
        cout << "\n> ";
        std::getline(std::cin, in);
        for (auto& c : in) c = toupper(c);
        cout << in << '\n';
        if (in == "1") search_sensors(sensor_search_addr);
        if (in == "2") laser_on_off();
        if (in == "3") latch_read();
        if (in == "4") stream_read();
    }
    while (in != "");


    //press_any_key();
    return 0;
}
