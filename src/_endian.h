////////////////////////////////////////////////////////////////////////////////
// File      : endian.h
// Content   : choose the right include for endianness
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(__linux__) || defined(__CYGWIN__)

#   include <endian.h>
#   define htonll(x) htobe64(x)
#   define ntohll(x) be64toh(x)

#elif defined(__APPLE__)


#elif defined(__OpenBSD__)

#   include <sys/endian.h>

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)

#   include <sys/endian.h>

#elif defined(_WIN32) || defined(WIN32)

#   include <winsock2.h>

#else
#   error platform not supported
#endif
