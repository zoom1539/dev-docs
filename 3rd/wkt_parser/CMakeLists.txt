set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-fext-numeric-literals ${CMAKE_CXX_FLAGS}")

if(EV_DOCKER)
    find_package(OpenCV REQUIRED)
else()
    find_package(OpenCV 3.4.15 REQUIRED PATHS /root/PLATFORM/3rdparty/opencv3415)
endif()

if(EV_DOCKER)
    find_package(Boost REQUIRED)
else()
    set(Boost_DIR /root/PLATFORM/3rdparty/boost_1_76_0)
    set(Boost_INCLUDE_DIRS ${Boost_DIR}/include)
endif()



LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

# libWKTParser.so
add_library(WKTParser SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/WKTParser.cpp)
if(EV_DOCKER)
        target_link_libraries(WKTParser
                ${OpenCV_LIBS}
                boost_system
                boost_filesystem
                boost_thread
                boost_coroutine
                boost_context
                boost_regex
        )
else()
        target_link_libraries(WKTParser
                ${OpenCV_LIBS}
                ${Boost_DIR}/lib/libboost_system.so
                ${Boost_DIR}/lib/libboost_filesystem.so
                ${Boost_DIR}/lib/libboost_thread.so
                ${Boost_DIR}/lib/libboost_coroutine.so
                ${Boost_DIR}/lib/libboost_context.so
                ${Boost_DIR}/lib/libboost_regex.so
        )
endif()
target_include_directories(WKTParser
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${OpenCV_INCLUDE_DIR}
        PRIVATE ${Boost_INCLUDE_DIRS}
)
target_compile_options(WKTParser
        PRIVATE -std=c++11 -Wall -O3 -fPIC)

# Install libWKTParser.so
install(TARGETS WKTParser DESTINATION lib)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    # testWkt
    add_executable(testWktParser ${CMAKE_CURRENT_SOURCE_DIR}/src/test-wkt-parser.cpp)
    target_link_libraries(testWktParser
            ${OpenCV_LIBS}
            WKTParser
    )
    target_include_directories(testWktParser
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    install(TARGETS testWktParser DESTINATION bin)
endif ()