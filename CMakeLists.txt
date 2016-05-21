# -------------------------------------------------------------------------
# Build Squawk Project
# run the cmake command from your build directory:
# -------------------------------------------------------------------------
# Example Usage:
# cmake .. -DCMAKE_C_COMPILER=gcc-6 -DCMAKE_CXX_COMPILER=/usr/bin/g++-6 -Dbuild_tests=true -DCMAKE_BUILD_TYPE=debug -DTESTFILES=~/testfiles/
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.2.2)

project (squawk)

SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "1")
SET(PATCH_VERSION "1" CACHE PATCH_VERSION "The package build number." )

# -------------------------------------------------------------------------
# -----------------------   Options and Parameters  -----------------------
# -------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
option(build_tests "Build all squawk unit tests." ON)
option(CMAKE_COMPILER_IS_GNUCXX "is the compiler gnucxx" OFF)
SET(FORMAT_PATH "external/fmt-3.0.0/" CACHE FORMAT_PATH "the path to the c++ format library.")
SET(MSTCH_PATH "external/mstch-1.0.2" CACHE MSTCH_PATH "the path to the c++ mustache (mstch) library.")
SET(TESTFILES "/home/e3a/testfiles" CACHE TESTFILES "The path to the testfiles.")
SET(ASIO_PATH "${CMAKE_SOURCE_DIR}/external/asio-1.10.6" CACHE ASIO_PATH "The path to asio header files.")
SET(BOWER_COMPONENTS "angular-animate" "angular-aside" "angular-bootstrap" "angular-route" "angular-sanitize" "bootstrap" "ngGallery" "videogular"
                     "dirPagination" "font-awesome" "api-console" "jquery" "angular-utils-pagination")

# -------------------------------------------------------------------------
# -----------------------   Config Toolchain        -----------------------
# -------------------------------------------------------------------------
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()
set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} -O0 -g)
add_definitions(-DASIO_STANDALONE)
add_definitions(-DSQUAWK_RELEASE="${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}/")
add_definitions(-DTESTFILES="${TESTFILES}/")
add_definitions(-DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}/")
add_definitions(-Wall -Wextra)

# -------------------------------------------------------------------------
# -----------------------   Find Dependencies       -----------------------
# -------------------------------------------------------------------------
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
find_package(LibAVCodec COMPONENTS avformat avcodec avutil)
set(LIBS ${LIBS} ${AVCODEC_LIBRARIES})
find_package(Boost 1.54.0 COMPONENTS filesystem system)
set(LIBS ${LIBS} ${Boost_LIBRARIES})
find_package(Poppler REQUIRED cpp)
set(LIBS ${LIBS} ${POPPLER_LIBRARIES})
find_package(CURLpp REQUIRED)
set(LIBS ${LIBS} ${CURLPP_LIBRARIES})
find_package(OpenSSL REQUIRED)
set(LIBS ${LIBS} ${OPENSSL_LIBRARIES})

set(LIBS ${LIBS} -luuid -lpthread -lm) #needed for ubuntu

SET( SQUAWK_INCLUDES ${CMAKE_SOURCE_DIR}/httpcpp/includes ${CMAKE_SOURCE_DIR}/ssdpcpp/includes
     ${Boost_INCLUDE_DIRS} ${ASIO_PATH}/include/ ${LIBXML2_INCLUDE_DIR} ${IMLIB2_INCLUDE_DIR}
     ${AVCODEC_INCLUDES} ${CURLPP_INCLUDE_DIRS} ${OPENSSL_INCLUDES} ${CMAKE_SOURCE_DIR}/${FORMAT_PATH})

# -------------------------------------------------------------------------
# -----------------------   Compile Libraries       -----------------------
# -------------------------------------------------------------------------
add_subdirectory (${FORMAT_PATH} EXCLUDE_FROM_ALL)
add_subdirectory (${MSTCH_PATH} EXCLUDE_FROM_ALL)
SET( SQUAWK_INCLUDES ${SQUAWK_INCLUDES} ${mstch_INCLUDE_DIR})
add_subdirectory (httpcpp)
add_subdirectory (ssdpcpp)
add_subdirectory (server)

# -------------------------------------------------------------------------
# -----------------------   Compile Binary          -----------------------
# -------------------------------------------------------------------------
include_directories(${ROOT} ${SQUAWK_INCLUDES} ${CMAKE_SOURCE_DIR}/server/src/ ${CMAKE_SOURCE_DIR}/server/src/upnp )
add_executable(squawkmain squawkmain.cpp)
target_link_libraries(squawkmain server ssdpcpp httpcpp fmt ${GTEST_LIBRARIES} ${LIBS})
install(TARGETS squawkmain RUNTIME DESTINATION bin)

# -------------------------------------------------------------------------
# -----------------------   Manage Test Cases       -----------------------
# -------------------------------------------------------------------------
if (build_tests)
    enable_testing()
endif()

# -------------------------------------------------------------------------
# -----------------------   Bower Components        -----------------------
# -------------------------------------------------------------------------
add_custom_target(bower /usr/local/bin/bower --no-color -f -s --allow-root install ${BOWER_COMPONENTS}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Download Bower Components" VERBATIM
)
install(DIRECTORY ${CMAKE_BINARY_DIR}/bower_components/ DESTINATION var/lib/squawk/bower_components )

# -------------------------------------------------------------------------
# -----------------------   Doxygen                 -----------------------
# -------------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

# -------------------------------------------------------------------------
# -----------------------   Generate Package        -----------------------
# -------------------------------------------------------------------------
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_SET_DESTDIR "on")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
SET(CPACK_GENERATOR "DEB")

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

SET(CPACK_PACKAGE_DESCRIPTION "loudlabs media server")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "squawk DLNA media server.")
SET(CPACK_PACKAGE_VENDOR "loudlabs")
SET(CPACK_PACKAGE_CONTACT "e.knecht@netwings.ch")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libpcrecpp0, liblog4cxx10, libimlib2, libavcodec54, libavformat54, libavutil52, libpoppler-cpp0, libcurlpp0, libboost-filesystem1.54.0")
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION "media")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
SET(CPACK_COMPONENTS_ALL applications Libraries ApplicationData)
INCLUDE(CPack)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
