////////////////////////////////////////////////////////////////////////////////
// File      : socket_addr.h
// Contents  : sockaddr helpers
//
// Author(s) : Frederic Gerard - mailfge@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "socket_portability.h"
#include "export.h"

/// A all in one sockaddr with size information.
struct socketaddr {
   union {
      struct sockaddr         sa;
      struct sockaddr_in      s4;
      struct sockaddr_in6     s6;
      struct sockaddr_storage ss;
   };
   socklen_t size;

   void LIBSOCKET_EXPORT setIPV4(const sockaddr_in& sa)  { s4 = sa, size=sizeof(sa); }
   void LIBSOCKET_EXPORT setIPV6(const sockaddr_in6& sa) { s6 = sa, size=sizeof(sa); }
};

int         LIBSOCKET_EXPORT IpAddrDomain(const std::string& ipAddr);
socketaddr  LIBSOCKET_EXPORT SockAddr(const std::string& node, int domain = AF_UNSPEC);
socketaddr  LIBSOCKET_EXPORT SockAddr(const std::string& node, uint16_t port, int domain=AF_UNSPEC);
socketaddr  LIBSOCKET_EXPORT SockAddr(const std::string& node, uint16_t port, addrinfo&);
std::string LIBSOCKET_EXPORT IfName(const std::string& ipAddr);
std::string LIBSOCKET_EXPORT IpAddr(const std::string& ifName, int domain=AF_INET);
int         LIBSOCKET_EXPORT IfIndex(const std::string& IfName);
