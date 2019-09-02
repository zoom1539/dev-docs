add_library(encrypt_wrapper INTERFACE)
target_include_directories(encrypt_wrapper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

install(
        TARGETS encrypt_wrapper DESTINATION lib
)
install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib/libev_base64.so
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libev_confuse.so
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libev_encrypt.so
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libev_encrypt_api.so
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libev_encrypt_suite.so
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/libev_file_helper.so DESTINATION lib
)