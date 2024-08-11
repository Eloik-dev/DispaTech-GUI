cmake_minimum_required(VERSION 3.22)

project(dispatech-gui)

include_directories(/usr/local/include)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0)

# Find all project files to compile
file(GLOB_RECURSE src_files "src/*.cpp" "header/*.h")

# Copy default configurations files to the build directory
file(COPY "default/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add Socket.IO C++ client library
find_package(sioclient REQUIRED)

add_executable(dispatech-gui "main.cpp" ${src_files})
target_link_libraries(dispatech-gui
    PkgConfig::GTK4
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-app
    PkgConfig::gstreamer-video
    nlohmann_json::nlohmann_json
    sioclient
    OpenSSL::SSL
    OpenSSL::Crypto
    curl
)
