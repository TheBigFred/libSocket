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

#   include <libkern/OSByteOrder.h>

#   define htobe16(x) OSSwapHostToBigInt16(x)
#   define htole16(x) OSSwapHostToLittleInt16(x)
#   define be16toh(x) OSSwapBigToHostInt16(x)
#   define le16toh(x) OSSwapLittleToHostInt16(x)
 
#   define htobe32(x) OSSwapHostToBigInt32(x)
#   define htole32(x) OSSwapHostToLittleInt32(x)
#   define be32toh(x) OSSwapBigToHostInt32(x)
#   define le32toh(x) OSSwapLittleToHostInt32(x)
 
#   define htobe64(x) OSSwapHostToBigInt64(x)
#   define htole64(x) OSSwapHostToLittleInt64(x)
#   define be64toh(x) OSSwapBigToHostInt64(x)
#   define le64toh(x) OSSwapLittleToHostInt64(x)

#elif defined(_WIN32) || defined(WIN32)

#   include <winsock2.h>
#   if BYTE_ORDER == LITTLE_ENDIAN

#      define htobe16(x) htons(x)
#      define htole16(x) (x)
#      define be16toh(x) ntohs(x)
#      define le16toh(x) (x)
 
#      define htobe32(x) htonl(x)
#      define htole32(x) (x)
#      define be32toh(x) ntohl(x)
#      define le32toh(x) (x)
 
#      define htobe64(x) htonll(x)
#      define htole64(x) (x)
#      define be64toh(x) ntohll(x)
#      define le64toh(x) (x)

#   elif BYTE_ORDER == BIG_ENDIAN
#       error platform not supported
#   endif

#else
#   error platform not supported
#endif
