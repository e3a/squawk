# Build httpcpp
cmake_minimum_required(VERSION 2.8.9)

project(httpcpp)

find_package(GoogleTest REQUIRED)
find_package(Log4cxx REQUIRED)
set(LIBS ${LIBS} ${LOG4CXX_LIBRARIES})  
find_package(PCRE++ REQUIRED)
set(LIBS ${LIBS} -lpcrecpp)  

# add_definitions(-DASIO_ENABLE_HANDLER_TRACKING)

include_directories(
  ${SQUAWK_INCLUDES}
  ${LOG4CXX_INCLUDE_DIR}  ${PCREPP_INCLUDE_DIR}
  src/asio)

set(SOURCES  src/httpparser.cpp src/httpservlet.cpp src/webserver.cpp src/httpresponse.cpp)
set(SOURCES ${SOURCES} src/asio/asioconnection.cpp
                       src/asio/asioserver.cpp
                       src/asio/httpserverfactory.cpp)
set(SOURCES ${SOURCES} src/servlet/fileservlet.cpp)

add_library(libhttpcpp ${SOURCES})

# if (build_tests)
#    enable_testing()
#    include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} includes)
#    add_executable(testmain test/testmain.cpp
#                   test/mimetypetest.cpp
#                   test/httpparsertest.cpp
#                   test/httpservlettest.cpp
#        ${SOURCES})
#    target_link_libraries(testmain pthread)
#    target_link_libraries(testmain
#        ${LIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
#    add_test(squawk-tests testmain)
# endif()
