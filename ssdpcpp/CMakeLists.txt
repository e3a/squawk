# Build ssdpcpp
cmake_minimum_required(VERSION 3.2.2)

project(ssdpcpp)

set(SOURCES
    src/ssdpserverimpl.cpp
    src/asio/ssdpserverconnection.cpp
    src/asio/ssdpclientconnection.cpp
)

include_directories(${SQUAWK_INCLUDES} src/asio includes)
add_library(ssdpcpp ${SOURCES})

if (build_tests)
   enable_testing()
   set(LIBS ${CMAKE_BINARY_DIR}/httpcpp/libhttpcpp.a ${LIBS})
   include_directories(${GTEST_INCLUDE_DIRS} ${SQUAWK_INCLUDES} src/asio includes)
   add_executable(testmain_ssdp
                  test/testmain.cpp
                  test/headerparsetest.cpp
                  test/timertest.cpp)
   target_link_libraries(testmain_ssdp ssdpcpp ${LIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)
   add_test(ssdp-tests testmain_ssdp)
endif()
