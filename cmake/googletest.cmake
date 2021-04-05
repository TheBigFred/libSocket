################################################################################
## File      : CMakeLists.txt
## Contents  : googletest builder
##
## Author(s) : Frederic Gerard - mailfge@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

# ==============================================================================
# == Download and build googletest at configure time
# ==============================================================================

if(NOT EXISTS "${CMAKE_BINARY_DIR}/googletest-download")
   configure_file(
      ../cmake/googletest-download.cmake ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
endif()

if (NOT EXISTS "${CMAKE_BINARY_DIR}/googletest-build")
   execute_process(
      COMMAND           "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
   )

   execute_process(
      COMMAND           "${CMAKE_COMMAND}" --build .
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
   )
endif()

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Prevent installing GoogleTest
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

if(NOT TARGET gtest)
   # Add googletest directly to our build. This adds the following targets:
   # gtest, gtest_main, gmock and gmock_main
   add_subdirectory(
      "${CMAKE_BINARY_DIR}/googletest-src"
      "${CMAKE_BINARY_DIR}/googletest-build"
   )
endif()
