# CMakeList.txt : Top-level CMake project file

cmake_minimum_required (VERSION 3.7)
message(STATUS "CMake version: " ${CMAKE_VERSION})

project ("ls2d_example")


#==========
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release)
endif()
if (NOT "${CMAKE_CXX_STANDARD}")
    set(CMAKE_CXX_STANDARD 14)
endif()

include_directories(${CMAKE_SOURCE_DIR})

#========== Compiler options ==========
message (STATUS "CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})

if ("MSVC" STREQUAL "${CMAKE_CXX_COMPILER_ID}")
    add_definitions(-D_WIN32_WINNT=0x0501)
    add_compile_options(/utf-8) #  specify both the source and execution character sets as encoded by using UTF-8.
elseif ("GNU" STREQUAL "${CMAKE_CXX_COMPILER_ID}")
    add_compile_options(-Wno-psabi) # disable warnings about ABI changes
endif()


#========== Output directory ==========
math(EXPR platform_bits "${CMAKE_SIZEOF_VOID_P} * 8")
set (platform_dir "x${platform_bits}-${CMAKE_BUILD_TYPE}")
set (output_directory "${CMAKE_SOURCE_DIR}/bin/${platform_dir}")
message(STATUS "platform_dir: " ${platform_dir})
message(STATUS "output_directory: " ${output_directory})
message (STATUS "CMAKE_PREFIX_PATH: " "${CMAKE_PREFIX_PATH}")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_directory})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_directory})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_directory})

#========== Boost ==========
add_definitions(-DBOOST_ASIO_STANDALONE)
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
add_definitions(-DBOOST_SYSTEM_NO_LIB)
add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED)

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.62.0)

if (NOT ${Boost_FOUND})
    message(FATAL_ERROR "Boost not found")
endif()

include_directories(${Boost_INCLUDE_DIR})

# message(STATUS "Boost_VERSION: " ${Boost_VERSION}) 
# message(STATUS "Boost_INCLUDE_DIR: " ${Boost_INCLUDE_DIR})
# message(STATUS "Boost_LIBRARIES: " ${Boost_LIBRARIES})

#========== Threads ==========
find_package(Threads REQUIRED)

#========== Projects ==========
add_subdirectory ("prism")
add_subdirectory ("console")


