////////////////////////////////////////////////////////////////////////////////
// File      : socketdgram.h
// Contents  : socket datagram interface
//
// Author(s) : Frederic Gerard - mailfge@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "socket.h"

class LIBSOCKET_EXPORT SocketDGRAM : public Socket
{
public:
   SocketDGRAM(int domain = AF_UNSPEC, int proto = IPPROTO_UDP);
   ~SocketDGRAM();

   int enableBroadcast() noexcept;
   int setMulticastTTL(uint8_t value) noexcept;
   int setReusePort(bool value = true) noexcept;

   int igmpJoin(const std::string &GroupAddr, int IfIndex);
   int igmpJoin(const std::string &sourceAddr, const std::string &GroupAddr, int IfIndex);
   int igmpLeave();

   socketaddr getSocketaddr() const noexcept;

   int send(const msghdr &message) const;
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

   int recv(struct msghdr &message) override;
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
   group_req mreq[IGMP_REQ_ARRAY_SIZE] = {};
   group_source_req mreq_src[IGMP_REQ_ARRAY_SIZE] = {};
};
