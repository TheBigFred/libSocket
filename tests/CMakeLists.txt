################################################################################
## File      : CMakeLists.txt
## Contents  : tests CMakeLists
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

# ==============================================================================
# == Get & configure gtest, libconsole
# ==============================================================================

if (MSVC AND BUILD_SHARED_LIBS)
   set(gtest_force_shared_crt ON CACHE BOOL "Prevent overriding the parent project's compiler/linker settings" FORCE)
endif ()

set(INSTALL_GTEST OFF)

if (VCPKG_BUILD)

   find_package(GTest CONFIG REQUIRED)

else (VCPKG_BUILD)
   
   include(FetchContent)
   FetchContent_Declare(
      googletest
         GIT_REPOSITORY https://github.com/google/googletest.git
         GIT_TAG        v1.14.0                                   # last release to support C++11: release-1.12.1
   )
   FetchContent_MakeAvailable(googletest)

endif(VCPKG_BUILD)


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
   NAME ${PROJECT_TESTS}
   COMMAND $<TARGET_FILE:${PROJECT_TESTS}>
)

set_build_flags(${PROJECT_TESTS})

target_include_directories(${PROJECT_TESTS}
   PRIVATE
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}>
)

target_link_libraries(${PROJECT_TESTS}
   PRIVATE
      ${PROJECT_NAME}
      GTest::gtest
)

if (MSVC AND BUILD_SHARED_LIBS)
   add_custom_command (TARGET ${PROJECT_TESTS} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_BINARY_DIR}/bin ${OUTPUT_DIRECTORY}/bin
   )

   if (EXISTS ${CMAKE_INSTALL_PREFIX}/bin)
      add_custom_command (TARGET ${PROJECT_TESTS} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_INSTALL_PREFIX}/bin ${OUTPUT_DIRECTORY}/bin/$<CONFIG>
      )
   endif()
endif()


# ==============================================================================
# == binaries loop
# ==============================================================================

list(APPEND binaries
   multicast
   broadcast
)

if (NOT MSVC)
   list(APPEND binaries
      udp
      raw
   )
endif()

foreach(item ${binaries})
   
   add_executable(${item}
      ${item}.cpp
   )
   
   set_build_flags(${item})

   target_include_directories(${item}
      PRIVATE
         $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}>
   )

   target_link_libraries(${item}
      PRIVATE
         ${PROJECT_NAME}
   )

endforeach(item)

