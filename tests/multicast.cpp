////////////////////////////////////////////////////////////////////////////////
// File      : multicast.cpp
// Contents  : multicast test application
//
// Author(s) : Frederic Gerard - mailfge@gmail.com
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

////////////////////////////////////////////////////////////////////////////////
int multicastSender(const std::string &group, uint16_t port)
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

      constexpr int len = 1500;
      char buf[len];
      int n = 0;
      do
      {
         snprintf(buf, len, "Hello from %s count %d", group.c_str(), n++);
         if (socket.send(buf, (int)strlen(buf) + 1) == -1)
         {
            std::cout << "send failed\n";
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
int multicastReceiver(const std::string &IfIndex, const std::string &group, uint16_t port)
{
   try
   {
      auto socket = SocketDGRAM(AF_INET);

      int rc = socket.setAnyAddr(port);
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

      rc = socket.bind();
      if (rc == -1)
      {
         std::cout << "bind failed " << socket.error() << std::endl;
         return 1;
      }

      auto index = std::atoi(IfIndex.c_str());
      rc = socket.igmpJoin(group, index);
      if (rc == -1)
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
   std::cout << "  multicast --send GroupAddr Port\n",
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

   if (argc >= 3)
   {
      if (strcmp(argv[1], "--send") == 0)
      {
         return multicastSender(argv[2], std::atoi(argv[3]));
      }

      if ((strcmp(argv[1], "--recv") == 0) && argc >= 4)
      {

         return multicastReceiver(argv[4], argv[2], std::atoi(argv[3]));
      }

      usage();
      return 1;
   }
}