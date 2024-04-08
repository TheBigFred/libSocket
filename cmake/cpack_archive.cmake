################################################################################
## File      : cpack_archive.cmake
## Contents  : CPack tar.gz & .zip configuration
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

if (UNIX)
    list(APPEND CPACK_GENERATOR "TGZ")
elseif(WIN32)
    list(APPEND CPACK_GENERATOR "ZIP")
endif()

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

set(ar_releasePkgName   "${CPACK_PACKAGE_NAME}")
set(ar_debugPkgName     "${CPACK_PACKAGE_NAME}-debug")
set(ar_develPkgName     "${CPACK_PACKAGE_NAME}-devel")

set(CPACK_ARCHIVE_COMPONENT_INSTALL  ON)
set(CPACK_ARCHIVE_RUNTIME_FILE_NAME ${ar_releasePkgName})
set(CPACK_ARCHIVE_DEVEL_FILE_NAME   ${ar_develPkgName})
set(CPACK_ARCHIVE_DEBUG_FILE_NAME   ${ar_debugPkgName})
