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

message(CHECK_START "Finding Doxygen")
find_package(Doxygen REQUIRED dot)

if (NOT DOXYGEN_FOUND)
   message(CHECK_FAIL "not found. Unable to generate documentation.")

else()
   
   message(CHECK_PASS  "found")
   set(DOXYGEN_PROJECT_NAME      ${PROJECT_NAME})
      set(DOXYGEN_PROJECT_VERSION   ${PROJECT_VERSION})
      set(DOXYGEN_OUTPUT_DIR)
      set(DOXYGEN_INPUT_DIR 
         "${CMAKE_CURRENT_SOURCE_DIR} \
         ${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}"
      )
      set(DOXYGEN_FRONT_PAGE        front_page.md)

      configure_file(
         ${PROJECT_SOURCE_DIR}/cmake/doxyfile.in
         ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile @ONLY
      )

      add_custom_target(${PROJECT_NAME}-doxygen ALL
         COMMAND
            ${DOXYGEN_EXECUTABLE}  ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile
         WORKING_DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}
         COMMENT
            "Building HTML documentation with Doxygen"
      )
 
endif()
