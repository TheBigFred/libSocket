################################################################################
## File      : CMakeLists.txt
## Contents  : Generate doxygen documentation
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

find_package(Perl QUIET)
find_package(Doxygen QUIET)

if (PERL_FOUND AND DOXYGEN_FOUND)

   set(DOXYGEN_PROJECT_NAME      ${PROJECT_NAME})
   set(DOXYGEN_PROJECT_VERSION   ${PROJECT_VERSION})
   set(DOXYGEN_OUTPUT_DIR        .)
   set(DOXYGEN_INPUT_DIR        "${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..")
   set(DOXYGEN_FRONT_PAGE        front_page.md)

   configure_file(
      ../cmake/Doxyfile.in
      ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile @ONLY
   )

   add_custom_target(${PROJECT_NAME}-doxygen
      COMMAND
         ${DOXYGEN_EXECUTABLE}  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
      WORKING_DIRECTORY
         ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT
         "Building HTML documentation with Doxygen"
   )
endif()
