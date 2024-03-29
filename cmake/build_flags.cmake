################################################################################
## File      : target_default_build_flags.cmake
## Contents  : default flags cmake function
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

function(${PROJECT_NAME}_build_flags target_name)

   # CFLAGS & CXXFLAGS
   target_compile_options(${target_name}
      PRIVATE
         $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -fmessage-length=0>
         $<$<CXX_COMPILER_ID:MSVC>:>
         $<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:GNU>>:-O0 -ggdb>
         $<$<AND:$<CONFIG:RELEASE>,$<CXX_COMPILER_ID:GNU>>:-O3 -ggdb>
   )

   # Tell we need to use C++11
   set_target_properties(${target_name}
      PROPERTIES 
         CXX_STANDARD            11
         CXX_STANDARD_REQUIRED   YES
   )

   # -D
   target_compile_definitions(${target_name}
      PUBLIC
         $<$<CONFIG:DEBUG>:DEBUG=1>
         $<$<CONFIG:RELEASE>:NDEBUG=1>
   )

   # -I
   target_include_directories(${target_name}
      PUBLIC
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
         $<INSTALL_INTERFACE:include>
   )

endfunction()
