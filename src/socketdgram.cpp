////////////////////////////////////////////////////////////////////////////////
// File      : socketdgram.cpp
// Contents  : socket datagram implementation
//
// Author(s) : Frederic Gerard - mailfge@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>

#include "endian.h"
#include "socketdgram.h"

/**
 * @brief Construct a new SocketDGRAM object.
 * 
 * @param domain : AF_INET for IPV4, AF_INET6 for IPV6, AF_UNSPEC for autodetect.
 * @param proto : IPPROTO_UDP, or a valid IPPROTO_xxxx.
 */
SocketDGRAM::SocketDGRAM(int domain, int proto) : Socket(domain, SOCK_DGRAM, proto)
{
}

SocketDGRAM::~SocketDGRAM()
{
   igmpLeave();
}

/**
 * @brief Enable broadcast socket option.
 * 
 * @return int : zero on success.
 */
int SocketDGRAM::enableBroadcast() noexcept
{
   int n = 1;
   return setsockopt(mSock, SOL_SOCKET, SO_BROADCAST, CPCHAR_WSCAST(&n), sizeof(n));
}

/**
 * @brief Set the Multicast TTL.
 * 
 * @param ttl : The time to live value.
 * @return int : zero on success.
 */
int SocketDGRAM::setMulticastTTL(uint8_t ttl) noexcept
{
   int value = ttl;
   return setsockopt(mSock, IPPROTO_IP, IP_MULTICAST_TTL, CPCHAR_WSCAST(&value), sizeof(value));
}

/**
 * @brief Enable the port ruese socket option.
 *
 * Under UNIX like, we set the SO_REUSEPORT socket option.
 * Under winsoc,  we set the SO_REUSEADDR socket option.
 * 
 * @return int : zero on success.
 */
int SocketDGRAM::setReusePort(bool value /*=true*/) noexcept
{
   int reuse = static_cast<int>(value);
#ifndef OS_WINDOWS
   return setsockopt(mSock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
#else
   return setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, CPCHAR_WSCAST(&reuse), sizeof(reuse));
#endif
}

/**
 * @brief Send a igmp v2 join.
 *
 * You can send up to 10 igmp v2 request.
 * You can override IGMP_REQ_ARRAY_SIZE in the cmake cache to change this limitation. 
 * 
 * @param GroupAddr : The group address to join.
 * @param IfIndex : The interface index that should send the igmp join.
 * @return int : zero on success.
 */
int SocketDGRAM::igmpJoin(const std::string &GroupAddr, int IfIndex)
{
   if (mreq_cnt < IGMP_REQ_ARRAY_SIZE)
   {
      auto saddr = SockAddr(GroupAddr);
      if (saddr.size == 0)
         return -1;

      mreq[mreq_cnt].gr_group = saddr.ss;
      mreq[mreq_cnt].gr_interface = IfIndex;
      int rc = setsockopt(mSock, IPPROTO_IP, MCAST_JOIN_GROUP, CPCHAR_WSCAST(&mreq[mreq_cnt]), sizeof(mreq[mreq_cnt]));
      if (rc == 0)
         mreq_cnt++;
      return rc;
   }
   return -1;
}

/**
 * @brief Send a igmp v3 join.
 * 
 * You can send up to 10 igmp v3 request.
 * You can override IGMP_REQ_ARRAY_SIZE in the cmake cache to change this limitation. 
 *
 * @param sourceAddr : The source address of the specifed group address.
 * @param GroupAddr : The group address to join.
 * @param IfIndex : The interface index that should send the igmp join.
 * @return int : zero on success.
 */
int SocketDGRAM::igmpJoin(const std::string &sourceAddr, const std::string &GroupAddr, int IfIndex)
{
   if (mreq_src_cnt < IGMP_REQ_ARRAY_SIZE)
   {
      auto GrpAddr = SockAddr(GroupAddr);
      if (GrpAddr.size == 0)
         return -1;

      auto srcAddr = SockAddr(sourceAddr);
      if (srcAddr.size == 0)
         return -1;

      mreq_src[mreq_src_cnt].gsr_group = GrpAddr.ss;
      mreq_src[mreq_src_cnt].gsr_source = srcAddr.ss;
      mreq_src[mreq_src_cnt].gsr_interface = IfIndex;

      int rc = setsockopt(mSock, IPPROTO_IP, MCAST_JOIN_SOURCE_GROUP, CPCHAR_WSCAST(&mreq_src[mreq_src_cnt]), sizeof(mreq_src[mreq_src_cnt]));
      if (rc == 0)
         mreq_src_cnt++;
      return rc;
   }
   return -1;
}

/**
 * @brief Leave all requested igmp join.
 * 
 * @return int : zero on success.
 */
int SocketDGRAM::igmpLeave()
{
   int rc = 0;
   for (uint32_t i = 0; i < mreq_src_cnt; i++)
      rc |= setsockopt(mSock, IPPROTO_IP, MCAST_LEAVE_SOURCE_GROUP, CPCHAR_WSCAST(&mreq_src[i]), sizeof(mreq_src[i]));

   for (uint32_t i = 0; i < mreq_cnt; i++)
      rc |= setsockopt(mSock, IPPROTO_IP, MCAST_LEAVE_GROUP, CPCHAR_WSCAST(&mreq[i]), sizeof(mreq[i]));

   return rc;
}

/**
 * @brief Readback the socket address.
 *
 * @return socketaddr : the current socketaddr value.
 */
socketaddr SocketDGRAM::getSocketaddr() const noexcept
{
   return mAddr;
}

/**
 * @brief Scatter / Gather
 *
 * This method encpasulates the sendmsg BSD system call and
 * the winsoc WSASendMsg. 
 *
 * The winsoc API does not use the msghdr struct,
 * thus we have to translate msghdr sturct to WSAMSG struct.
 * To avoid dynamic allocation, we use a fix array of 10 WSABUFF.
 * You can override this WSABUFF_ARRAY_SIZE in the cmake cache.
 * 
 * @param msg : the buffers collection to send.
 * @return int : zero on success.
 */
int SocketDGRAM::send(const msghdr &msg) const
{
#ifdef OS_UNIX
   return sendmsg(mSock, &msg, mSendFlags);
#elif defined OS_WINDOWS

   if (WSABUFF_ARRAY_SIZE < msg.msg_iovlen)
      return -1;

   WSAMSG Msg = {};
   Msg.name = (LPSOCKADDR)msg.msg_name;
   Msg.namelen = msg.msg_namelen;
   Msg.Control.buf = (char *)msg.msg_control;
   Msg.Control.len = msg.msg_controllen;

   WSABUF piov[WSABUFF_ARRAY_SIZE];
   for (uint32_t i = 0; i < msg.msg_iovlen; i++)
   {
      piov[i].buf = (char *)msg.msg_iov[i].iov_base;
      piov[i].len = msg.msg_iov[i].iov_len;
   }
   Msg.lpBuffers = piov;
   Msg.dwBufferCount = msg.msg_iovlen;
   Msg.dwFlags = msg.msg_flags;

   DWORD nbBytesSent = 0;
   auto rc = WSASendMsg(mSock, &Msg, mSendFlags, &nbBytesSent, nullptr, nullptr);

   if (rc == SOCKET_ERROR)
      return -1;
   return nbBytesSent;
#endif
}

int SocketDGRAM::send(const void *buffer, uint32_t size) const noexcept
{
   if (buffer == nullptr || size == 0)
      return -1;

   return sendto(mSock, CPCHAR_WSCAST(buffer), size, mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(const std::string &binary) const noexcept
{
   return sendto(mSock, binary.data(), INT_WSCAST(binary.size()), mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(const char *txt) const noexcept
{
   if (txt == nullptr)
      return -1;
   return sendto(mSock, txt, INT_WSCAST(strlen(txt) + 1), mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(uint8_t data) const noexcept
{
   return sendto(mSock, CPCHAR_WSCAST(&data), sizeof(uint8_t), mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(uint16_t data) const noexcept
{
   uint16_t d = htons(data);
   return sendto(mSock, CPCHAR_WSCAST(&d), sizeof(uint16_t), mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(uint32_t data) const noexcept
{
   uint32_t d = htonl(data);
   return sendto(mSock, CPCHAR_WSCAST(&d), sizeof(uint32_t), mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(uint64_t data) const noexcept
{
   uint64_t d = htonll(data);
   return sendto(mSock, CPCHAR_WSCAST(&d), sizeof(uint64_t), mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(int8_t data) const noexcept
{
   return sendto(mSock, CPCHAR_WSCAST(&data), sizeof(int8_t), mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(int16_t data) const noexcept
{
   int16_t d = htons(data);
   return sendto(mSock, CPCHAR_WSCAST(&d), sizeof(int16_t), mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(int32_t data) const noexcept
{
   int32_t d = htonl(data);
   return sendto(mSock, CPCHAR_WSCAST(&d), sizeof(int32_t), mSendFlags, &mAddr.sa, mAddr.size);
}

int SocketDGRAM::send(int64_t data) const noexcept
{
   int64_t d = htonll(data);
   return sendto(mSock, CPCHAR_WSCAST(&d), sizeof(int64_t), mSendFlags, &mAddr.sa, mAddr.size);
}

/**
 * @brief Scatter / Gather
 *
 * This method encpasulates the recvmsg BSD system call and
 * the winsoc WSARecvFrom. 
 *
 * The winsoc API does not use the msghdr struct,
 * thus we have to translate msghdr sturct to WSAMSG struct.
 * To avoid dynamic allocation, we use a fix array of 10 WSABUFF.
 * You can override this WSABUFF_ARRAY_SIZE in the cmake cache.
 * 
 * @param msg : the buffers collection.
 * @return int : zero on success.
 */
int SocketDGRAM::recv(struct msghdr &msg)
{
#ifdef OS_UNIX
   return recvmsg(mSock, &msg, mRecvFlags);
#elif defined OS_WINDOWS

   WSABUF piov[WSABUFF_ARRAY_SIZE] = {};
   for (uint32_t i = 0; i < msg.msg_iovlen; i++)
   {
      piov[i].buf = (char *)msg.msg_iov[i].iov_base;
      piov[i].len = msg.msg_iov[i].iov_len;
   }

   DWORD nbBytesRecvd = 0;
   auto rc = WSARecvFrom(mSock, piov, msg.msg_iovlen,
                         &nbBytesRecvd, (LPDWORD)&mRecvFlags,
                         (sockaddr *)msg.msg_name, &msg.msg_namelen,
                         nullptr, nullptr);

   if (rc == SOCKET_ERROR)
      return -1;

   return nbBytesRecvd;
#endif
}

int SocketDGRAM::recv(void *buffer, uint32_t size) noexcept
{
   if (buffer == nullptr || size == 0)
      return -1;

   return recvfrom(mSock, PCHAR_WSCAST(buffer), size, mRecvFlags, &mAddr.sa, &mAddr.size);
}

int SocketDGRAM::recv(uint8_t &data) noexcept
{
   return recvfrom(mSock, PCHAR_WSCAST(&data), sizeof(data), mRecvFlags, &mAddr.sa, &mAddr.size);
}

int SocketDGRAM::recv(uint16_t &data) noexcept
{
   uint16_t d = 0;
   int rc = recvfrom(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags, &mAddr.sa, &mAddr.size);
   data = ntohs(d);
   return rc;
}

int SocketDGRAM::recv(uint32_t &data) noexcept
{
   uint32_t d = 0;
   int rc = recvfrom(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags, &mAddr.sa, &mAddr.size);
   data = ntohl(d);
   return rc;
}

int SocketDGRAM::recv(uint64_t &data) noexcept
{
   uint64_t d = 0;
   int rc = recvfrom(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags, &mAddr.sa, &mAddr.size);
   data = ntohll(d);
   return rc;
}

int SocketDGRAM::recv(int8_t &data) noexcept
{
   return recvfrom(mSock, PCHAR_WSCAST(&data), sizeof(data), mRecvFlags, &mAddr.sa, &mAddr.size);
}

int SocketDGRAM::recv(int16_t &data) noexcept
{
   int16_t d = 0;
   int rc = recvfrom(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags, &mAddr.sa, &mAddr.size);
   data = ntohs(d);
   return rc;
}

int SocketDGRAM::recv(int32_t &data) noexcept
{
   int32_t d = 0;
   int rc = recvfrom(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags, &mAddr.sa, &mAddr.size);
   data = ntohl(d);
   return rc;
}

int SocketDGRAM::recv(int64_t &data) noexcept
{
   int64_t d = 0;
   int rc = recvfrom(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags, &mAddr.sa, &mAddr.size);
   data = ntohll(d);
   return rc;
}
