# FindEncryptModule.cmake
#
# Finds the ev_encrypt_module library
#
# This will define the following variables
#
#    ev_encrypt_module_FOUND
#    ev_encrypt_module_INCLUDE_DIRS
#
# and the following imported targets
#
#     ev_encrypt_module
#
# Author: huangruhui - huangruhui@extremevision.com.cn

find_package(PkgConfig)
pkg_check_modules(PC_ev_encrypt_module QUIET ev_encrypt_module)

find_path(ev_encrypt_module_INCLUDE_DIR
        NAMES encrypt_wrapper.hpp
        PATHS ${PC_ev_encrypt_module_INCLUDE_DIRS} "/usr/local/ev_sdk/3rd/ev_encrypt_module/include"
        PATH_SUFFIXES ev_encrypt_module
)

find_library(ev_encrypt_module_LIBRARIY
        NAMES ev_encrypt_module
        PATHS "/usr/local/ev_sdk/3rd/ev_encrypt_module/lib")

set(ev_encrypt_module_VERSION ${PC_ev_encrypt_module_VERSION})

mark_as_advanced(ev_encrypt_module_FOUND ev_encrypt_module_INCLUDE_DIR ev_encrypt_module_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ev_encrypt_module
        REQUIRED_VARS ev_encrypt_module_INCLUDE_DIR ev_encrypt_module_LIBRARIY
        VERSION_VAR ev_encrypt_module_VERSION
)

if(ev_encrypt_module_FOUND)
    set(ev_encrypt_module_INCLUDE_DIRS ${ev_encrypt_module_INCLUDE_DIR})
    set(ev_encrypt_module_LIBRARIES ${ev_encrypt_module_LIBRARIY})
#    add_library(ev_encrypt_module STATIC IMPORTED)
#    set_target_properties(ev_encrypt_module PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ev_encrypt_module_INCLUDE_DIR}")
endif()