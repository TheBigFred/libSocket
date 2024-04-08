################################################################################
## File      : gen_files.cmake
## Contents  : generate files: version .h .cpp .cmake
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

configure_file(
   ${PROJECT_SOURCE_DIR}/cmake/dependencies.cmake.in
   ${PROJECT_BINARY_DIR}/cmake/dependencies.cmake @ONLY
)

configure_file(
   ${PROJECT_SOURCE_DIR}/cmake/version.h.in
   ${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}/version.h @ONLY
)
configure_file(
   ${PROJECT_SOURCE_DIR}/cmake/version.cpp.in
   ${PROJECT_BINARY_DIR}/version.cpp @ONLY
)

configure_file(
   ${PROJECT_SOURCE_DIR}/cmake/config.h.in
   ${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}/config.h
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
   EXPORT_FILE_NAME           "${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}/export.h"
   EXPORT_MACRO_NAME          EXPORT
   DEPRECATED_MACRO_NAME      DEPRECATED
   NO_EXPORT_MACRO_NAME       NO_EXPORT
   INCLUDE_GUARD_NAME         ${PROJECT_NAME_UUC}_EXPORT_H
   STATIC_DEFINE              STATICLIB_DEFINE
   NO_DEPRECATED_MACRO_NAME   NO_DEPRECATED
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
   ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LC}ConfigVersion.cmake
   VERSION ${PROJECT_VERSION}
   COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
   ${PROJECT_SOURCE_DIR}/cmake/config.cmake.in
   ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LC}Config.cmake
   INSTALL_DESTINATION cmake
)
