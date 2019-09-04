# FindEncryptModule.cmake
#
# Finds the encrypt_module library
#
# This will define the following variables
#
#    encrypt_module_FOUND
#    encrypt_module_INCLUDE_DIRS
#
# and the following imported targets
#
#     encrypt_module
#
# Author: huangruhui - huangruhui.v@gmail.com

find_package(PkgConfig)
pkg_check_modules(PC_encrypt_module QUIET encrypt_module)

find_path(encrypt_module_INCLUDE_DIR
        NAMES encrypt_wrapper.hpp
        PATHS ${PC_encrypt_module_INCLUDE_DIRS} "/usr/local/ev_sdk/3rd/encrypt_module/include"
        PATH_SUFFIXES encrypt_module
)

find_library(encrypt_module_LIBRARIY
        NAMES encrypt_wrapper
        PATHS "/usr/local/ev_sdk/3rd/encrypt_module/lib")

set(encrypt_module_VERSION ${PC_encrypt_module_VERSION})

mark_as_advanced(encrypt_module_FOUND encrypt_module_INCLUDE_DIR encrypt_module_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(encrypt_module
        REQUIRED_VARS encrypt_module_INCLUDE_DIR encrypt_module_LIBRARIY
        VERSION_VAR encrypt_module_VERSION
)

if(encrypt_module_FOUND)
    set(encrypt_module_INCLUDE_DIRS ${encrypt_module_INCLUDE_DIR})
    set(encrypt_module_LIBRARIES ${encrypt_module_LIBRARIY})
#    add_library(encrypt_module STATIC IMPORTED)
#    set_target_properties(encrypt_module PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${encrypt_module_INCLUDE_DIR}")
endif()