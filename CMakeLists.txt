################################################################################
## File      : CMakeLists.txt
## Contents  : Portable socket library main CMakeLists
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(libSocket VERSION 0.0.0.0 LANGUAGES CXX)


# ==============================================================================
# == Setup Modules Path, default cmake flags & vars
# ==============================================================================

include(${PROJECT_SOURCE_DIR}/cmake/cmake_flags.cmake)

if (PROJECT_IS_TOP_LEVEL)
   include(${PROJECT_SOURCE_DIR}/cmake/cpack_.cmake)
endif()


# ==============================================================================
# == Options
# ==============================================================================

option(USE_TWEAK_AS_DATE                "Use project Tweak number as BCD date"          OFF)
option(USE_TWEAK                        "Show project Tweak number in version fonction" OFF)
option(INSTALL_HEADER_ONLY              "Install only header files"         OFF)
option(ENABLE_TEST_${PROJECT_NAME_UUC}  "Enable Unit tests"                 OFF)
option(ENABLE_DOC_${PROJECT_NAME_UUC}   "Generate Doxygen documentation"    OFF)

set(IGMP_REQ_ARRAY_SIZE 10 CACHE STRING "Igmp request array size")
if (MSVC)
   set(WSABUFF_ARRAY_SIZE 10 CACHE STRING "Scatter/Gather array size")
endif()

# ==============================================================================
# == Add Sources
# ==============================================================================

add_subdirectory(src/${PROJECT_NAME})


# ==============================================================================
# == Optional targets
# ==============================================================================

if (ENABLE_TEST_${PROJECT_NAME_UUC})
   enable_testing()
   add_subdirectory(tests)
endif()

if (ENABLE_DOC_${PROJECT_NAME_UUC})
   add_subdirectory(docs)
endif()
