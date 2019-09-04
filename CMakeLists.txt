cmake_minimum_required(VERSION 2.8)
project(ji)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_INSTALL_PREFIX "/usr/local/ev_sdk")

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)
find_package(JILicense REQUIRED)
find_package(EncryptModule REQUIRED)
find_package(OpenCV REQUIRED)
find_library(glog REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/cJSON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/boost_interface)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/license/v10)

# 在这里添加开发者的cmake工程
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sample-classifier)

# 编译libji.so
add_library (${PROJECT_NAME} SHARED ${PROJECT_SOURCE_DIR}/src/ji.cpp)
target_include_directories(${PROJECT_NAME}
        PRIVATE ${OpenCV_INCLUDE_DIR}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${ji_license_INCLUDE_DIRS}
        PUBLIC ${encrypt_module_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBS}
        crypto
        curl
        glog
        ${ji_license_LIBRARIES}
        ${encrypt_module_LIBRARIES}
        cjson
        BoostInterface
        ClassifierPredictor
)

# 编译 JI API 测试程序
add_executable(test-ji-api ${CMAKE_CURRENT_SOURCE_DIR}/test/src/test.cpp)
target_link_libraries(test-ji-api
        ${OpenCV_LIBS}
        cjson
        glog
        ji
)

# install libji.so
install(TARGETS ${PROJECT_NAME} DESTINATION lib)

# install test-ji-api
install(TARGETS test-ji-api DESTINATION bin)

# install encrypt_module
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/encrypt_module/lib/libencrypt_wrapper.so DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/encrypt_module/lib/libev_encrypt.so DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/encrypt_module/lib/libev_encrypt_api.so DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/encrypt_module/lib/libev_encrypt_suite.so DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/encrypt_module/lib/libev_base64.so DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/encrypt_module/lib/libev_confuse.so DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rd/encrypt_module/lib/libev_file_helper.so DESTINATION lib)