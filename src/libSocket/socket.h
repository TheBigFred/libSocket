////////////////////////////////////////////////////////////////////////////////
// File      : socket.h
// Contents  : The socket interface
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "socket_addr.h"
#include "socket_portability.h"
#include <libSocket/export.h>
#include <string>

class LIBSOCKET_EXPORT Socket
{
public:
   Socket(int domain, int type, int proto = 0);
   virtual ~Socket();

   SOCKET open() noexcept;
   bool isOpen() noexcept;
   int close() noexcept;
   int setAnyAddr(int domain, uint16_t port) noexcept;
   int setAnyAddr(uint16_t port) noexcept;
   int setAddr(const socketaddr &sa) noexcept;
   int setAddr(const sockaddr_in &sa) noexcept;
   int setAddr(const sockaddr_in6 &sa) noexcept;
#ifdef OS_UNIX   
   int setAddr(const sockaddr_ll &sa) noexcept;
#endif
   int setAddr(int domain, const std::string &, uint16_t port) noexcept;
   int setAddr(const std::string &, uint16_t port) noexcept;
   int bind() noexcept;
   uint16_t getPort() const;
   socketaddr getSocketaddr() const noexcept;
   
   int error() const;

   int setOption(int level, int option_name, const void *option_value, int option_len) noexcept;
   int getOption(int level, int option_name, void *option_value, int *option_len) noexcept;

   int setRecvTimeout(uint32_t s, uint32_t ms) noexcept;
   int setSendTimeout(uint32_t s, uint32_t ms) noexcept;

   int getSendFlags() noexcept;
   int getRecvFlags() noexcept;

   void setSendFlag(int) noexcept;
   void setRecvFlag(int) noexcept;

   void setSendFlags(int) noexcept;
   void setRecvFlags(int) noexcept;

   void resetSendFlag(int) noexcept;
   void resetRecvFlag(int) noexcept;

   void resetSendFlags() noexcept;
   void resetRecvFlags() noexcept;

   virtual int send(const msghdr &message) const noexcept;
   virtual int send(const void *buffer, uint32_t size) const noexcept = 0;
   virtual int send(const std::string &binary) const noexcept = 0;
   virtual int send(const char *txt) const noexcept = 0;
   virtual int send(uint8_t data) const noexcept = 0;
   virtual int send(uint16_t data) const noexcept = 0;
   virtual int send(uint32_t data) const noexcept = 0;
   virtual int send(uint64_t data) const noexcept = 0;
   virtual int send(int8_t data) const noexcept = 0;
   virtual int send(int16_t data) const noexcept = 0;
   virtual int send(int32_t data) const noexcept = 0;
   virtual int send(int64_t data) const noexcept = 0;

   virtual int recv(msghdr &message) noexcept;
   virtual int recv(void *buffer, uint32_t size) noexcept = 0;
   virtual int recv(uint8_t &data) noexcept = 0;
   virtual int recv(uint16_t &data) noexcept = 0;
   virtual int recv(uint32_t &data) noexcept = 0;
   virtual int recv(uint64_t &data) noexcept = 0;
   virtual int recv(int8_t &data) noexcept = 0;
   virtual int recv(int16_t &data) noexcept = 0;
   virtual int recv(int32_t &data) noexcept = 0;
   virtual int recv(int64_t &data) noexcept = 0;

protected:
   Socket();
   SOCKET mSock = INVALID_SOCKET;
   int mDomain = AF_UNSPEC;
   int mType = 0;
   int mProto = 0;

   socketaddr mAddr = {};

   unsigned int mSendFlags = 0;
   unsigned int mRecvFlags = 0;
#ifdef OS_WINDOWS
   WSADATA wsaData;
#endif
};
