# Build ssdpcpp
cmake_minimum_required(VERSION 2.8.9)

project(ssdpcpp)

include_directories(${SQUAWK_INCLUDES} ${LOG4CXX_INCLUDE_DIR} src/asio includes)

set(SOURCES src/ssdpserverimpl.cpp ${SOURCES} src/asio/ssdpasioconnection.cpp)

add_library(ssdpcpp ${SOURCES})
