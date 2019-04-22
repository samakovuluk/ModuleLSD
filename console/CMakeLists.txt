# CMakeList.txt : CMake project for console

cmake_minimum_required (VERSION 3.7)

add_executable (console "main.cpp")
target_link_libraries(console prism)
target_link_libraries(console ${Boost_LIBRARIES})

if(WIN32)
    target_link_libraries(console ws2_32)
endif()
