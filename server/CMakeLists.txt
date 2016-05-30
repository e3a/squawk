# Build the Squawk Server
cmake_minimum_required(VERSION 3.2.2)

project(server)

set(UTILS src/utils/media.h src/utils/pdfparser.cpp src/utils/amazonfacade.cpp src/utils/image.cpp)
set(SQUAWK src/squawk.h src/squawkconfig.cpp)
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

include_directories(${ROOT} ${SQUAWK_INCLUDES} src/upnp )
add_library(server ${UTILS} ${SQUAWK} ${DATABASE} ${UPNP})

#add_executable(aws src/utils/aws.cpp ${SOURCES})
#target_link_libraries(aws ${LIBS})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/squawk.xml DESTINATION etc/squawk )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/logger.properties DESTINATION etc/squawk )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docroot/ConnectionMgr.xml DESTINATION var/lib/squawk/www )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docroot/ContentDir.xml DESTINATION var/lib/squawk/www )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/docroot/X_MS_MediaReceiverRegistrar.xml DESTINATION var/lib/squawk/www )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/squawk-init-script DESTINATION /etc/init.d/ RENAME squawk
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/squawk-default DESTINATION /etc/default/ RENAME squawk-server )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docroot/ DESTINATION var/lib/squawk/www )

if (build_tests)
    enable_testing()
    include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR} ${ID3_INCLUDE_DIR} ${FLAC++_INCLUDE_DIR} ${GTEST_INCLUDE_DIRS})
    add_executable(testmain_server
        test/testmain.cpp
        test/testsquawkconfig.cpp
        test/utils/testmedia.cpp
        test/utils/testamazonfacade.cpp
        test/utils/testisbnparser.cpp
        test/utils/XmlParseTest.cpp
        test/utils/XmlWriterTest.cpp
        test/db/testsqlite3connectionmanager.cpp
        test/upnp/TestParseConnectionManager.cpp
        test/upnp/upnptest.cpp
        test/upnp/didlxmlwritertest.cpp
        test/upnp/didljsonwritertest.cpp
        test/testupnpcontentdirectoryapi.cpp
        test/upnpcontentdirectoryparsertest.cpp
        test/testupnpcontentdirectorydao.cpp)
    target_link_libraries(testmain_server
        server ssdpcpp httpcpp fmt pthread ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${LIBS})
    add_test(server-tests testmain_server)
endif()
