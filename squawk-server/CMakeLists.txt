# Build the Squawk Server
cmake_minimum_required(VERSION 2.8.9)

project(squawk-server)

find_package(GoogleTest REQUIRED)

# find_package(Flac++ REQUIRED)
find_package(Poppler REQUIRED cpp)
message( ${POPPLER_LIBRARIES} )
set(LIBS ${LIBS} ${POPPLER_LIBRARIES})

# find_package(Lame REQUIRED)
# set(LIBS ${LIBS} ${LAME_LIBRARY})
# find_package(LibMagic REQUIRED)
# set(LIBS ${LIBS} ${LibMagic_LIBRARY})  

set(LIBS ${CMAKE_BINARY_DIR}/httpcpp/libhttpcpp.a ${LIBS})
set(LIBS ${CMAKE_BINARY_DIR}/ssdpcpp/libssdpcpp.a ${LIBS})

include_directories(${ROOT} ${SQUAWK_INCLUDES} ${POPPLER_INCLUDE_DIRS} includes)
#include_directories(${ROOT} ${SQLITE3_INCLUDE_DIR}
#  ${FLAC++_INCLUDE_DIR} ${TINYXML_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${LOG4CXX_INCLUDE_DIR}
#  ${PCREPP_INCLUDE_DIR} ${SQUAWK_INCLUDES} ${LAME_INCLUDE_DIR} includes)
#link_directories(${Boost_LIBRARY_DIR})

set(UTILS src/utils/pdfparser.cpp)
set(SQUAWK src/squawkconfig.cpp src/loggereventlistener.cpp)
set(DATABASE src/db/sqlite3statement.cpp src/db/sqlite3database.cpp)
set(MEDIA src/media/mediadao.cpp src/media/fileparser.cpp)
set(API src/api/apibrowseservlet.cpp src/api/apialbumlistservlet.cpp src/api/apialbumsletterservlet.cpp src/api/apialbumitemservlet.cpp
        src/api/apiartistlistservlet.cpp src/api/apivideolistservlet.cpp
        src/api/mediaservlet.cpp src/api/coverservlet.cpp
        src/http/api/apiupnpdevicehandler.cpp)
set(SERVLET src/servlet/apistatisticservlet.cpp src/servlet/imageservlet.cpp src/servlet/songservlet.cpp )
set(UPNP src/upnp/upnpconnectionmanager.cpp src/upnp/upnpmediaservlet.cpp src/upnp/upnpxmldescription.cpp src/upnp/upnpcontentdirectory.cpp
         src/upnp/upnpmusicdirectorymodule.cpp src/upnp/upnpvideodirectory.cpp src/upnp/upnpfiledirectory.cpp src/upnp/upnpimagedirectory.cpp )
#         src/upnp/upnpwriter.cpp )

set(SOURCES ${UTILS} ${SQUAWK} ${DATABASE} ${MEDIA} ${HTTP_SERVER} ${API} ${SERVLET} ${UPNP})

add_executable(squawk src/squawkserver.cpp ${SOURCES})
target_link_libraries(squawk ${LIBS})

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
        test/upnp/TestParseConnectionManager.cpp
        ${SOURCES})
    target_link_libraries(testmain_server pthread)
    target_link_libraries(testmain_server
        ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${LIBS})
    add_test(squawk-server testmain_server)
endif()

