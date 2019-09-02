cmake_minimum_required(VERSION 2.8)
project(ji)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

find_package(OpenCV REQUIRED)
find_library(glog REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/cJSON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/boost_interface)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/encrypted_module/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/license/v10)

# libji.so
add_library (${PROJECT_NAME} SHARED ${PROJECT_SOURCE_DIR}/src/ji.cpp)
target_include_directories(${PROJECT_NAME}
        PRIVATE ${OpenCV_INCLUDE_DIR}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBS}
        crypto
        curl
        glog
        ji_license
        encrypt_wrapper
        cjson
        BoostInterface
)

# libji.so
install(
        TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
)
