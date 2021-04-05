////////////////////////////////////////////////////////////////////////////////
// File      : broadcast.cpp
// Contents  : broadcast test application
//
// Author(s) : Frederic Gerard - mailfge@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <thread>
#include <chrono>
#include <string>
#include <cstring>
#include <iostream>
#include "socketdgram.h"

////////////////////////////////////////////////////////////////////////////////
int broadcastSender(const std::string &IpAddr, uint16_t port)
{
   try
   {
      auto socket = SocketDGRAM();

      int rc = socket.setAddr(IpAddr, port);
      if (rc != 0)
      {
         std::cout << "setAddr failed " << socket.error() << std::endl;
         return 1;
      }

      auto src = socket.open();
      if (src == INVALID_SOCKET)
      {
         std::cout << "open failed " << socket.error() << std::endl;
         return 1;
      }

      rc = socket.enableBroadcast();
      if (rc != 0)
      {
         std::cout << "enableBroadcast failed " << socket.error() << std::endl;
         return 1;
      }

      std::string str("Hello wolrd!!!");
      int n = 0;

      do
      {
         auto data = str + std::to_string(n++);
         if (socket.send(data) == -1)
         {
            std::cout << "send failed " << socket.error() << std::endl;
            return 1;
         }

         std::cout << "send " << data << std::endl;
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
int broadcastReceiver(uint16_t port)
{
   try
   {
      auto socket = SocketDGRAM(AF_INET);

      int rc = socket.setAnyAddr(port);
      if (rc == -1)
      {
         std::cout << "setAddr failed " << socket.error() << std::endl;
         return 1;
      }

      auto src = socket.open();
      if (src == INVALID_SOCKET)
      {
         std::cout << "open failed " << socket.error() << std::endl;
         return 1;
      }

      rc = socket.bind();
      if (rc == -1)
      {
         std::cout << "bind failed " << socket.error() << std::endl;
         return 1;
      }

      constexpr int len = 1500;
      char buff[len];
      int nbytes = 0;
      while ((nbytes = socket.recv(buff, len)) > 0)
      {
         buff[nbytes] = 0;
         std::cout << buff << std::endl;
      }
   }
   catch (const std::exception &exp)
   {
      std::cout << exp.what();
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
void usage()
{
   std::cout << "Usage\n";
   std::cout << "  broadcast --send  IP_Address port\n",
       std::cout << "      send broadcast packets on the IP_Address\n\n";
   std::cout << "  broadcast --recv port\n";
   std::cout << "      receive broadcast packet\n\n";
}

int main(int argc, char **argv)
{
   if (argc < 3)
   {
      usage();
      return 1;
   }

   if ((strcmp(argv[1], "--send") == 0) && argc == 4)
   {
      return broadcastSender(std::string(argv[2]), std::atoi(argv[3]));
   }

   if ((strcmp(argv[1], "--recv") == 0) && argc == 3)
   {
      return broadcastReceiver(std::atoi(argv[2]));
   }

   usage();
   return 1;
}