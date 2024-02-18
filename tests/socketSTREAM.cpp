////////////////////////////////////////////////////////////////////////////////
// File      : SocketSTREAM.cpp
// Contents  : gtests SocketSTREAM
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
//  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <random>
#include <cstring>
#include <string>
#include <thread>
#include <chrono>
#include "socketstream.h"

#include "extern.h"

constexpr uint8_t cu8 = 254;
constexpr uint16_t cu16 = 50000;
constexpr uint32_t cu32 = 4000000000;
constexpr uint64_t cu64 = 4000000000000;
const char *txt = "Hello!";
int txtSize = (int)strlen(txt) + 1;
std::string string("Hello world!!!");

void SndThread(const std::string &IpAddr, uint16_t Port)
{
   SocketSTREAM sockSnd(AF_INET);
   ASSERT_EQ(sockSnd.setAddr(IpAddr, Port), 0);
   ASSERT_NE(sockSnd.open(), INVALID_SOCKET);
   ASSERT_EQ(sockSnd.connect(), 0);

   uint8_t ack;
   ASSERT_EQ(sockSnd.send(cu8), sizeof(cu8));      sockSnd.recv(ack);
   ASSERT_EQ(sockSnd.send(cu16), sizeof(cu16));    sockSnd.recv(ack);
   ASSERT_EQ(sockSnd.send(cu32), sizeof(cu32));    sockSnd.recv(ack);
   ASSERT_EQ(sockSnd.send(cu64), sizeof(cu64));    sockSnd.recv(ack);
   ASSERT_EQ(sockSnd.send(txt), txtSize);          sockSnd.recv(ack);
   ASSERT_EQ(sockSnd.send(string), string.size()); sockSnd.recv(ack);
   ASSERT_EQ(sockSnd.close(), 0);
}

void RcvThread(uint16_t Port)
{
   SocketSTREAM sockRcv(AF_INET);
   ASSERT_EQ(sockRcv.setAnyAddr(Port), 0);
   ASSERT_NE(sockRcv.open(), INVALID_SOCKET);
   auto rc = sockRcv.bind();
   if (rc == -1)
      std::cout << strerror(errno) << std::endl;
   ASSERT_EQ(rc, 0);

   ASSERT_EQ(sockRcv.listen(), 0);

   SocketSTREAM wsock = sockRcv.accept();
   ASSERT_EQ(wsock.isOpen(), true);

   uint8_t u8 = 0;
   ASSERT_EQ(wsock.recv(u8), sizeof(u8));
   ASSERT_EQ(u8, cu8);
   wsock.send((uint8_t)1);

   uint16_t u16 = 0;
   ASSERT_EQ(wsock.recv(u16), sizeof(u16));
   ASSERT_EQ(u16, cu16);
   wsock.send((uint8_t)1);

   uint32_t u32 = 0;
   ASSERT_EQ(wsock.recv(u32), sizeof(u32));
   ASSERT_EQ(u32, cu32);
   wsock.send((uint8_t)1);

   uint64_t u64 = 0;
   ASSERT_EQ(wsock.recv(u64), sizeof(u64));
   ASSERT_EQ(u64, cu64);
   wsock.send((uint8_t)1);

   char *buffer[50] = {};
   ASSERT_EQ(wsock.recv(buffer, 50), txtSize);
   ASSERT_EQ(strcmp(txt, (const char *)buffer), 0);
   wsock.send((uint8_t)1);

   ASSERT_EQ(wsock.recv(buffer, 50), string.size());
   ASSERT_EQ(memcmp((const void *)string.data(), (const void *)buffer, string.size()), 0);
   wsock.send((uint8_t)1);

   ASSERT_EQ(wsock.close(), 0);
   ASSERT_EQ(sockRcv.close(), 0);
}

TEST(SocketSTREAM, send_recv_ping_pong)
{
   auto Port = port + portOffset++;

   auto rcvTh = std::thread(RcvThread, Port);
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   auto sndTh = std::thread(SndThread, "127.0.0.1", Port);
   rcvTh.join();
   sndTh.join();
}

TEST(SocketSTREAM, getPort)
{
   auto Port = port + portOffset++;

   SocketSTREAM sockSrv(AF_INET);
   ASSERT_EQ(sockSrv.setAnyAddr(Port), 0);
   ASSERT_NE(sockSrv.open(), INVALID_SOCKET);
   ASSERT_NO_THROW(sockSrv.getPort());
}

void SndFileThread(uint16_t Port)
{
   SocketSTREAM sockSnd(AF_INET);
   ASSERT_EQ(sockSnd.setAddr("127.0.0.1", Port), 0);
   ASSERT_NE(sockSnd.open(), INVALID_SOCKET);
   ASSERT_EQ(sockSnd.connect(), 0);

   std::string file = path + "/archive.tar";
   ASSERT_NO_THROW(sockSnd.sendFile(file));

   ASSERT_EQ(sockSnd.close(), 0);
}

void RcvFileThread(uint16_t Port)
{
   SocketSTREAM sockRcv(AF_INET);
   ASSERT_EQ(sockRcv.setAnyAddr(Port), 0);
   ASSERT_NE(sockRcv.open(), INVALID_SOCKET);
   ASSERT_EQ(sockRcv.bind(), 0);

   ASSERT_EQ(sockRcv.listen(), 0);

   SocketSTREAM wsock = sockRcv.accept();
   ASSERT_EQ(wsock.isOpen(), true);

   std::string file = path + "/rcvArchive.tar";
   ASSERT_NO_THROW(wsock.recvFile(file));

   ASSERT_EQ(wsock.close(), 0);
   ASSERT_EQ(sockRcv.close(), 0);
}

TEST(SocketSTREAM, send_recv_file)
{
   auto Port = port + portOffset++;
   auto rcvTh = std::thread(RcvFileThread, Port);
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   auto sndTh = std::thread(SndFileThread, Port);
   rcvTh.join();
   sndTh.join();
}
