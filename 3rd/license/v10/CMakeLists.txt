add_library(ji_license INTERFACE)
target_include_directories(ji_license INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(
        TARGETS ji_license DESTINATION lib
)

# license tools
install(
        PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/clear.sh
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/ev_codec
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/ev_license
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/oneKeyTest.sh
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/oneKeyTest_n.sh DESTINATION bin/license-tools/v10/
)