# Build the Squawk Server
cmake_minimum_required(VERSION 2.8.9)

project(squawk-server)

find_package(GoogleTest REQUIRED)

# find_package(Flac++ REQUIRED)
find_package(Poppler REQUIRED cpp)
set(LIBS ${LIBS} ${POPPLER_LIBRARIES})

find_package(OpenSSL REQUIRED)
set(LIBS ${LIBS} ${OPENSSL_LIBRARIES})

find_package(CURLpp REQUIRED)
set(LIBS ${LIBS} ${CURLPP_LIBRARIES})

set(LIBS ${CMAKE_BINARY_DIR}/httpcpp/libhttpcpp.a ${LIBS})
set(LIBS ${CMAKE_BINARY_DIR}/ssdpcpp/libssdpcpp.a ${LIBS})
set(LIBS ${CMAKE_BINARY_DIR}/external/fmt-3.0.0/fmt/libfmt.a ${LIBS})

#Build mstch
#add_subdirectory(external/mstch)
#include_directories(${mstch_INCLUDE_DIR})

include_directories(${ROOT} ${SQUAWK_INCLUDES} ${POPPLER_INCLUDE_DIRS} ${OPENSSL_INCLUDES} ${CURLPP_INCLUDE_DIRS} src/upnp )

set(UTILS src/utils/media.h src/utils/pdfparser.cpp src/utils/amazonfacade.cpp src/utils/image.cpp)
set(SQUAWK src/squawk.h src/squawkconfig.cpp src/loggereventlistener.cpp)
set(DATABASE src/db/dbexception.h src/db/sqlite3statement.cpp src/db/sqlite3connection.cpp src/db/sqlite3database.cpp)
set(UPNP src/upnp/didl.cpp
         src/upnp/upnp2.h
         src/upnp/didlxmlwriter.cpp
         src/upnp/didljsonwriter.h
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
         src/squawkserver.cpp
)

set(SOURCES ${UTILS} ${SQUAWK} ${DATABASE} ${HTTP_SERVER} ${UPNP})

add_library(squawk ${SOURCES})
add_executable(squawkmain src/squawkmain.cpp)
target_link_libraries(squawkmain squawk ${LIBS})

#add_executable(aws src/utils/aws.cpp ${SOURCES})
#target_link_libraries(aws ${LIBS})

install(TARGETS squawkmain RUNTIME DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/squawk.xml DESTINATION etc/squawk )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/logger.properties DESTINATION etc/squawk )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docroot/ConnectionMgr.xml DESTINATION var/lib/squawk/www )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docroot/ContentDir.xml DESTINATION var/lib/squawk/www )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docroot/X_MS_MediaReceiverRegistrar.xml DESTINATION var/lib/squawk/www )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docroot/icons DESTINATION var/lib/squawk/www )

if (build_tests)
    enable_testing()
    include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR} ${ID3_INCLUDE_DIR} ${FLAC++_INCLUDE_DIR} ${GTEST_INCLUDE_DIRS})
    add_executable(testmain_server test/testmain.cpp
        #test/media/teststringparse.cpp
        # TODO move to toplevel test/testutils.cpp
        test/testoptionsparser.cpp
        # TODO use from http_cpp test/upnp/httpparsetest.cpp
        # TODO use from http_cpp test/http/httpparsertest.cpp
        test/http/mimetypetest.cpp
        test/utils/testmedia.cpp
        test/utils/testamazonfacade.cpp
        test/utils/testisbnparser.cpp
        test/db/testsqlite3connectionmanager.cpp

        test/upnp/TestParseConnectionManager.cpp
        test/upnp/upnptest.cpp
        test/upnp/didlxmlwritertest.cpp
        test/upnp/didljsonwritertest.cpp
        test/testupnpcontentdirectoryapi.cpp
        test/testupnpcontentdirectoryparser.cpp
        test/testupnpcontentdirectorydao.cpp)

    target_link_libraries(testmain_server
        squawk pthread ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${LIBS})
    add_test(squawk-server testmain_server)
endif()
