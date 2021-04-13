################################################################################
## File      : CMakeLists.txt
## Contents  : tests CMakeLists
##
## Author(s) : Frederic Gerard - mailfge@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

include(../cmake/googletest.cmake)

# ==============================================================================
# == Define the project targets
# ==============================================================================

set(PROJECT_TESTS ${PROJECT_NAME}-tests)

add_executable(${PROJECT_TESTS}
   socketDGRAM.cpp
   socketSTREAM.cpp
   main.cpp
)

add_test(
   NAME ${PROJECT_NAME}
   COMMAND $<TARGET_FILE:${PROJECT_TESTS}>
)

# ==============================================================================
# == Build flags
# ==============================================================================

libSocket_build_flags(${PROJECT_TESTS})

target_include_directories(${PROJECT_TESTS}
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
)

# ==============================================================================
# == Linker
# ==============================================================================

target_link_libraries(${PROJECT_TESTS}
   PRIVATE
      ${LIBNAME_SHARED}
      gtest
)

# ==============================================================================
# == binaries loop
# ==============================================================================

list(APPEND binaries
   udp
   multicast
   broadcast
)

foreach(item ${binaries})
   
   add_executable(${item}
      ${item}.cpp
   )
   
   libSocket_build_flags(${item})

   target_include_directories(${item}
      PUBLIC
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
   )

   target_link_libraries(${item}
      PRIVATE ${LIBNAME_SHARED}
   )

endforeach(item)


# ==============================================================================
# == Generate an archive.tar for the SocketSTREAM send_recv_file unittest
# ==============================================================================

if (MSVC)
   set(EXTENSION ".exe")
endif()

add_custom_command (
   TARGET ${PROJECT_TESTS} POST_BUILD
   COMMAND tar -cf archive.tar ${PROJECT_TESTS}${EXTENSION}
   DEPENDS
   WORKING_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}
)
