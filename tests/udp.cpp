////////////////////////////////////////////////////////////////////////////////
// File      : udp.cpp
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
#include <iomanip>
#include "socketdgram.h"

void printSocketaddr(const socketaddr &saddr)
{
   char *buffer[INET6_ADDRSTRLEN] = {};

   if (saddr.sa.sa_family == AF_INET)
      inet_ntop(saddr.sa.sa_family, (void *)&saddr.s4.sin_addr, (char *)buffer, INET6_ADDRSTRLEN);
   else
      inet_ntop(saddr.sa.sa_family, (void *)&saddr.s6.sin6_addr, (char *)buffer, INET6_ADDRSTRLEN);

   std::cout << (char *)buffer << " : ";
   std::cout << ntohs(saddr.s4.sin_port) << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
int udpSender(const std::string &IpAddr, uint16_t port)
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

      std::string str("Hello wolrd!!! ");
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

         constexpr int len = 1500;
         char buff[len];
         auto nbytes = socket.recv(buff, len);
         if (nbytes == -1)
         {
            std::cout << "recv failed " << socket.error() << std::endl;
            return 1;
         }
         buff[nbytes] = 0;
         std::cout << "recv " << buff << " from ";
         printSocketaddr(socket.getSocketaddr());
         std::cout << std::endl;

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
int udpReceiver(uint16_t port)
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
         std::cout << "recv " << buff << " from ";
         printSocketaddr(socket.getSocketaddr());
         std::cout << std::endl;

         std::string ack("ack ");
         ack.append(buff);
         socket.send(ack);
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
   std::cout << "  udp --send  IP_Address port\n",
       std::cout << "      send datagram on the IP_Address\n\n";
   std::cout << "  udp --recv port\n";
   std::cout << "      receive datagram\n\n";
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
      return udpSender(std::string(argv[2]), std::atoi(argv[3]));
   }

   if ((strcmp(argv[1], "--recv") == 0) && argc == 3)
   {
      return udpReceiver(std::atoi(argv[2]));
   }

   usage();
   return 1;
}