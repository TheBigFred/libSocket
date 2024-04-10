################################################################################
## File      : install.cmake
## Contents  : CMakeLists.txt install section
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

include(GNUInstallDirs)

if(NOT INSTALL_HEADER_ONLY)

   # == bin .so .a
   install(
      TARGETS
         ${PROJECT_NAME}
      EXPORT
         ${PROJECT_NAME}
      RUNTIME
         DESTINATION    ${CMAKE_INSTALL_BINDIR}
         COMPONENT      ${PROJECT_NAME}_runtime
      LIBRARY
         DESTINATION    ${CMAKE_INSTALL_LIBDIR}
         COMPONENT      ${PROJECT_NAME}_library
      ARCHIVE
         DESTINATION    ${CMAKE_INSTALL_LIBDIR}
         COMPONENT      ${PROJECT_NAME}_archive
   )

   if (NOT MSVC AND BUILD_SHARED_LIBS)

      install(
         FILES
            ${SYMBOLES}
         DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
         COMPONENT
            ${PROJECT_NAME}_debug
      )

   endif()

   if (MSVC AND BUILD_SHARED_LIBS)

      set(PDBFILENAME $<IF:$<CONFIG:Debug>,${OUTPUT_DIRECTORY}/${CMAKE_INSTALL_BINDIR}/Debug/${PROJECT_NAME}${DEBUG_POSTFIX}.pdb,${OUTPUT_DIRECTORY}/${CMAKE_INSTALL_BINDIR}/Release/${PROJECT_NAME}.pdb>)
      install(
         FILES
            ${PDBFILENAME}
         DESTINATION
            ${CMAKE_INSTALL_BINDIR}
         CONFIGURATIONS
            Debug
         COMPONENT
            ${PROJECT_NAME}_debug
       )

   endif()

   # == cmake
   install(
      EXPORT
         ${PROJECT_NAME}
      FILE
         ${PROJECT_NAME_LC}.cmake
      DESTINATION
         ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LC}
      COMPONENT
         ${PROJECT_NAME}_cmake
   )

   install(
      FILES
         ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LC}Config.cmake
         ${PROJECT_BINARY_DIR}/${PROJECT_NAME_LC}ConfigVersion.cmake
      DESTINATION
         ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LC}
      COMPONENT
         ${PROJECT_NAME}_cmake
   )

   install(
      FILES
         ${PROJECT_BINARY_DIR}/cmake/dependencies.cmake
      DESTINATION
         ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LC}
      RENAME
         ${PROJECT_NAME_LC}_dependencies.cmake
      COMPONENT
         ${PROJECT_NAME}_cmake
   )
   
   # == doc
   if (ENABLE_DOC_${PROJECT_NAME})
      install(
         DIRECTORY
            ${PROJECT_SOURCE_DIR}/docs/html
         DESTINATION
            ${CMAKE_INSTALL_DOCDIR}
         COMPONENT
            ${PROJECT_NAME}_doc
      )
   endif()

endif()

# == include
install(
   FILES
      ${PUB_INC_FILES}
   DESTINATION
      ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
   COMPONENT
      ${PROJECT_NAME}_headers
)

install(
   DIRECTORY
      ${pubSubFolders}
   DESTINATION
      ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
   COMPONENT
      ${PROJECT_NAME}_headers
   FILES_MATCHING PATTERN
      "*.h"
)

