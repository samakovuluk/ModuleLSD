# CMakeList.txt : CMake project for prism

cmake_minimum_required (VERSION 3.7)

add_library (prism
    utils/byte_manip.h
    utils/timer.h
    utils/timer.cpp
    s6/ls2d_device.h
    s6/ls2d_device.cpp
    s6/ls2d_settings.h
    s6/ls2d_settings.cpp
    s6/sensor_search.h
    s6/sensor_search.cpp
    s6/sensor_info.h
    s6/sensor_info.cpp
)

target_link_libraries(prism Threads::Threads)

