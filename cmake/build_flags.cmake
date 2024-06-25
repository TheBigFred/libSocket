################################################################################
## File      : build_flags.cmake
## Contents  : set_build_flags cmake function
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

include(GNUInstallDirs)

function(set_build_flags target_name)

   # CFLAGS & CXXFLAGS
   target_compile_options(${target_name}
      PRIVATE
         $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -fmessage-length=0 -Wl,-whole-archive -ggdb -Wno-missing-field-initializers>
         $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-O0>
         $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>>:-O3>
         $<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:GNU>>:-Os>
         $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:GNU>>:-O2>
   )

   if (BUILD_SHARED_LIBS)
      target_compile_options(${target_name}
         PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:>
            $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>: /MDd>
            $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>: /MD /O2>
            $<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:MSVC>>: /MD /O1>
            $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>: /MD /O2>
      )
   else()
      target_compile_options(${target_name}
         PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:>
            $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>: /MTd>
            $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>: /MT /O2>
            $<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:MSVC>>: /MT /O1>
            $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>: /MT /O2>
      )
   endif()

   set(lib_prefix lib)
   if (${PROJECT_NAME} MATCHES  "^lib")
      set(lib_prefix "")
   endif()

   set_target_properties(${target_name}
      PROPERTIES
         CXX_STANDARD                 11
         CXX_STANDARD_REQUIRED        YES
         POSITION_INDEPENDENT_CODE    YES
         WINDOWS_EXPORT_ALL_SYMBOLS   OFF
         PREFIX                       "${lib_prefix}"
         DEBUG_POSTFIX                "${DEBUG_POSTFIX}"

         ARCHIVE_OUTPUT_DIRECTORY     "${OUTPUT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}"
         LIBRARY_OUTPUT_DIRECTORY     "${OUTPUT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}"
         RUNTIME_OUTPUT_DIRECTORY     "${OUTPUT_DIRECTORY}/${CMAKE_INSTALL_BINDIR}"
   )

   if (BUILD_SHARED_LIBS)
      set_target_properties(${target_name}
         PROPERTIES
            CXX_VISIBILITY_PRESET      hidden
            VISIBILITY_INLINES_HIDDEN  YES
            SOVERSION                  ${PROJECT_VERSION_MAJOR}
            VERSION                    ${PROJECT_VERSION}
      )

   else()
      target_compile_definitions(${target_name}
         PRIVATE
         ${PROJECT_NAME_UUC}_STATIC_DEFINE
      )

   endif()

   # set runtime path (aka RPATH)
   if ((NOT MSVC) AND BUILD_SHARED_LIBS)

      file(RELATIVE_PATH _rel ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR} ${CMAKE_INSTALL_PREFIX})
      if (APPLE)
         set(_rpath "@loader_path/${_rel}")
      else()
         set(_rpath "\$ORIGIN/${_rel}")
      endif()
      file(TO_NATIVE_PATH "${_rpath}/${CMAKE_INSTALL_LIBDIR}" __rpath)

      set_target_properties(${target_name}
         PROPERTIES
            MACOSX_RPATH                  ON
            SKIP_BUILD_RPATH              OFF
            BUILD_WITH_INSTALL_RPATH      OFF
            INSTALL_RPATH                 "${__rpath}"
            INSTALL_RPATH_USE_LINK_PATH   ON
      )
   endif()

   # -D
   target_compile_definitions(${target_name}
      PUBLIC
         $<$<CONFIG:Debug>:_DEBUG=1>
         $<$<NOT:$<CONFIG:DEBUG>>:NDEBUG=1>
   )

   # -I
   target_include_directories(${target_name}
      PRIVATE
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}>
      PUBLIC
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
         $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
         $<INSTALL_INTERFACE:include>
   )

endfunction()
