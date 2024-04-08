################################################################################
## File      : cmake_flags.cmake
## Contents  : setup VCPKG_BUILD, the build type
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LC)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)
string(REPLACE "-" "_" PROJECT_NAME_U   ${PROJECT_NAME})
string(REPLACE "-" "_" PROJECT_NAME_ULC ${PROJECT_NAME_LC})
string(REPLACE "-" "_" PROJECT_NAME_UUC ${PROJECT_NAME_UC})


# == Output build path
set(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output" CACHE PATH "build output path" FORCE)


# == target properties
set(DEBUG_POSTFIX "")


# == VCPKG or classic build
if(${CMAKE_TOOLCHAIN_FILE} MATCHES "vcpkg.cmake$")
  set(VCPKG_BUILD ON  CACHE BOOL "" FORCE)
  message(STATUS "VCPKG_BUILD is ${VCPKG_BUILD}")
endif()

# == Default Build Type
if (NOT CMAKE_BUILD_TYPE AND NOT GENERATOR_IS_MULTI_CONFIG)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE forced to ${CMAKE_BUILD_TYPE}")
else()
  if (CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
  endif()
  if (CMAKE_CONFIGURATION_TYPES)
    message(STATUS "CMAKE_CONFIGURATION_TYPES are ${CMAKE_CONFIGURATION_TYPES} ")
  endif()
endif()

if (BUILD_SHARED_LIBS)
  message(STATUS "BUILD_SHARED_LIBS is ${BUILD_SHARED_LIBS}")
else()
  message(STATUS "BUILD_SHARED_LIBS is not defined")
endif()
