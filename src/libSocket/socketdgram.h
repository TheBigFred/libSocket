////////////////////////////////////////////////////////////////////////////////
// File      : socketdgram.h
// Contents  : socket datagram interface
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

class EXPORT SocketDGRAM : public Socket
{
public:
   SocketDGRAM(int domain = AF_UNSPEC, int proto = IPPROTO_UDP);
   SocketDGRAM(int domain, int type, int proto);
   ~SocketDGRAM();

   int enableBroadcast() noexcept;
   int setMulticastTTL(uint8_t value) noexcept;
   int setReusePort(bool value = true) noexcept;

   int igmpJoin(const std::string &GroupAddr, int IfIndex);
   int igmpJoin(const std::string &sourceAddr, const std::string &GroupAddr, int IfIndex);
   int igmpLeave();

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

private:
   uint32_t mreq_cnt = 0;
   uint32_t mreq_src_cnt = 0;
   group_req* mpreq = nullptr;
   group_source_req* mpreq_src = nullptr;
};
