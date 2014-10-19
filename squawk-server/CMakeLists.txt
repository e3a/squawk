# Build the Squawk Server
cmake_minimum_required(VERSION 2.8.9)

project(squawk-server)

find_package(GoogleTest REQUIRED)
find_package(Boost COMPONENTS system filesystem serialization thread regex REQUIRED)
set(LIBS ${LIBS} ${Boost_LIBRARIES})
find_package(Sqlite3 REQUIRED)
set(LIBS ${LIBS} ${SQLITE3_LIBRARY}) 
# find_package(ID3 REQUIRED)
# set(LIBS ${LIBS} ${ID3_LIBRARY})
find_package(Flac++ REQUIRED)
set(LIBS ${LIBS} ${FLAC++_LIBRARIES})  

# find_package(TinyXML REQUIRED)
# set(LIBS ${LIBS} ${TINYXML_LIBRARIES})
find_package(LibXml2 REQUIRED)
set(LIBS ${LIBS} ${LIBXML2_LIBRARIES})


find_package(Log4cxx REQUIRED)
set(LIBS ${LIBS} ${LOG4CXX_LIBRARIES})  
find_package(Imlib2 REQUIRED)
set(LIBS ${LIBS} ${IMLIB2_LIBRARIES})  
find_package(PCRE++ REQUIRED)
set(LIBS ${LIBS} -lpcrecpp)  
find_package(Lame REQUIRED)
set(LIBS ${LIBS} ${LAME_LIBRARY})
find_package(LibMagic REQUIRED)
MESSAGE(" Found Lame: " ${LAME_FOUND} " - " ${LAME_INCLUDE_DIR})
set(LIBS ${LIBS} ${LibMagic_LIBRARY})  
find_package(LibAVCodec COMPONENTS avformat avcodec avutil avresample)
set(LIBS ${LIBS} ${AVCODEC_LIBRARIES})
set(LIBS ${LIBS} ${CMAKE_BINARY_DIR}/httpcpp/liblibhttpcpp.a)
set(LIBS ${LIBS} ${CMAKE_BINARY_DIR}/ssdpcpp/liblibssdpcpp.a)

include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR}
  ${FLAC++_INCLUDE_DIR} ${TINYXML_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${LOG4CXX_INCLUDE_DIR} ${IMLIB2_INCLUDE_DIR}
  ${PCREPP_INCLUDE_DIR} ${AVCODEC_INCLUDES} ${SQUAWK_INCLUDES} ${LAME_INCLUDE_DIR} includes)
link_directories(${Boost_LIBRARY_DIR})

set(SQUAWK src/squawkconfig.cpp src/squawkservice.cpp)
set(DATABASE src/db/sqlite3statement.cpp src/db/sqlite3database.cpp src/db/squawkdao.cpp )
set(MEDIA src/media/mediadao.cpp src/media/libavcpp.cpp src/media/fileparser.cpp src/media/flacparser.cpp)
set(API src/http/api/apiupnpdevicehandler.cpp)
set(SERVLET src/servlet/apialbumsbyartist.cpp src/servlet/apiartistservlet.cpp src/servlet/apialbumsservlet.cpp src/servlet/apialbumservlet.cpp
            src/servlet/apistatisticservlet.cpp src/servlet/coverservlet.cpp src/servlet/imageservlet.cpp src/servlet/songservlet.cpp src/servlet/upnpxmldescription.cpp
            src/servlet/upnpcontentdirectory.cpp src/servlet/upnpmusicdirectorymodule.cpp )

set(SOURCES ${SQUAWK} ${DATABASE} ${MEDIA} ${HTTP_SERVER} ${API} ${SERVLET})

add_executable(av src/media/LibavMain.cpp ${SOURCES})  #TODO remove
add_executable(squawk src/squawkserver.cpp ${SOURCES})
target_link_libraries(squawk ${LIBS})
target_link_libraries(av ${LIBS})

if (build_tests)
    enable_testing()
    include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR} ${ID3_INCLUDE_DIR} ${FLAC++_INCLUDE_DIR} ${GTEST_INCLUDE_DIRS} includes)
    add_executable(testmain_server test/testmain.cpp
        test/media/teststringparse.cpp
        test/api/testapiartisthandler.cpp
        # TODO move to toplevel test/testutils.cpp
        test/testoptionsparser.cpp
        # TODO use from http_cpp test/upnp/httpparsetest.cpp
        # TODO use from http_cpp test/http/httpparsertest.cpp
        test/http/mimetypetest.cpp
        ${SOURCES})
    target_link_libraries(testmain_server pthread)
    target_link_libraries(testmain_server
        ${LIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
    add_test(squawk-tests testmain_server)
endif()

