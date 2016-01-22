# Build the Squawk Server
cmake_minimum_required(VERSION 2.8.9)

project(squawk-server)

find_package(GoogleTest REQUIRED)

find_package(Boost 1.54.0 COMPONENTS filesystem system)
set(LIBS ${LIBS} ${Boost_LIBRARIES})

# find_package(Flac++ REQUIRED)
find_package(Poppler REQUIRED cpp)
set(LIBS ${LIBS} ${POPPLER_LIBRARIES})

find_package(OpenSSL REQUIRED)
set(LIBS ${LIBS} ${OPENSSL_LIBRARIES})

find_package(CURLpp REQUIRED)
set(LIBS ${LIBS} ${CURLPP_LIBRARIES})

set(LIBS ${CMAKE_BINARY_DIR}/httpcpp/libhttpcpp.a ${LIBS})
set(LIBS ${CMAKE_BINARY_DIR}/ssdpcpp/libssdpcpp.a ${LIBS})

include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${SQUAWK_INCLUDES} ${POPPLER_INCLUDE_DIRS} ${OPENSSL_INCLUDES} ${CURLPP_INCLUDE_DIRS} includes src/upnp )

set(UTILS src/utils/pdfparser.cpp src/utils/amazonfacade.cpp src/utils/image.cpp )
set(SQUAWK src/squawkconfig.cpp src/loggereventlistener.cpp)
set(DATABASE src/db/dbexception.h src/db/sqlite3statement.cpp src/db/sqlite3connection.cpp src/db/sqlite3database.cpp)
set(MEDIA src/media/image.cpp src/media/album.cpp  src/media/mediadao.cpp src/media/fileparser.cpp)
set(API src/api/apibrowseservlet.cpp src/api/apialbumlistservlet.cpp src/api/apialbumitemservlet.cpp src/api/apiartistlistservlet.cpp
        src/api/apivideolistservlet.cpp src/api/apivideoitemservlet.cpp src/api/mediaservlet.cpp src/api/coverservlet.cpp
        src/api/apiupnpeventservlet.cpp src/api/apiupnpdeviceservlet.cpp src/api/apistatisticsservlet.cpp src/api/apibooklistservlet.cpp)
set(SERVLET src/servlet/imageservlet.cpp src/servlet/songservlet.cpp)
set(UPNP src/upnp/didl.h src/upnp/upnp2.h
         src/upnpconnectionmanager.cpp
         src/upnpcontentdirectory.cpp
         src/upnpcontentdirectoryapi.cpp
         src/upnpcontentdirectorydao.cpp
         src/upnpcontentdirectoryfile.cpp
         src/upnpcontentdirectoryimage.cpp
         src/upnpcontentdirectorymusic.cpp
         src/upnpcontentdirectoryparser.cpp
         src/upnpcontentdirectoryvideo.cpp
         src/upnpmediaservlet.cpp
         src/upnpxmldescription.cpp
)

set(SOURCES ${UTILS} ${SQUAWK} ${DATABASE} ${MEDIA} ${HTTP_SERVER} ${API} ${SERVLET} ${UPNP})

add_executable(squawk src/squawkserver.cpp ${SOURCES})
target_link_libraries(squawk ${LIBS})

add_executable(aws src/utils/aws.cpp ${SOURCES})
target_link_libraries(aws ${LIBS})

install(TARGETS squawk RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/squawk.xml DESTINATION etc/squawk )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/logger.properties DESTINATION etc/squawk )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docroot/ConnectionMgr.xml DESTINATION var/lib/squawk/www )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docroot/ContentDir.xml DESTINATION var/lib/squawk/www )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docroot/X_MS_MediaReceiverRegistrar.xml DESTINATION var/lib/squawk/www )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docroot/icons DESTINATION var/lib/squawk/www )

if (build_tests)
    enable_testing()
    include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR} ${ID3_INCLUDE_DIR} ${FLAC++_INCLUDE_DIR} ${GTEST_INCLUDE_DIRS} includes)
    add_executable(testmain_server test/testmain.cpp
        test/TestTextParse.cpp
        test/TestConfigParser.cpp
        #test/media/teststringparse.cpp
        test/media/testartist.cpp
        test/api/testapiartisthandler.cpp
        # TODO move to toplevel test/testutils.cpp
        test/testoptionsparser.cpp
        # TODO use from http_cpp test/upnp/httpparsetest.cpp
        # TODO use from http_cpp test/http/httpparsertest.cpp
        test/http/mimetypetest.cpp
        test/utils/testamazonfacade.cpp
        test/utils/testisbnparser.cpp
        test/db/testsqlite3connectionmanager.cpp

        test/upnp/TestParseConnectionManager.cpp
        test/upnp/didltest.cpp
        test/upnp/upnptest.cpp
        test/testupnpcontentdirectoryparser.cpp

        ${SOURCES})
    target_link_libraries(testmain_server pthread)
    target_link_libraries(testmain_server
        ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${LIBS})
    add_test(squawk-server testmain_server)
endif()

