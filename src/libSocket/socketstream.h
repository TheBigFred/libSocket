////////////////////////////////////////////////////////////////////////////////
// File      : socketstream.h
// Contents  : socket stream interface
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "socket.h"

class EXPORT SocketSTREAM : public Socket
{
public:
   SocketSTREAM(int domain = AF_UNSPEC, int proto = IPPROTO_TCP);
   SocketSTREAM(int domain, int type, int proto);
   SocketSTREAM(const SocketSTREAM &) = default;
   ~SocketSTREAM() = default;

   int listen(int n = 1);
   SocketSTREAM accept(bool block = true);
   int connect() noexcept;
   int KeepAlive(bool enable = true) noexcept;

   int send(const msghdr &message) const noexcept override;
   int send(const void *buffer, uint32_t size) const noexcept override;
   int send(const std::string &binary) const noexcept override;
   int send(const char *txt) const noexcept override;
   int send(uint8_t data) const noexcept override;
   int send(uint16_t data) const noexcept override;
   int send(uint32_t data) const noexcept override;
   int send(uint64_t data) const noexcept override;
   int send(int8_t data) const noexcept override;
   int send(int16_t data) const noexcept override;
   int send(int32_t data) const noexcept override;
   int send(int64_t data) const noexcept override;

   int recv(msghdr &message) noexcept override;
   int recv(void *buffer, uint32_t size) noexcept override;
   int recv(uint8_t &data) noexcept override;
   int recv(uint16_t &data) noexcept override;
   int recv(uint32_t &data) noexcept override;
   int recv(uint64_t &data) noexcept override;
   int recv(int8_t &data) noexcept override;
   int recv(int16_t &data) noexcept override;
   int recv(int32_t &data) noexcept override;
   int recv(int64_t &data) noexcept override;

   void sendFile(const std::string& fileName, void (*callback)(uint64_t Progress, uint64_t Target) = nullptr);
   void recvFile(const std::string& fileName, void (*callback)(uint64_t Progress, uint64_t Target) = nullptr);

private:
   SocketSTREAM(SOCKET wSock, const socketaddr &addr);
   void setNONBLOCK(bool on = true);
   bool mNONBLOCK = false;
};
