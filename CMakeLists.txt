# Build httpcpp
# run the cmake command from your build directory:
# cmake .. -DCMAKE_COMPILER_IS_GNUCXX=true -Dbuild_tests=true -DCMAKE_BUILD_TYPE=debug
cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
option(build_tests "Build all squawk unit tests." OFF)

project (squawk)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()
set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} -O0)

find_package(GoogleTest REQUIRED)

SET( SQUAWK_INCLUDES ${CMAKE_SOURCE_DIR}/commons ${CMAKE_SOURCE_DIR}/httpcpp/includes ${CMAKE_SOURCE_DIR}/ssdpcpp/includes
     /home/e3a/asio-1.10.2/include/ )

#add_subdirectory (commons)
add_subdirectory (httpcpp)
add_subdirectory (ssdpcpp)
add_subdirectory (squawk-server)

if (build_tests)
    enable_testing()
    include_directories(${ROOT} ${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} includes)
    add_executable(testmain test/XmlEncodeTest.cpp )
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
