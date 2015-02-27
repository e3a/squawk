# Build squawk project
# run the cmake command from your build directory:
# cmake .. -DCMAKE_COMPILER_IS_GNUCXX=true -Dbuild_tests=true -DCMAKE_BUILD_TYPE=debug -DTESTFILES='"/home/e3a/testfiles/"' -DASIO_PATH=~/asio-1.10.2/include/
cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
option(build_tests "Build all squawk unit tests." OFF)

project (squawk)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()
set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} -O0)
add_definitions(-DASIO_STANDALONE)
add_definitions(-DTESTFILES=${TESTFILES})
add_definitions(-Wall)

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

SET( SQUAWK_INCLUDES ${CMAKE_SOURCE_DIR}/commons ${CMAKE_SOURCE_DIR}/httpcpp/includes ${CMAKE_SOURCE_DIR}/ssdpcpp/includes
     ${ASIO_PATH} ${LIBXML2_INCLUDE_DIR} ${IMLIB2_INCLUDE_DIR} ${AVCODEC_INCLUDES})

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
    target_link_libraries(testmain pthread)
    target_link_libraries(testmain
        ${LIBS} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
    add_test(squawk-tests testmain)
endif()


# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
