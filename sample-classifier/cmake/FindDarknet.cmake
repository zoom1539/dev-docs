# FindJILicense.cmake
#
# Finds the darknet library
#
# This will define the following variables
#
#    darknet_FOUND
#    darknet_INCLUDE_DIRS
#
# and the following imported targets
#
#     darknet
#
# Author: huangruhui - huangruhui.v@gmail.com

find_package(PkgConfig)
pkg_check_modules(PC_darknet QUIET darknet)

find_path(darknet_INCLUDE_DIR
        NAMES darknet.h
        PATHS ${PC_darknet_INCLUDE_DIRS} "/usr/local/ev_sdk/sample-classifier/darknet/include"
        PATH_SUFFIXES darknet
)

find_library(darknet_LIBRARIY
        NAMES darknet
        PATHS "/usr/local/ev_sdk/sample-classifier/darknet/lib")

set(darknet_VERSION ${PC_darknet_VERSION})

mark_as_advanced(darknet_FOUND darknet_INCLUDE_DIR darknet_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(darknet
        REQUIRED_VARS darknet_INCLUDE_DIR darknet_LIBRARIY
        VERSION_VAR darknet_VERSION
)

if(darknet_FOUND)
    set(darknet_INCLUDE_DIRS ${darknet_INCLUDE_DIR})
    set(darknet_LIBRARIES ${darknet_LIBRARIY})
#    add_library(darknet STATIC IMPORTED)
#    set_target_properties(darknet PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${darknet_INCLUDE_DIR}")
endif()