////////////////////////////////////////////////////////////////////////////////
// File      : socket_addr.h
// Contents  : sockaddr helpers
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <iomanip>
#include <iostream>

#include "socket_portability.h"
#include <libSocket/export.h>

/// A all in one sockaddr with size information.
struct socketaddr {
   union {
      struct sockaddr         sa;
      struct sockaddr_in      s4;
      struct sockaddr_in6     s6;
      struct sockaddr_storage ss;      
#ifdef OS_UNIX      
      struct sockaddr_ll      ll;
#endif
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
std::string LIBSOCKET_EXPORT IfName(int IfIndex);
std::string LIBSOCKET_EXPORT IpAddr(const std::string& ifName, int domain=AF_INET);
int         LIBSOCKET_EXPORT IfIndex(const std::string& IfName);
socketaddr  LIBSOCKET_EXPORT MacAddr_fromIfName(const std::string& IfName);
socketaddr  LIBSOCKET_EXPORT MacAddr_fromString(const std::string& IfName);


////////////////////////////////////////////////////////////////////////////////
template<typename T>
struct omanip {
   std::ostream& (*impl) (std::ostream&, T);
   T t;

   omanip(std::ostream& (*f)(std::ostream&, T), T arg) : impl(f), t(arg) {}

   void operator()(std::ostream& s) const { impl(s, t); }
   friend std::ostream& operator<<(std::ostream& s, const omanip& f) { f(s); return s; }
};

inline std::ostream& _macAddr(std::ostream& os, const socketaddr& addr)
{
   os << std::hex << std::setfill('0') << std::setw(2);
   for (int i=0; i<5; i++)
      os << (int)(addr.sa.sa_data[i]&0xff) << ':';
   os << (int)(addr.sa.sa_data[5]&0xff);
   os << std::dec;
   return os;
}

inline std::ostream& _macAddr(std::ostream& os, unsigned char* addr)
{
   if (addr != nullptr)
   {
      os << std::hex << std::setfill('0') << std::setw(2);
      for (int i=0; i<5; i++)
         os << (int)(addr[i]&0xff) << ':';
      os << (int)(addr[5]&0xff);
      os << std::dec;
   }
   return os;
}

inline omanip<const socketaddr&> macAddr(const socketaddr& addr) { return omanip<const socketaddr&>(_macAddr, addr); }
inline omanip<unsigned char*>    macAddr(unsigned char* addr)    { return omanip<unsigned char*>(_macAddr, addr); }
