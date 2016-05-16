# Build ssdpcpp
cmake_minimum_required(VERSION 2.8.9)

project(ssdpcpp)

find_package(CURLpp REQUIRED)
set(LIBS ${LIBS} ${CURLPP_LIBRARIES})

include_directories(${SQUAWK_INCLUDES} ${LOG4CXX_INCLUDE_DIR} ${CURLPP_INCLUDE_DIRS} src/asio includes)

set(SOURCES ${SOURCES} src/ssdpserverimpl.cpp src/asio/ssdpserverconnection.cpp src/asio/ssdpclientconnection.cpp)

add_library(ssdpcpp ${SOURCES})

if (build_tests)
   enable_testing()
   set(LIBS ${CMAKE_BINARY_DIR}/httpcpp/libhttpcpp.a ${LIBS})
   include_directories(${GTEST_INCLUDE_DIRS} ${SQUAWK_INCLUDES} ${LOG4CXX_INCLUDE_DIR} src/asio includes)
   add_executable(testmain_ssdp test/testmain.cpp
                  test/headerparsetest.cpp
                  test/timertest.cpp)
   target_link_libraries(testmain_ssdp ssdpcpp ${LIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)
   add_test(ssdp-tests testmain_ssdp)
   add_custom_target(unittest ssdp-tests)
endif()
