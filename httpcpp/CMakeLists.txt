# Build httpcpp
cmake_minimum_required(VERSION 3.2.2)

project(httpcpp)

set(HTTP_SOURCES
    src/httprequestparser.cpp
    src/httpresponseparser.cpp
    src/httpservlet.cpp
    src/webserver.cpp
    src/httpresponse.cpp
    src/httprequest.cpp
    src/asio/httpconnection.cpp
    src/asio/httpserver.cpp
    src/servlet/fileservlet.cpp
    src/httpclient.cpp
    src/asio/httpclientconnection.cpp
)

include_directories(${SQUAWK_INCLUDES} ${PROJECT_SOURCE_DIR}/src/asio)
add_library(httpcpp ${HTTP_SOURCES})

if (build_tests)
   enable_testing()
   include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} includes)
   add_executable(testmain_httpcpp
                  test/testmain.cpp
                  test/mimetypetest.cpp
                  test/httpparsertest.cpp
                  test/httpresponseparsertest.cpp
                  test/httpservlettest.cpp
                  test/httpclienttest.cpp
                  test/urlencodetest.cpp)
   target_link_libraries(testmain_httpcpp httpcpp ${LIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
   add_test(httpcpp-tests testmain_httpcpp)
endif()
