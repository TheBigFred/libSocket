////////////////////////////////////////////////////////////////////////////////
// File      : raw.cpp
// Contents  : raw test application
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
#include <iomanip>
#include <iostream>

#include "socketdgram.h"

#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>

////////////////////////////////////////////////////////////////////////////////
int rawSender(socketaddr macAddrSrc, socketaddr macAddrDst, uint16_t proto=0x8870)
{
   try
   {
      auto socket = SocketDGRAM(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

      if (socket.open() == INVALID_SOCKET)
      {
         std::cout << "open failed " << socket.error() << std::endl;
         return 1;
      }

      constexpr int len = 9000;
      char buf[len];
      ethhdr* eth = (ethhdr*)buf;
      memcpy(eth->h_source, macAddrSrc.sa.sa_data, 6);
      memcpy(eth->h_dest,   macAddrDst.sa.sa_data, 6);
      eth->h_proto = htons(proto);

      int* data = (int*)buf + sizeof(ethhdr);
      *data = 0;
      int n = 0;
      do
      {
         *data++;
         if (socket.send(buf, len) == -1)
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
int rawReceiver()
{
   try
   {
      auto socket = SocketDGRAM(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

      if (socket.open() == INVALID_SOCKET)
      {
         std::cout << "open failed " << strerror(socket.error()) << std::endl;
         return 1;
      }

      constexpr int len = 65536;
      char buff[len];
      int nbytes = 0;
      ethhdr* eth = (ethhdr*)buff;
      int* data = (int*)buff + sizeof(ethhdr);
      while ((nbytes = socket.recv(buff, len)) > 0)
      {
         //auto addr = socket.getSocketaddr();
         std::cout << "\nPacket received: " << nbytes << "Bytes\n";
         std::cout << "\tSrc Addr: " << std::hex << std::setfill('0') << std::setw(2) << eth->h_source[0] << ':' << eth->h_source[1] << ':' << eth->h_source[2] << ':' << eth->h_source[3] << ':' << eth->h_source[4] << ':' << eth->h_source[5] << std::endl;
         std::cout << "\tDst Addr: " << std::hex << std::setfill('0') << std::setw(2) << eth->h_dest[0] << ':' << eth->h_dest[1] << ':' << eth->h_dest[2] << ':' << eth->h_dest[3] << ':' << eth->h_dest[4] << ':' << eth->h_dest[5] << std::endl;
         std::cout << "\tProtocol: " << eth->h_proto << std::endl;
         std::cout << "\tData: " << *data << std::endl;
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
   std::cout << "  raw --send IfNameSrc IfNameDst\n",
   std::cout << "  raw --recv\n";
}

int main(int argc, char **argv)
{
   if (argc < 2)
   {
      usage();
      return 1;
   }

   if (strcmp(argv[1], "--send") == 0 && argc >= 4)
   {
      auto src = MacAddr(argv[2]);
      auto dst = MacAddr(argv[3]);
      return rawSender(src, dst);
   }

   else if (strcmp(argv[1], "--recv") == 0)
   {
      return rawReceiver();
   }
   else
      usage();

   return 1;
}