################################################################################
## File      : CMakeLists.txt
## Contents  : Portable socket library main CMakeLists
##
## Author(s) : Frederic Gerard - mailfge@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(libSocket VERSION 0.0.0.0 LANGUAGES CXX)

set(LIBNAME Socket)
set(LIBNAME_STATIC ${LIBNAME}-static)
set(LIBNAME_SHARED ${LIBNAME}-shared)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LC)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

# ==============================================================================
# == Standalone build or Global CMakeLists
# ==============================================================================
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
   set(STANDALONE_${PROJECT_NAME} ON)
   message(STATUS "** Standalone build **")
endif()

# ==============================================================================
# == Import Modules
# ==============================================================================

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_flags.cmake)

if (STANDALONE_${PROJECT_NAME})
   include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeCPack.cmake)
endif()

# ==============================================================================
# == Default Build Type
# ==============================================================================

if (NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ==============================================================================
# == Options
# ==============================================================================

option(USE_TWEAK_AS_DATE            "Use project Tweak number as BCD date"  OFF)
option(USE_TWEAK                    "Show project Tweak number in version fonction" OFF)
option(ENABLE_TEST_${PROJECT_NAME}  "Enable Unit tests"                     OFF)
option(ENABLE_DOC_${PROJECT_NAME}   "Generate Doxygen documentation"        OFF)
option(DevelPackage                 "Generate developpment package"         OFF)

set(IGMP_REQ_ARRAY_SIZE 10 CACHE STRING "Igmp request array size")
if (MSVC)
   set(WSABUFF_ARRAY_SIZE 10 CACHE STRING "Scatter/Gather array size")
endif()

# ==============================================================================
# == Add Sources
# ==============================================================================

add_subdirectory(src)

if (ENABLE_TEST_${PROJECT_NAME})
   enable_testing()
   add_subdirectory(tests)
endif()

if (ENABLE_DOC_${PROJECT_NAME})
   add_subdirectory(docs)
endif()

# ==============================================================================
# == Create symlink from src to ${PROJET_NAME}
# ==============================================================================

if(NOT STANDALONE_${PROJECT_NAME})

   if (NOT EXISTS  ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME})

      execute_process (
         COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_CURRENT_LIST_DIR}/src
            ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}
      )
   endif()

   if (NOT EXISTS  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})

      execute_process (
         COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_CURRENT_BINARY_DIR}/src
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
      )
   endif()

endif()

# ==============================================================================
# == Remove generated docs & symlink
# ==============================================================================

add_custom_target(${PROJECT_NAME}-clean
   COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
   COMMAND ${CMAKE_COMMAND} -E remove_directory -f ${CMAKE_CURRENT_SOURCE_DIR}/docs/html
   COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}
   COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_SOURCE_DIR}/config.h
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

