////////////////////////////////////////////////////////////////////////////////
// File      : socketDGRAM.cpp
// Contents  : gtests SocketDGRAM
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
//  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
#include "socketdgram.h"

#include "extern.h"

class SocketDGRAM_Fixture : public ::testing::Test
{
protected:
   SocketDGRAM sockRcv;
   SocketDGRAM sockSnd;

   void SetUp() override
   {
      auto Port = port + portOffset++;

      ASSERT_EQ(sockRcv.setAnyAddr(AF_INET, Port), 0);
      ASSERT_NE(sockRcv.open(), INVALID_SOCKET);
      ASSERT_EQ(sockRcv.bind(), 0);
      ASSERT_EQ(sockRcv.setRecvTimeout(0, 100), 0);

      ASSERT_EQ(sockSnd.setAddr("127.0.0.1", Port), 0);
      ASSERT_NE(sockSnd.open(), INVALID_SOCKET);
   }

   void TearDown() override
   {
      ASSERT_EQ(sockRcv.close(), 0);
      ASSERT_EQ(sockSnd.close(), 0);
   }
};

TEST_F(SocketDGRAM_Fixture, send_recv)
{
   constexpr uint8_t cu8 = 254;
   constexpr uint16_t cu16 = 50000;
   constexpr uint32_t cu32 = 4000000000;
   constexpr uint64_t cu64 = 4000000000000;
   const char *txt = "Hello!";
   int txtSize = (int)strlen(txt) + 1;
   std::string string("Hello world!!!");

   uint8_t u8 = 0;
   ASSERT_EQ(sockSnd.send(cu8), sizeof(cu8));
   ASSERT_EQ(sockRcv.recv(u8), sizeof(u8));
   ASSERT_EQ(u8, cu8);

   uint16_t u16 = 0;
   ASSERT_EQ(sockSnd.send(cu16), sizeof(cu16));
   ASSERT_EQ(sockRcv.recv(u16), sizeof(u16));
   ASSERT_EQ(u16, cu16);

   uint32_t u32 = 0;
   ASSERT_EQ(sockSnd.send(cu32), sizeof(cu32));
   ASSERT_EQ(sockRcv.recv(u32), sizeof(u32));
   ASSERT_EQ(u32, cu32);

   uint64_t u64 = 0;
   ASSERT_EQ(sockSnd.send(cu64), sizeof(cu64));
   ASSERT_EQ(sockRcv.recv(u64), sizeof(u64));
   ASSERT_EQ(u64, cu64);

   char *buffer[50] = {};
   ASSERT_EQ(sockSnd.send(txt), txtSize);
   ASSERT_EQ(sockRcv.recv(buffer, 10), txtSize);
   ASSERT_EQ(strcmp(txt, (const char *)buffer), 0);

   ASSERT_EQ(sockSnd.send(string), string.size());
   ASSERT_EQ(sockRcv.recv(buffer, 50), string.size());
   ASSERT_EQ(memcmp((const void *)string.data(), (const void *)buffer, string.size()), 0);
}

TEST_F(SocketDGRAM_Fixture, send_recv_ping_pong)
{
   constexpr uint8_t cu8 = 10;

   for (uint8_t i = 0; i < 5; i++)
   {
      uint8_t su8 = cu8 + i;
      uint8_t ru8 = 0;
      ASSERT_EQ(sockSnd.send(su8), sizeof(su8));
      ASSERT_EQ(sockRcv.recv(ru8), sizeof(ru8));
      ASSERT_EQ(ru8, su8);

      su8 = cu8 + i * 2;
      ru8 = 0;
      ASSERT_EQ(sockRcv.send(su8), sizeof(su8));
      ASSERT_EQ(sockSnd.recv(ru8), sizeof(ru8));
      ASSERT_EQ(ru8, su8);
   }
}

void Send_Scatter_Gather_Thread(const std::string& Address, uint16_t Port)
{
   uint8_t buffer1[32] = {};
   uint8_t buffer2[1024] = {};

   iovec Iov[2] = {};
   msghdr msg = {};

   Iov[0].iov_base = (void*)buffer1;
   Iov[0].iov_len = 32;
   Iov[1].iov_base = (void*)buffer2;
   Iov[1].iov_len = 1024;

   msg.msg_iov = Iov;
   msg.msg_iovlen = 2;
   msg.msg_control = nullptr;
   msg.msg_controllen = 0;

   SocketDGRAM sockSnd(AF_INET);
   ASSERT_EQ(sockSnd.setAddr(Address, Port), 0);
   ASSERT_NE(sockSnd.open(), INVALID_SOCKET);

   socketaddr saddr = sockSnd.getSocketaddr();
   msg.msg_name = &saddr.sa;
   msg.msg_namelen = saddr.size;

   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   for (uint32_t i = 0; i < 1000; i++)
   {
      memset(buffer1, i % 255, 32);
      memset(buffer2, (i + 1) % 255, 1024);
      ASSERT_EQ(sockSnd.send(msg), 32 + 1024);
   }

   strcpy((char*)buffer1, "STOP");
   ASSERT_EQ(sockSnd.send(msg), 32 + 1024);
   ASSERT_EQ(sockSnd.close(), 0);
}

void Recv_Scatter_Gather_Thread(uint16_t Port)
{
   uint8_t buffer1[32] = {};
   uint8_t buffer2[1200] = {};

   iovec Iov[2] = {};
   msghdr msg = {};

   Iov[0].iov_base = (void*)buffer1;
   Iov[0].iov_len = 32;
   Iov[1].iov_base = (void*)buffer2;
   Iov[1].iov_len = 1200;

   msg.msg_iov = Iov;
   msg.msg_iovlen = 2;
   msg.msg_control = nullptr;
   msg.msg_controllen = 0;

   SocketDGRAM sockRcv(AF_INET);
   ASSERT_EQ(sockRcv.setAnyAddr(Port), 0);
   ASSERT_NE(sockRcv.open(), INVALID_SOCKET);
   ASSERT_EQ(sockRcv.bind(), 0);

   sockaddr sa = {};
   msg.msg_name = &sa;
   msg.msg_namelen = sizeof(sa);

   uint32_t i = 0;
   while (1)
   {
      ASSERT_EQ(sockRcv.recv(msg), 32 + 1024);
      if (strcmp((char*)buffer1, "STOP") != 0)
      {
         ASSERT_EQ(buffer1[0], buffer1[31]);
         ASSERT_EQ(buffer2[0], buffer2[1023]);
         ASSERT_EQ(buffer2[0], (buffer1[0] + 1) % 255);
      }
      else
         break;
      i++;
   }
   ASSERT_EQ(i, 1000);
   ASSERT_EQ(sockRcv.close(), 0);
}

TEST(SocketDGRAM, Scatter_Gather)
{
   auto Port = port + portOffset++;

   auto rcvTh = std::thread(Recv_Scatter_Gather_Thread, Port);
   auto sndTh = std::thread(Send_Scatter_Gather_Thread, "127.0.0.1", Port);
   rcvTh.join();
   sndTh.join();
}

void SendThread(const std::string &GroupAddress, uint16_t Port)
{
   SocketDGRAM sockSnd(AF_INET);
   ASSERT_EQ(sockSnd.setAddr(GroupAddress, Port), 0);
   ASSERT_NE(sockSnd.open(), INVALID_SOCKET);

   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   std::string string("Hello world!!!");
   ASSERT_EQ(sockSnd.send(string), string.size());
   ASSERT_EQ(sockSnd.close(), 0);
}

void RecvThread(const std::string &IfAddress, const std::string &GroupAddress, uint16_t Port)
{
   SocketDGRAM sockRcv(AF_INET);
#if defined(_WIN32) || defined(WIN32)
   ASSERT_EQ(sockRcv.setAnyAddr(Port), 0);
#else
   ASSERT_EQ(sockRcv.setAddr(GroupAddress, Port), 0);
#endif
   ASSERT_NE(sockRcv.open(), INVALID_SOCKET);
   ASSERT_EQ(sockRcv.bind(), 0);

   int index = 0;
   if (!IfAddress.empty() && IfAddress.compare("0.0.0.0"))
   {
      auto ifName = IfName(IfAddress);
      index = IfIndex(ifName);
   }
   ASSERT_EQ(sockRcv.igmpJoin(GroupAddress, index), 0);

   char *buffer[50] = {};
   ASSERT_EQ(sockRcv.recv(buffer, 50), strlen("Hello world!!!"));

   ASSERT_EQ(sockRcv.igmpLeave(), 0);
   ASSERT_EQ(sockRcv.close(), 0);
}

TEST(SocketDGRAM, multicast)
{
   auto Port = port + portOffset++;

   auto rcvTh = std::thread(RecvThread, IfIpAddr, "239.1.1.1", Port);
   auto sndTh = std::thread(SendThread, "239.1.1.1", Port);
   rcvTh.join();
   sndTh.join();
}

TEST(SocketDGRAM, getPort)
{
   auto Port = port + portOffset++;

   SocketDGRAM sock(AF_INET);
   ASSERT_EQ(sock.setAnyAddr(Port), 0);
   ASSERT_NE(sock.open(), INVALID_SOCKET);
   ASSERT_EQ(sock.bind(), 0);
   ASSERT_NO_THROW(sock.getPort());
   ASSERT_EQ(sock.close(), 0);
}
