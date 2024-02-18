////////////////////////////////////////////////////////////////////////////////
// File      : platform.h
// Content   : define OS_UNIX or OS_WINDOWS
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(__unix__)   || \
    defined(__unix)     || \
    defined(__linux__)  || \
    defined(__CYGWIN__) || \
    (defined(__APPLE__) && defined(__MACH__))

#   define OS_UNIX
#   undef  OS_WINDOWS

#elif defined(_WIN32) || defined(WIN32)

#   undef OS_UNIX
#   define OS_WINDOWS

#else

#   error platform not supported

#endif