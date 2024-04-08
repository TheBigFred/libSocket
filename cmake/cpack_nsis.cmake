################################################################################
## File      : cpack_nsis.cmake
## Contents  : MS Windows cmake packager
##
## Author    : TheBigFred - thebigfred.github@gmail.com
## URL       : https://github.com/TheBigFred/libSocket
##
##-----------------------------------------------------------------------------
##  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
##-----------------------------------------------------------------------------
################################################################################

if (WIN32)  
    find_program(MAKENSIS_FOUND makensis PATHS "C:/Program Files (x86)/NSIS")
    if (NOT MAKENSIS_FOUND)
        message(STATUS "makensis not found, you can get it here: https://nsis.sourceforge.io")
    else()

        list(APPEND CPACK_GENERATOR   "NSIS")
        set(CPACK_NSIS_PACKAGE_NAME   "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}")
        set(CPACK_NSIS_CONTACT        "TheBigFred - thebigfred.github@gmail.com")
        set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

    endif()
endif()
