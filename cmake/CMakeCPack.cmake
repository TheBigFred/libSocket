################################################################################
## File      : CMakeCPack.cmake
## Contents  : 
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

set(CPACK_PACKAGE_NAME   "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Frederic Gerard - mailfge@gmail.com")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A low level portable socket library")
set(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_CURRENT_LIST_DIR}/../LICENSE")
set(CPACK_RESOURCE_FILE_README        "${CMAKE_CURRENT_LIST_DIR}/../README.md")

set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${PROJECT_NAME}")

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_BINARY_DIR};/.git/;.gitignore;.svn")
set(CPACK_SOURCE_GENERATOR "ZIP;TGZ")
set(CPACK_GENERATOR "ZIP;TGZ")

if (UNIX AND CMAKE_SYSTEM_NAME MATCHES Linux)
   find_program(DPKG_FOUND dpkg)
   if (DPKG_FOUND)
      list(APPEND CPACK_GENERATOR "DEB")
      set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Frederic Gerard - mailfge@gmail.com")
      set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
   endif()

   find_program(RPMBUILD_FOUND rpmbuild)
   if (RPMBUILD_FOUND)
      list(APPEND CPACK_GENERATOR "RPM")
      set(CPACK_RPM_PACKAGE_RELEASE "1")
      set(CPACK_RPM_PACKAGE_LICENSE "LGPL")
   endif()
endif()

if(WIN32 OR MINGW)
   list(APPEND CPACK_GENERATOR "NSIS")
   set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
   set(CPACK_NSIS_CONTACT "Frederic Gerard - mailfge@gmail.com")
   set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
endif()

if(APPLE)
   list(APPEND CPACK_GENERATOR "Bundle")
   set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
   configure_file(${CMAKE_CURRENT_LIST_DIR}/Info.plist.in Info.plist @ONLY)
   set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
   set(CPACK_BUNDLE_ICON ${CMAKE_CURRENT_LIST_DIR}/coffee.icns)
endif()

include(CPack)
