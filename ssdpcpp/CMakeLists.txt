# Build ssdpcpp
cmake_minimum_required(VERSION 3.2.2)

project(ssdpcpp)

set(SOURCES
    src/ssdp.h
    src/ssdpserverimpl.cpp
    src/asio/ssdpserverconnection.cpp
    src/asio/ssdpclientconnection.cpp
)

include_directories(${SQUAWK_INCLUDES} src/asio)
add_library(ssdpcpp ${SOURCES})

if (build_tests)
   enable_testing()
   include_directories(${SQUAWK_INCLUDES} ${GTEST_INCLUDE_DIRS} src/asio includes)
   add_executable(testmain_ssdp
                  test/testmain.cpp
                  test/headerparsetest.cpp
                  test/timertest.cpp)
              target_link_libraries(testmain_ssdp ssdpcpp httpcpp pthread ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${LIBS} )
   add_test(ssdp-tests testmain_ssdp)
endif()
