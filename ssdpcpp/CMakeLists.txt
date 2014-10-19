# Build ssdpcpp
cmake_minimum_required(VERSION 2.8.9)

project(ssdpcpp)

find_package(GoogleTest REQUIRED)
find_package(Log4cxx REQUIRED)
set(LIBS ${LIBS} ${LOG4CXX_LIBRARIES})  

include_directories(
  ${SQUAWK_INCLUDES}
  ${LOG4CXX_INCLUDE_DIR}
  src/asio
  includes)

set(SOURCES src/ssdpserverimpl.cpp ${SOURCES} src/asio/ssdpasioconnection.cpp)

add_library(libssdpcpp ${SOURCES})

#if (build_tests)
#    enable_testing()
#    include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} includes)
#    add_executable(testmain_ssdpcpp test/testmain.cpp
 #                  test/mimetypetest.cpp
 #                  test/httpparsertest.cpp
 #                  test/httpservlettest.cpp
#        ${SOURCES})
#    target_link_libraries(testmain_ssdpcpp pthread)
#    target_link_libraries(testmain_ssdpcpp
#        ${LIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
#    add_test(squawk-tests testmain_ssdpcpp)
#endif()
