################################################################################
## File      : cpack_debuild.cmake
## Contents  : Debian cmake packager
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

if (UNIX AND CMAKE_SYSTEM_NAME MATCHES Linux)
   find_program(DEBBUILD_FOUND debuild)
   if (NOT DEBBUILD_FOUND)
      message(STATUS "debuild not found")
   else()

      list(APPEND CPACK_GENERATOR           "DEB")
      set(CPACK_DEBIAN_PACKAGE_RELEASE      "1")
      set(CPACK_DEBIAN_PACKAGE_MAINTAINER   "TheBigFred - thebigfred.github@gmail.com")
      set(CPACK_DEBIAN_PACKAGE_SECTION      "devel")
      set(CPACK_DEBIAN_PACKAGE_DEPENDS      "")

      set(deb_releasePkgName   "${CPACK_PACKAGE_NAME}.deb")
      set(deb_debugPkgName     "${CPACK_PACKAGE_NAME}-debug.deb")
      set(deb_develPkgName     "${CPACK_PACKAGE_NAME}-devel.deb")

      set(CPACK_DEB_COMPONENT_INSTALL       ON)
      set(CPACK_DEBIAN_RUNTIME_FILE_NAME    ${deb_releasePkgName})
      set(CPACK_DEBIAN_DEVEL_FILE_NAME      ${deb_develPkgName})
      set(CPACK_DEBIAN_DEBUG_FILE_NAME      ${deb_debugPkgName})
         
      set(CPACK_DEBIAN_RUNTIME_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
      set(CPACK_DEBIAN_DEVEL_PACKAGE_NAME   ${CPACK_PACKAGE_NAME}-devel)
      set(CPACK_DEBIAN_DEBUG_PACKAGE_NAME   ${CPACK_PACKAGE_NAME}-debug)

   endif()
endif()
