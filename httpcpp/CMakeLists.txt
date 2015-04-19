# Build httpcpp
cmake_minimum_required(VERSION 2.8.9)

project(httpcpp)

message( ${PROJECT_SOURCE_DIR}/src/asio )
include_directories(${SQUAWK_INCLUDES} ${LOG4CXX_INCLUDE_DIR} ${PCREPP_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src/asio)

set(SOURCES src/httpparser.cpp src/httpservlet.cpp src/webserver.cpp src/httpresponse.cpp
            src/httprequest.cpp
            src/asio/asioconnection.cpp src/asio/asioserver.cpp src/asio/httpserverfactory.cpp
            src/servlet/fileservlet.cpp
            src/httpclient.cpp src/asio/httpclientconnection.cpp)

add_library(httpcpp ${SOURCES})

add_executable(http src/httpmain.cpp ${SOURCES})
target_link_libraries(http ${LIBS})

if (build_tests)
   enable_testing()
   include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} includes)
   add_executable(testmain_httpcpp test/testmain.cpp
                  test/mimetypetest.cpp
                  test/httpparsertest.cpp
                  test/httpresponseparsertest.cpp
                  test/httpservlettest.cpp
                  test/httpclienttest.cpp
                  ${SOURCES})
#   target_link_libraries(testmain_httpcpp pthread)
   target_link_libraries(testmain_httpcpp ${LIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
   add_test(httpcpp-tests testmain_httpcpp)
endif()
