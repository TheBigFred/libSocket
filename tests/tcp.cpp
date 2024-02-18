////////////////////////////////////////////////////////////////////////////////
// File      : tcp.cpp
// Contents  : broadcast test application
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <cerrno>
#include <cstring>
#include <string>
#include <iostream>
#include "socketstream.h"


int connectloop(const std::string &IpAddr, uint16_t Port)
{
  while(1)
  {
    SocketSTREAM sockSnd(AF_INET);
    sockSnd.setAddr(IpAddr, Port);
    if (sockSnd.open() == INVALID_SOCKET)
    {
      std::cout << "INVALID_SOCKET\n";
      return 1;
    }
    if (sockSnd.connect() != 0)
    {
      std::cout << "connect failed\n";
      return 1;
    }
  }
}

int connectloopMP(const std::string &IpAddr, uint16_t nbPort)
{
  uint16_t Port = 1025;
  while(1)
  {
    SocketSTREAM sockSnd(AF_INET);
    sockSnd.setAddr(IpAddr, Port++);
    if (sockSnd.open() == INVALID_SOCKET)
    {
      std::cout << "INVALID_SOCKET\n";
      return 1;
    }
    sockSnd.connect();
    if (Port == 0) Port = 1025;
    if (Port >= 1025+nbPort) Port = 1025;
  }
}

int send(const std::string &IpAddr, uint16_t Port)
{
  SocketSTREAM sockSnd(AF_INET);
  sockSnd.setAddr(IpAddr, Port);
  if (sockSnd.open() == INVALID_SOCKET)
  {
    std::cout << "INVALID_SOCKET\n";
    return 1;
  }
  if (sockSnd.connect() != 0)
  {
    std::cout << "connect failed\n";
    return 1;
  }

  char buffer[1480];
  while(1)
  {
    sockSnd.send(buffer,1480);
  }
  return 0;
}

int receive(uint16_t Port)
{
  SocketSTREAM sockRcv(AF_INET);
  sockRcv.setAnyAddr(Port);
  sockRcv.open();
  auto rc = sockRcv.bind();
  if (rc == -1)
    std::cout << strerror(errno) << std::endl;
  
  sockRcv.listen();
  SocketSTREAM wsock = sockRcv.accept();
  
  char buffer[1480];
  while (1)
  {
    wsock.recv(buffer,1480);
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
void usage()
{
  std::cout << "Usage\n";
  std::cout << "  tcp --send  IP_Address port\n",
  std::cout << "  tcp --recv port\n";
  std::cout << "  tcp --connectloop IP_Address port\n";
  std::cout << "  tcp --connectloopMP IP_Address nbport\n";
}

int main(int argc, char **argv)
{
   if (argc < 2)
   {
      usage();
      return 1;
   }

   if ((strcmp(argv[1], "--send") == 0) && argc == 4)
   {
      return send(std::string(argv[2]), std::atoi(argv[3]));
   }

   if ((strcmp(argv[1], "--recv") == 0) && argc == 3)
   {
      return receive(std::atoi(argv[2]));
   }

   if ((strcmp(argv[1], "--connectloop") == 0) && argc == 4)
   {
      return connectloop(std::string(argv[2]), std::atoi(argv[3]));
   }

  if ((strcmp(argv[1], "--connectloopMP") == 0) && argc == 4)
   {
      return connectloopMP(std::string(argv[2]), std::atoi(argv[3]));
   }

   usage();
   return 1;
}
