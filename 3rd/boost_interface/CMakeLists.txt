find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)

# libBoostInterface.so
add_library(BoostInterface ${CMAKE_CURRENT_SOURCE_DIR}/src/BoostInterface.cpp)
target_link_libraries(BoostInterface
        ${OpenCV_LIBS}
        boost_system
        boost_filesystem
        boost_thread
        boost_coroutine
        boost_context
        boost_regex
)
target_include_directories(BoostInterface
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${OpenCV_INCLUDE_DIR}
        PRIVATE ${Boost_INCLUDE_DIRS}
)
target_compile_options(BoostInterface
        PRIVATE -std=c++11 -Wall -O3 -fPIC)

# testWkt
add_executable(testWkt ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(testWkt
        ${OpenCV_LIBS}
        BoostInterface
)
target_include_directories(testWkt
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(TARGETS BoostInterface DESTINATION lib)
install(TARGETS testWkt DESTINATION bin)