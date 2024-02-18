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
////////////////////////////////////////////////////////////////////////////////
int rawSender(socketaddr macAddrSrc, socketaddr macAddrDst, uint32_t packetSize, long time)
{
   try
   {
      auto socket = SocketDGRAM(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
      if (socket.open() == INVALID_SOCKET)
      {
         std::cout << "open failed " << socket.error() << std::endl;
         return 1;
      }

      uint32_t bufferLenght = (packetSize > sizeof(ethhdr)) ? packetSize : sizeof(ethhdr);
      auto buff = std::unique_ptr<char[]>(new char[bufferLenght]);

      ethhdr* eth = (ethhdr*)buff.get();
      memcpy(eth->h_source, macAddrSrc.sa.sa_data, 6);
      memcpy(eth->h_dest,   macAddrDst.sa.sa_data, 6);
      eth->h_proto = (packetSize < 1500) ? htons(0x7999) : htons(0x8870);

      int* data = nullptr;
      if (bufferLenght >= sizeof(ethhdr)+4)
      {
         data = (int*)(buff.get() + sizeof(ethhdr));
         *data = 0;
      }

      int numPackets = 0;
      int size = 0;
      timespec ts;
      ts.tv_sec = 0;
      ts.tv_nsec = time;

      std::chrono::time_point<std::chrono::steady_clock> T1 = std::chrono::steady_clock::now();
      do
      {
         if (data!=nullptr) *data = numPackets;
         int rc = socket.send(buff.get(), bufferLenght);
         if (rc == -1)
         {
            std::cout << "send failed " << strerror(socket.error()) << std::endl;
            return 1;
         }
         size += rc;
         numPackets++;
         std::chrono::time_point<std::chrono::steady_clock> T2 = std::chrono::steady_clock::now();
         int64_t dTns = std::chrono::duration_cast<std::chrono::nanoseconds>(T2-T1).count();
         if (dTns >= 1000*1000*1000)
         {
            double dTsec = (double)dTns/(1000*1000*1000);
            auto MbitSec = ((size * 8)/dTsec) / (1024*1024);
            std::cout << std::setw(8) << std::setfill(' ');
            std::cout << std::fixed << std::setprecision(3);
            std::cout << MbitSec << " Mbio/s    NbPackets: "<< numPackets << "\n";
            T1 = T2;
            size = 0;
         }
         if (time)
             nanosleep(&ts,nullptr);
      } while (1);
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
      int* data = (int*)(buff + sizeof(ethhdr));
      while ((nbytes = socket.recv(buff, len)) > 0)
      {
         std::cout << "\nPacket received: " << nbytes << "Bytes\n";
         std::cout << "\tSrc Addr: " << macAddr(eth->h_source) << std::endl;
         std::cout << "\tDst Addr: " << macAddr(eth->h_dest) << std::endl;
         std::cout << "\tProtocol: " << std::hex << ntohs(eth->h_proto) << std::dec << std::endl;
         std::cout << "\tData: " << *data << std::endl;
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
   std::cout << "  raw --send IfNameSrc MacDst [packetSize=1480]\n",
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
      auto src = MacAddr_fromString(argv[2]);
      auto dst = MacAddr_fromString(argv[3]);

      uint32_t packetSize = 1480;
      long     time       = 0;

      if (argc == 4)
         return rawSender(src, dst, (uint32_t)1480, (long)0);
      else
      {
         int pktSize = std::atoi(argv[4]);
         if (pktSize < 0) pktSize = 0;
         packetSize = static_cast<uint32_t>(pktSize);

         if (argc == 6) time = std::atol(argv[5]);
      }
      return rawSender(src, dst, packetSize, time);
   }

   else if (strcmp(argv[1], "--recv") == 0)
   {
      return rawReceiver();
   }
   else
      usage();

   return 1;
}
