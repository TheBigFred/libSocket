################################################################################
## File      : cpack_.cmake
## Contents  : CPack configuration
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

set(CPACK_PACKAGE_NAME   "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "TheBigFred")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A low level portable socket library")
set(CPACK_RESOURCE_FILE_LICENSE       "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README        "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGING_INSTALL_PREFIX    "")
set(CPACK_OUTPUT_FILE_PREFIX          ${CMAKE_BINARY_DIR}/package)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_BINARY_DIR};/.git/;.gitignore;.svn")

set(CPACK_COMPONENTS_ALL 
   ${PROJECT_NAME}_runtime
   ${PROJECT_NAME}_library
   ${PROJECT_NAME}_archive
   ${PROJECT_NAME}_headers
   ${PROJECT_NAME}_cmake
   ${PROJECT_NAME}_debug
   ${PROJECT_NAME}_doc
)

set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)

set(CPACK_COMPONENT_${PROJECT_NAME_UC}_RUNTIME_GROUP  RUNTIME)
set(CPACK_COMPONENT_${PROJECT_NAME_UC}_LIBRARY_GROUP  RUNTIME)
set(CPACK_COMPONENT_${PROJECT_NAME_UC}_ARCHIVE_GROUP  DEVEL)
set(CPACK_COMPONENT_${PROJECT_NAME_UC}_HEADERS_GROUP  DEVEL)
set(CPACK_COMPONENT_${PROJECT_NAME_UC}_CMAKE_GROUP    DEVEL)
set(CPACK_COMPONENT_${PROJECT_NAME_UC}_DOC_GROUP      DEVEL)
set(CPACK_COMPONENT_${PROJECT_NAME_UC}_DEBUG_GROUP    DEBUG)

include(cpack_archive)
#include(cpack_deb)
#include(cpack_rpm)
#include(cpack_nsis)

if(APPLE)
   list(APPEND CPACK_GENERATOR "Bundle")
   set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
   configure_file(${CMAKE_CURRENT_LIST_DIR}/Info.plist.in Info.plist @ONLY)
   set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
   set(CPACK_BUNDLE_ICON ${CMAKE_CURRENT_LIST_DIR}/coffee.icns)
endif()

include(CPack)
file(WRITE ${CPACK_OUTPUT_FILE_PREFIX}/version.txt ${PROJECT_VERSION})
