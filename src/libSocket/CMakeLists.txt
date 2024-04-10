################################################################################
## File      : CMakeLists.txt
## Content   : src CMakeLists
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

# ==============================================================================
# == Define the project target
# ==============================================================================

add_library(${PROJECT_NAME})


# ==============================================================================
# == src folder files
# ==============================================================================

list(APPEND PRI_INC_FILES
)

list(APPEND PUB_INC_FILES
   ${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}/config.h
   ${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}/export.h
   ${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}/version.h
   _endian.h
   platform.h
   poll.h
   socket.h
   socket_addr.h
   socket_portability.h
   socketdgram.h
   socketstream.h
)

target_sources(${PROJECT_NAME}
   PRIVATE
      ${PRI_INC_FILES}
      ${PUB_INC_FILES}
      ${PROJECT_BINARY_DIR}/version.cpp   
      socket.cpp
      socket_addr.cpp
      socketdgram.cpp
      socketstream.cpp
)


# ==============================================================================
# == Include Sub Folders
# ==============================================================================

list(APPEND priSubFolders
)

list(APPEND pubSubFolders
)

foreach(item ${priSubFolders} ${pubSubFolders})
   add_subdirectory(${item})
endforeach(item)


# ==============================================================================
# == Build flags
# ==============================================================================

include(${PROJECT_SOURCE_DIR}/cmake/build_flags.cmake)
set_build_flags(${PROJECT_NAME})


# ==============================================================================
# == Generate files
# ==============================================================================

include(${PROJECT_SOURCE_DIR}/cmake/gen_files.cmake)


# ==============================================================================
# == Linker
# ==============================================================================

include(${PROJECT_BINARY_DIR}/cmake/dependencies.cmake)
if(PROJECT_IS_TOP_LEVEL)

  foreach(dep ${${COMP_UUC}_DEP_LIST})
    find_package(${dep} REQUIRED)
  endforeach()

   target_link_libraries(${PROJECT_NAME}
      PRIVATE
         ${${COMP_UUC}_TARGET_LIST}
   )

else()
   target_link_libraries(${PROJECT_NAME}
      PRIVATE
         ${${COMP_UUC}_STRIPED_TARGET_LIST}
   )

endif()


# ==============================================================================
# == Strip symboles
# ==============================================================================

include(${PROJECT_SOURCE_DIR}/cmake/strip_symboles.cmake)
strip_symboles(${PROJECT_NAME})


# ==============================================================================
# == Install Directives
# ==============================================================================

include(${PROJECT_SOURCE_DIR}/cmake/install.cmake)

