# FindJILicense.cmake
#
# Finds the ji_license library
#
# This will define the following variables
#
#    ji_license_FOUND
#    ji_license_INCLUDE_DIRS
#
# and the following imported targets
#
#     ji_license
#
# Author: huangruhui - huangruhui@extremevision.com.cn

find_package(PkgConfig)
pkg_check_modules(PC_ji_license QUIET ji_license)

find_path(ji_license_INCLUDE_DIR
        NAMES ji_license.h
        PATHS ${PC_ji_license_INCLUDE_DIRS} "/usr/local/ev_sdk/3rd/license/v10/include"
        PATH_SUFFIXES ji_license
)

find_library(ji_license_LIBRARIY
        NAMES ji_license
        PATHS "/usr/local/ev_sdk/3rd/license/v10/lib")

set(ji_license_VERSION ${PC_ji_license_VERSION})

mark_as_advanced(ji_license_FOUND ji_license_INCLUDE_DIR ji_license_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ji_license
        REQUIRED_VARS ji_license_INCLUDE_DIR ji_license_LIBRARIY
        VERSION_VAR ji_license_VERSION
)

if(ji_license_FOUND)
    set(ji_license_INCLUDE_DIRS ${ji_license_INCLUDE_DIR})
    set(ji_license_LIBRARIES ${ji_license_LIBRARIY})
#    add_library(ji_license STATIC IMPORTED)
#    set_target_properties(ji_license PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ji_license_INCLUDE_DIR}")
endif()