# Build squawk project
# run the cmake command from your build directory:
# cmake .. -DCMAKE_COMPILER_IS_GNUCXX=true -Dbuild_tests=true -DCMAKE_BUILD_TYPE=debug -DTESTFILES=/home/e3a/testfiles/ -DASIO_PATH=/home/e3a/asio-1.10.2/
cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
option(build_tests "Build all squawk unit tests." ON)
option(CMAKE_COMPILER_IS_GNUCXX "is the compiler gnucxx" OFF)
SET(TESTFILES "/home/e3a/testfiles" CACHE TESTFILES "The path to the testfiles.")
SET(ASIO_PATH "${CMAKE_SOURCE_DIR}/contrib/asio-1.10.6/" CACHE ASIO_PATH "The path to asio header files.")
SET(BOWER_COMPONENTS "angular-animate" "angular-aside" "angular-bootstrap" "angular-route" "angular-sanitize" "bootstrap" "ngGallery" "videogular" "dirPagination" "font-awesome")

project (squawk)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()
set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} -O0)
add_definitions(-DASIO_STANDALONE)
add_definitions(-DTESTFILES="${TESTFILES}/")
add_definitions(-DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}/")
add_definitions(-Wall -Wextra)

find_package(GoogleTest REQUIRED)
find_package(LibXml2 REQUIRED)
set(LIBS ${LIBS} ${LIBXML2_LIBRARIES})
find_package(Sqlite3 REQUIRED)
set(LIBS ${LIBS} ${SQLITE3_LIBRARY})
find_package(PCRE++ REQUIRED)
set(LIBS ${LIBS} -lpcrecpp)
find_package(Log4cxx REQUIRED)
set(LIBS ${LIBS} ${LOG4CXX_LIBRARIES})
find_package(Imlib2 REQUIRED)
set(LIBS ${LIBS} ${IMLIB2_LIBRARIES})
#find_package(LibAVCodec COMPONENTS avformat avcodec avutil avresample)
find_package(LibAVCodec COMPONENTS avformat avcodec avutil)
set(LIBS ${LIBS} ${AVCODEC_LIBRARIES})
set(LIBS ${LIBS} -luuid -lpthread -lm) #needed for ubuntu

SET( SQUAWK_INCLUDES ${CMAKE_SOURCE_DIR}/commons ${CMAKE_SOURCE_DIR}/httpcpp/includes ${CMAKE_SOURCE_DIR}/ssdpcpp/includes
     ${ASIO_PATH}/include/ ${LIBXML2_INCLUDE_DIR} ${IMLIB2_INCLUDE_DIR} ${AVCODEC_INCLUDES})

add_subdirectory (httpcpp)
add_subdirectory (ssdpcpp)
add_subdirectory (squawk-server)

add_executable(media mediafile.cpp)
target_link_libraries(media ${LIBS})

if (build_tests)
    enable_testing()
    #include_directories(${ROOT} ${SQUAWK_INCLUDES} ${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} includes)
    include_directories(${ROOT} ${SQUAWK_INCLUDES} ${GTEST_INCLUDE_DIRS} includes)
    add_executable(testmain test/CommonsStringTest.cpp test/MediaTest.cpp
                            test/XmlEncodeTest.cpp test/XmlParseTest.cpp test/XmlWriterTest.cpp test/UpnpTest.cpp )
#    target_link_libraries(testmain pthread)
    target_link_libraries(testmain
        ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${LIBS})
    add_test(squawk-tests testmain)
endif()


add_custom_target(bower /usr/local/bin/bower --allow-root install ${BOWER_COMPONENTS}
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/squawk-server/docroot
COMMENT "Download Bower Components" VERBATIM
)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "0")
SET(PATCH_VERSION "1")

IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_SET_DESTDIR "on")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
SET(CPACK_GENERATOR "DEB")

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

SET(CPACK_PACKAGE_DESCRIPTION "squawk medua server")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "squawk DLNA media server.")
SET(CPACK_PACKAGE_VENDOR "loudlabs")
SET(CPACK_PACKAGE_CONTACT "e.knecht@netwings.ch")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
#demo dependencies
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libpcrecpp0, liblog4cxx10, libimlib2, libavcodec54, libavformat54, libavutil52")

#dependencies for this service menu
# SET(CPACK_DEBIAN_PACKAGE_DEPENDS " kate , dolphin ")

SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION "media")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

SET(CPACK_COMPONENTS_ALL applications Libraries ApplicationData)
INCLUDE(CPack)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
