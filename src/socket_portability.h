////////////////////////////////////////////////////////////////////////////////
// File      : socker_portability.h
// Contents  : Things needed to make BSD sockets and the WinSoc2 API compatible
//
// Author(s) : Frederic Gerard - mailfge@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "platform.h"
#ifdef OS_UNIX

#   ifndef _GNU_SOURCE
#      define _GNU_SOURCE
#   endif
#   include <unistd.h>
#   include <fcntl.h>
#   include <sys/stat.h>

#   include <netdb.h>
#   include <net/if.h>
#   include <arpa/inet.h>
#   include <netinet/in.h>
#   include <sys/socket.h>
    using  SOCKET = int;

#   define INVALID_SOCKET -1
#   define SOCKET_ERROR   -1

#   define CPCHAR_WSCAST
#   define PCHAR_WSCAST
#   define INT_WSCAST
#   define UINT_WSCAST

#elif defined OS_WINDOWS

#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif

#   include <windows.h>
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   pragma comment(lib, "Ws2_32.lib")

#  include <io.h>
#  include <fcntl.h>
#  include <BaseTsd.h>
#  include <vcruntime.h>
   typedef SSIZE_T ssize_t;

#   pragma warning(disable : 4996)
#   define _CRT_SECURE_NO_WARNINGS

#   define CPCHAR_WSCAST( p ) reinterpret_cast<const char*>( (p) )
#   define PCHAR_WSCAST( p )  reinterpret_cast<char*>( (p) )
#   define INT_WSCAST( p )    static_cast<int>( (p) )
#   define UINT_WSCAST( p )   static_cast<unsigned int>( (p) )

    struct iovec
    {
      void* iov_base;	/* Pointer to data.  */
      uint32_t iov_len;	/* Length of data.  */
    };

    struct msghdr
    {
      void*         msg_name;		/* Address to send to/receive from.  */
      socklen_t     msg_namelen;	/* Length of address data.  */

      struct iovec* msg_iov;	   /* Vector of data to send/receive into.  */
      uint32_t      msg_iovlen;	/* Number of elements in the vector.  */

      void*         msg_control;		/* Ancillary data (eg BSD filedesc passing). */
      uint32_t      msg_controllen;	/* Ancillary data buffer length.*/
      int msg_flags;		/* Flags on received message.  */
    };

#   ifdef DEBUG
    struct ip_mreq
    {
       IN_ADDR imr_multiaddr;
       IN_ADDR imr_interface;
    };

    struct ip_mreq_source
    {
       IN_ADDR imr_multiaddr;
       IN_ADDR imr_sourceaddr;
       IN_ADDR imr_interface;
    };
#   endif

#endif
