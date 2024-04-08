////////////////////////////////////////////////////////////////////////////////
// File      : multicast.cpp
// Contents  : multicast test application
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
//  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <thread>
#include <cerrno>
#include <cstring>
#include <iostream>
#include "socketdgram.h"

int setPattern(char* buff, int size, uint32_t pattern)
{
   if (buff == nullptr)
      return 0;

   uint32_t* ptr = (uint32_t *)buff;
   for (uint32_t i = 0; i < sizeof(buff) / sizeof(pattern); i++)
   {
      ptr[i] = pattern;
   }
   return size - (size%sizeof(pattern));
}

////////////////////////////////////////////////////////////////////////////////
int multicastSender(int ifIndex, const std::string &group, uint16_t port, int multicastLoop)
{
   auto socket = SocketDGRAM();

   try
   {
      int rc = socket.setAddr(group, port);
      if (rc == -1)
      {
         std::cout << "setAddr failed " << std::endl;
         return 1;
      }

      auto src = socket.open();
      if (src == INVALID_SOCKET)
      {
         std::cout << "open failed " << socket.error() << std::endl;
         return 1;
      }
      
      if (ifIndex >= 0)
      {
        auto ifaddr = SockAddr( IpAddr(IfName(ifIndex)) );
        if (socket.setOption(IPPROTO_IP, IP_MULTICAST_IF, (void*)&(ifaddr.s4.sin_addr), sizeof(ifaddr.s4.sin_addr)) == -1)
        {
          std::cout << "setOption IP_MULTICAST_IF failed " << socket.error() << std::endl;
          return 1;
        }
      }

      if (multicastLoop >= 0)
      {
        if (socket.setOption(IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&multicastLoop, sizeof(multicastLoop)) == -1)
        {
          std::cout << "setOption IP_MULTICAST_LOOP failed " << socket.error() << std::endl;
          return 1;
        }
      }

      if (ifIndex >= 0)
      {
        auto ifaddr = SockAddr( IpAddr(IfName(ifIndex)) );
        if (socket.setOption(IPPROTO_IP, IP_MULTICAST_IF, (void*)&(ifaddr.s4.sin_addr), sizeof(ifaddr.s4.sin_addr)) == -1)
        {
         std::cout << "setOption IP_MULTICAST_IF failed " << socket.error() << std::endl;
         return 1;
        }
      }

      if (socket.setOption(IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&multicastLoop, sizeof(multicastLoop)) == -1)
      {
         std::cout << "setOption IP_MULTICAST_LOOP failed " << socket.error() << std::endl;
         return 1;
      }

      char buf[1500];
      int len = setPattern(buf, sizeof(buf), 0x55AAAA55);
      int n = 0;
      do
      {
         snprintf(buf, len, "Hello from %s count %d", group.c_str(), n++);
         if (socket.send(buf, (int)strlen(buf) + 1) == -1)
         {
            std::cout << "send failed " << strerror(socket.error()) << std::endl;
            return 1;
         }
         std::cout << buf << std::endl;
         std::this_thread::sleep_for(std::chrono::seconds(1));
      } while (n < 100);
   }
   catch (const std::exception &exp)
   {
      std::cout << exp.what();
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
int multicastReceiver(int IfIndex, const std::string &group, uint16_t port)
{
   try
   {
      auto socket = SocketDGRAM(AF_INET);

#if defined(_WIN32) || defined(WIN32)
      int rc = socket.setAnyAddr(AF_INET,port);
#else
      int rc = socket.setAddr(group, port);
#endif
      if (rc == -1)
      {
         std::cout << "setAddr failed" << std::endl;
         return 1;
      }

      auto src = socket.open();
      if (src == INVALID_SOCKET)
      {
         std::cout << "open failed " << strerror(socket.error()) << std::endl;

         return 1;
      }

      if (socket.setReusePort() == -1)
      {
         std::cout << "setReusePort failed " << socket.error() << std::endl;
         return 1;
      }

      if (socket.bind() == -1)
      {
         std::cout << "bind failed " << socket.error() << std::endl;
         return 1;
      }

      if (socket.igmpJoin(group, IfIndex) == -1)
      {
         std::cout << "Igmp Join failed " << socket.error() << std::endl;
         return 1;
      }

      constexpr int len = 1500;
      char buff[len];
      int nbytes = 0;
      while ((nbytes = socket.recv(buff, len)) > 0)
      {
         buff[nbytes] = 0;
         std::cout << group << ":" << port << "  " << buff << std::endl;
      }
   }
   catch (const std::exception &exp)
   {
      std::cout << exp.what() << std::endl;
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
void usage()
{
   std::cout << "Usage\n";
   std::cout << "  multicast --send GroupAddr Port IfIndex [--noloop]\n",
   std::cout << "  multicast --recv GroupAddr Port IfIndex\n";
   std::cout << "      receive multicast on the designated interface\n\n";
}

int main(int argc, char **argv)
{
   if (argc < 2)
   {
      usage();
      return 1;
   }

   if (argc >= 5)
   {
      if (strcmp(argv[1], "--send") == 0)
      {
         int loop=1;
         if (argc >= 6) {
            if (strcmp(argv[5],"--noloop") == 0)
               loop=0;
         }
         auto index = std::atoi(argv[4]);
         return multicastSender(index, argv[2], std::atoi(argv[3]),loop);
      }

      if ((strcmp(argv[1], "--recv") == 0) && argc >= 5)
      {
         auto index = std::atoi(argv[4]);
         return multicastReceiver(index, argv[2], std::atoi(argv[3]));
      }

      usage();
      return 1;
   }
}
