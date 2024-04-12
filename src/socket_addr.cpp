////////////////////////////////////////////////////////////////////////////////
// File      : socket_addr.cpp
// Contents  : sockaddr helpers
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <regex>
#include <cstring>
#include <string>
#include <system_error>

#include "platform.h"
#include "socket_addr.h"
#include "socketdgram.h"

#ifdef OS_UNIX
#   include <sys/types.h>
#   include <sys/ioctl.h>
#   include <ifaddrs.h>
#endif

#ifdef OS_WINDOWS
#   include <iphlpapi.h>
#   pragma comment(lib, "IPHLPAPI.lib")
#endif

/**
 * @brief Detect, with regex, the type of an IP address.
 * 
 * @param ipAddr : An valid IPV4 or IPV6 address.
 * @return int : AF_INET, AF_INET6 or AF_UNSPEC if ipAddr does not match IPV4 and IPV6.
 */
int IpAddrDomain(const std::string& ipAddr)
{
   int domain = AF_UNSPEC;
   const std::regex ipv4_regex("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
   const std::regex ipv6_regex("^(?:[A-F0-9]{1,4}:){7}[A-F0-9]{1,4}$");

   if (std::regex_match(ipAddr, ipv4_regex))
      domain = AF_INET;

   else if (std::regex_match(ipAddr, ipv6_regex))
      domain = AF_INET6;

   return domain;
}

/**
 * @brief Try to convert the node to a valid sockaddr.
 *
 * This methode encapsulate getaddrinfo.
 *
 * @param node : An IPV4, IPV6 or a hostname.
 * @param domain : AF_UNSPEC, AF_INET, AF_INET6.
 * @return socketaddr : If the convertion failed, the socketaddr.size equals -1.
 */
socketaddr SockAddr(const std::string& node, int domain/*=AF_UNSPEC*/)
{
   addrinfo hints = {};
   hints.ai_family = (domain == AF_UNSPEC) ? IpAddrDomain(node) : domain;
   hints.ai_flags = AI_PASSIVE;
   hints.ai_canonname = nullptr;
   hints.ai_addr = nullptr;
   hints.ai_next = nullptr;

   return SockAddr(node, 0, hints);
}

/**
 * @brief Try to convert the tuple node, port to a valid sockaddr.
 *
 * This methode encapsulate getaddrinfo.
 *
 * @param node : An IPV4, IPV6 or a hostname.
 * @param port : A valid port number.
 * @param domain : AF_UNSPEC, AF_INET, AF_INET6.
 * @return socketaddr : If the convertion failed, the socketaddr.size equals -1.
 */
socketaddr SockAddr(const std::string& node, uint16_t port, int domain/*=AF_UNSPEC*/)
{
   addrinfo hints = {};
   hints.ai_family = (domain == AF_UNSPEC) ? IpAddrDomain(node) : domain;
   hints.ai_flags = AI_PASSIVE;
   hints.ai_canonname = nullptr;
   hints.ai_addr = nullptr;
   hints.ai_next = nullptr;

   return SockAddr(node, port, hints);
}

/**
 * @brief Try to convert the tuple node, port to a valid sockaddr.
 * 
 *  This methode encapsulate getaddrinfo.
 *
 * @param node : An IPV4, IPV6 or a hostname.
 * @param port : A valid port number.
 * @param hints : A valid addrinfo passed to the underlying getaddrinfo.
 * @return socketaddr : If the convertion failed, the socketaddr.size equals -1.
 */
socketaddr SockAddr(const std::string& node, uint16_t port, addrinfo& hints)
{
   char* pnode = nullptr;
   if (node != "")
      pnode = (char*)node.c_str();

   char* pport = nullptr;
   std::string sport = std::to_string(port);
   if (port != 0)
      pport = (char*)sport.c_str();

   socketaddr saddr = {};
   addrinfo* result = nullptr, * rp = nullptr;
   int rc = getaddrinfo(pnode, pport, &hints, &result);
   if (rc != 0)
   {
      freeaddrinfo(result);
      return saddr;
   }

   for (rp = result; rp != nullptr; rp = rp->ai_next)
   {
      if (rp->ai_addr)
      {
         if (rp->ai_addr->sa_family == AF_INET || rp->ai_addr->sa_family == AF_INET6)
         {
            saddr.sa = *rp->ai_addr;
            saddr.size  = static_cast<socklen_t>(rp->ai_addrlen);
            freeaddrinfo(result);
            return saddr;
         }
      }
   }
   freeaddrinfo(result);
   saddr.size = 0;
   return saddr;
}

/**
 * @brief Convert an IP address to associated interface name.
 * 
 * @param ipAddr : The ip address of an interface.
 * @return std::string : The interface name or an empty string.
 */
std::string IfName(const std::string& ipAddr)
{
   std::string     ifName;
#ifdef OS_UNIX
   char            buf[NI_MAXHOST];
   struct ifaddrs* addrs, * iap;

   getifaddrs(&addrs);
   for (iap = addrs; iap != nullptr; iap = iap->ifa_next)
   {
      if (iap->ifa_addr && (iap->ifa_flags & IFF_UP) &&
         (iap->ifa_addr->sa_family == AF_INET || iap->ifa_addr->sa_family == AF_INET6))
      {
         auto family = iap->ifa_addr->sa_family;
         auto size = (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);

         if (getnameinfo(iap->ifa_addr, size, buf, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0)
         {
            if (!strcmp(ipAddr.c_str(), buf))
            {
               //interface found
               ifName.append(iap->ifa_name);
               freeifaddrs(addrs);
               return ifName;
            }
         }
      }
   }
   freeifaddrs(addrs);

#elif defined OS_WINDOWS

   ULONG rc = NO_ERROR;
   ULONG buffSize = 15000;
   UINT it = 1;
   auto pAddresses = (PIP_ADAPTER_ADDRESSES)HeapAlloc(GetProcessHeap(), 0, buffSize);
   if (pAddresses == nullptr)
      throw std::bad_alloc();
   do
   {
      rc = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, pAddresses, &buffSize);
      if (rc == ERROR_BUFFER_OVERFLOW)
      {
         HeapFree(GetProcessHeap(), 0, pAddresses);
         it++;
         buffSize *= it;
         pAddresses = (PIP_ADAPTER_ADDRESSES)HeapAlloc(GetProcessHeap(), 0, buffSize);
         if (pAddresses == nullptr)
            throw std::bad_alloc();
      }
   } while (rc == ERROR_BUFFER_OVERFLOW && it <3);

   if (rc == NO_ERROR)
   {
      auto sa = SockAddr(ipAddr.c_str());
      auto pCurrAddresses = pAddresses;

      while (pCurrAddresses) {
         auto pUnicast = pCurrAddresses->FirstUnicastAddress;
         while (pUnicast)
         {
            if (memcmp(pUnicast->Address.lpSockaddr, &sa, pUnicast->Address.iSockaddrLength) == 0)
            {
               auto ifName = std::string(pCurrAddresses->AdapterName);
               HeapFree(GetProcessHeap(), 0, pAddresses);
               return ifName;
            }
            pUnicast = pUnicast->Next;
         }
         pCurrAddresses = pCurrAddresses->Next;
      }
   }

   HeapFree(GetProcessHeap(), 0, pAddresses);
#endif
   return ifName;
}

/**
 * @brief Convert an interface index into a interface name.
 * 
 * @param ifIndex : The index of an interface.
 * @return std::string : The interface name or an empty string.
 */
std::string IfName(int ifIndex)
{
   std::string ifName;
#ifdef OS_UNIX
   char buff[IF_NAMESIZE] = {};
   if ( if_indextoname(ifIndex , buff) != nullptr)
      ifName.append(buff);
#elif defined OS_WINDOWS
  // ToDo : https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-if_indextoname
#endif
   return ifName;
}

/**
 * @brief Convert an interface name to an ip address.
 * 
 * @param ifName : The name of an interface.
 * @param domain : AF_INET, AF_INT6.
 * @return std::string : The ip address of the specified interface or an empty string.
 */
std::string IpAddr(const std::string& ifName, int domain/*=AF_INET*/)
{
   std::string     ipaddr;
   char            buf[32];

#ifdef OS_UNIX   
   struct ifaddrs* addrs, * iap;

   getifaddrs(&addrs);
   for (iap = addrs; iap != nullptr; iap = iap->ifa_next)
   {
      if (iap->ifa_addr && (iap->ifa_flags & IFF_UP) &&
         (iap->ifa_addr->sa_family == AF_INET || iap->ifa_addr->sa_family == AF_INET6))
      {
         if (domain == AF_UNSPEC || domain == iap->ifa_addr->sa_family)
         {
            if (!strcmp(ifName.c_str(), iap->ifa_name))
            {
               //interface found
               auto family = iap->ifa_addr->sa_family;
               if (family == AF_INET) {
                  struct in_addr sa = ((struct sockaddr_in*)iap->ifa_addr)->sin_addr;
                  inet_ntop(family, &sa, buf, sizeof(buf));

               }
               else
               {
                  auto sa = ((struct sockaddr_in6*)iap->ifa_addr)->sin6_addr;
                  inet_ntop(family, &sa, buf, sizeof(buf));
               }
               ipaddr.append(buf);
               freeifaddrs(addrs);
               return ipaddr;
            }
         }
      }
   }
   freeifaddrs(addrs);

#elif defined OS_WINDOWS
  
   ULONG rc = NO_ERROR;
   ULONG buffSize = 15000;
   UINT it = 1;
   auto pAddresses = (PIP_ADAPTER_ADDRESSES)HeapAlloc(GetProcessHeap(), 0, buffSize);
   if (pAddresses == nullptr)
      throw std::bad_alloc();
   do
   {
      rc = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, pAddresses, &buffSize);
      if (rc == ERROR_BUFFER_OVERFLOW)
      {
         HeapFree(GetProcessHeap(), 0, pAddresses);
         it++;
         buffSize *= it;
         pAddresses = (PIP_ADAPTER_ADDRESSES)HeapAlloc(GetProcessHeap(), 0, buffSize);
         if (pAddresses == nullptr)
            throw std::bad_alloc();
      }
   } while (rc == ERROR_BUFFER_OVERFLOW && it < 3);

   if (rc == NO_ERROR)
   {
      auto pCurrAddresses = pAddresses;

      while (pCurrAddresses) {
         if (!strcmp(pCurrAddresses->AdapterName, ifName.c_str()))
         {
            auto pUnicast = pCurrAddresses->FirstUnicastAddress;
            while (pUnicast)
            {
               auto family = pUnicast->Address.lpSockaddr->sa_family;
               if (family == AF_INET)
               {
                  struct in_addr sa = ((struct sockaddr_in*)&pUnicast->Address.lpSockaddr->sa_data)->sin_addr;
                  inet_ntop(family, &sa, buf, sizeof(buf));
                  ipaddr.append(buf);
                  HeapFree(GetProcessHeap(), 0, pAddresses);
                  return ipaddr;
               }
               else
               {
                  auto sa = ((struct sockaddr_in6*)&pUnicast->Address.lpSockaddr->sa_data)->sin6_addr;
                  inet_ntop(family, &sa, buf, sizeof(buf));
                  ipaddr.append(buf);
                  HeapFree(GetProcessHeap(), 0, pAddresses);
                  return ipaddr;
               }
               pUnicast = pUnicast->Next;
            }
         }
         pCurrAddresses = pCurrAddresses->Next;
      }
   }

   HeapFree(GetProcessHeap(), 0, pAddresses);

#endif
   return ipaddr;
}

/**
 * @brief Convert an interface name into an interface index.
 * 
 * @param IfName : A valid interface name.
 * @return int : The interface index or -1 if not found.
 */
int IfIndex(const std::string& IfName)
{
#ifdef OS_UNIX

   auto rc = if_nametoindex(IfName.c_str());
   return (rc == 0) ? -1 : rc;

#elif defined OS_WINDOWS
  // ToDo try if_nametoindex : https://docs.microsoft.com/en-us/windows/win32/api/netioapi/nf-netioapi-if_nametoindex
   ULONG rc = NO_ERROR;
   ULONG buffSize = 15000;
   UINT it = 1;
   auto pAddresses = (PIP_ADAPTER_ADDRESSES)HeapAlloc(GetProcessHeap(), 0, buffSize);
   if (pAddresses == nullptr)
      throw std::bad_alloc();
   do
   {
      rc = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, pAddresses, &buffSize);
      if (rc == ERROR_BUFFER_OVERFLOW)
      {
         HeapFree(GetProcessHeap(), 0, pAddresses);
         it++;
         buffSize *= it;
         pAddresses = (PIP_ADAPTER_ADDRESSES)HeapAlloc(GetProcessHeap(), 0, buffSize);
         if (pAddresses == nullptr)
            throw std::bad_alloc();
      }
   } while (rc == ERROR_BUFFER_OVERFLOW && it < 3);

   if (rc == NO_ERROR)
   {
      auto pCurrAddresses = pAddresses;

      while (pCurrAddresses) {
         if (!strcmp(pCurrAddresses->AdapterName, IfName.c_str()))
         {
            int index = pCurrAddresses->IfIndex;
            HeapFree(GetProcessHeap(), 0, pAddresses);
            return index;
         }
         pCurrAddresses = pCurrAddresses->Next;
      }
   }

   HeapFree(GetProcessHeap(), 0, pAddresses);
#endif
   return -1;
}

socketaddr MacAddr_fromIfName(const std::string& IfName)
{
  socketaddr macAddr={};
#ifdef OS_UNIX

   ifreq request = {};
   strncpy(request.ifr_name,IfName.c_str(),IFNAMSIZ-1);

   auto rawSocket = SocketDGRAM(AF_PACKET,SOCK_RAW,IPPROTO_RAW);
   auto sockethandler = rawSocket.open();
   if (sockethandler  == INVALID_SOCKET)
     throw std::system_error(rawSocket.error(), std::system_category(), "Can not open raw socket");

   if((ioctl(sockethandler,SIOCGIFHWADDR,&request))<0)
     throw std::system_error(errno, std::system_category(), "Can not ioctl SIOCGIFHWADDR");

   macAddr.sa = request.ifr_hwaddr;
   macAddr.size = sizeof(macAddr.sa);

#elif defined OS_WINDOWS
   // ToDo GetAdaptersInfo https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersinfo
#endif

   return macAddr;
}

socketaddr MacAddr_fromString(const std::string& macStr)
{
   socketaddr macAddr = {};
   auto* d = macAddr.sa.sa_data;
   int rc = std::sscanf(macStr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
               (int*)&d[0],(int*)&d[1],(int*)&d[2],
               (int*)&d[3],(int*)&d[4],(int*)&d[5]);
   if (rc == 6)
      macAddr.size = sizeof(macAddr.sa);
   return macAddr;
}
