################################################################################
## File      : CMakeLists.txt
## Contents  : googletest downloader
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

cmake_minimum_required(VERSION 3.0.2)

project(googletest-download NONE)

include(ExternalProject)

ExternalProject_Add( googletest
   GIT_REPOSITORY https://github.com/google/googletest.git
   GIT_TAG main
   SOURCE_DIR "${CMAKE_BINARY_DIR}/googletest-src"
   BINARY_DIR "${CMAKE_BINARY_DIR}/googletest-build"
   CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER:FILEPATH="${CMAKE_C_COMPILER}"
                    -DCMAKE_CXX_COMPILER:FILEPATH="${CMAKE_CXX_COMPILER}"
                    -DINSTALL_GTEST:BOOL=False
   INSTALL_COMMAND  ""
   LOG_DOWNLOAD     ON
   LOG_CONFIGURE    ON
   LOG_BUILD        ON
)
