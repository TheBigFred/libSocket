################################################################################
## File      : strip_symboles.cmake
## Contents  : strip_symboles cmake function
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

set(SYMBOLES )

function(strip_symboles target_name)
    
   if (NOT MSVC AND BUILD_SHARED_LIBS)

      set(prefix_path ${OUTPUT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR})
      if (NOT "${CMAKE_CONFIGURATION_TYPES}" STREQUAL "")
         set(prefix_path ${prefix_path}/$<CONFIG>/)
      else()
         set(prefix_path ${prefix_path}/)
      endif()

      set(file_postfix )
      get_target_property(target_type ${target_name} TYPE)
      if (target_type STREQUAL "SHARED_LIBRARY")
         set(file_postfix .so)
      endif()

      set(file_prefix lib)
      if (${PROJECT_NAME} MATCHES  "^lib")
         set(file_prefix "")
      endif()

      
      set(filename
         $<IF:$<CONFIG:Debug>,${prefix_path}${file_prefix}${target_name}${DEBUG_POSTFIX}${file_postfix},${prefix_path}${file_prefix}${target_name}${file_postfix}>
      )
      set(filename_d ${filename}.debug)

      add_custom_command(TARGET ${target_name} POST_BUILD
         COMMAND ${CMAKE_OBJCOPY} --only-keep-debug ${filename} ${filename_d}
         COMMAND ${CMAKE_OBJCOPY} --strip-debug ${filename}
         COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=${filename_d} ${filename}
      )

      set(SYMBOLES ${SYMBOLES} ${filename_d} PARENT_SCOPE)

   endif()

endfunction()
