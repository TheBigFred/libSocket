////////////////////////////////////////////////////////////////////////////////
// File      : socket.cpp
// Contents  : The socket implementation
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
#include <cstdlib>
#include <stdexcept>
#include <system_error>

#include "config.h"
#include "socket.h"
#include "socket_portability.h"

Socket::Socket()
{
#ifdef OS_WINDOWS
   int iRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (iRes != 0)
   {
      auto msg = std::string("WSAStartup failed: ") + std::to_string(iRes);
      throw std::exception(msg.c_str());
   }
#endif
}
/**
 * @brief Construct a new Socket object.
 * 
 * @param domain : AF_INET for IPV4, AF_INET6 for IPV6, AF_UNSPEC for autodetect.
 * @param type : SOCK_STREAM, SOCKET_SOCK_DGRAM, or a valid SOCKET_xxxx.
 * @param proto : IPPROTO_UDP, IPPROTO_TCP, or a valid IPPROTO_xxxx.
 */
Socket::Socket(int domain, int type, int proto) : Socket()
{
   mDomain = domain;
   mType = type;
   mProto = proto;
   mSock = INVALID_SOCKET;
   mAddr.sa.sa_family = domain;
}

Socket::~Socket()
{
   close();
#ifdef OS_WINDOWS
   WSACleanup();
#endif
}

/**
 * @brief Create the underlying socket
 * 
 * @return SOCKET, a socket handle or INVALID_SOCKET
 */
SOCKET Socket::open() noexcept
{
   mSock = socket(mAddr.sa.sa_family, mType, mProto);
   return mSock;
}

/**
 * @brief Test the underlying socket validity.
 * 
 * @return true : The underlying socket is valid.
 * @return false : The underlying socket is not valid.
 */
bool Socket::isOpen() noexcept
{
   return (mSock != INVALID_SOCKET);
}

/**
 * @brief Close the underlying socket.
 * 
 * @return int : zero on success.
 */
int Socket::close() noexcept
{
   int rc = 0;
   if (mSock != INVALID_SOCKET)
   {
#ifdef OS_WINDOWS
      rc = closesocket(mSock);
#else
      rc = ::close(mSock);
#endif
   }
   mSock = INVALID_SOCKET;
   return rc;
}

/**
 * @brief set the internal sockaddr struct to ANY ADDR according to the domain.
 * 
 * @param domain : AF_INET for IPV4, AF_INET6 for IPV6, AF_UNSPEC for autodetect.
 * @param port : the port to bind
 * @return int : zero on success.
 */
int Socket::setAnyAddr(int domain, uint16_t port) noexcept
{
   mDomain = domain;
   return setAnyAddr(port);
}

/**
 * @brief set the internal sockaddr struct to ANY ADDR according to the socket domain.
 * 
 * @param port : the port to bind
 * @return int : zero on success.
 */
int Socket::setAnyAddr(uint16_t port) noexcept
{
   socketaddr saddr = {};
   saddr.sa.sa_family = mDomain;
   saddr.s4.sin_port = htons(port);

   if (mDomain == AF_INET)
   {
      saddr.s4.sin_addr.s_addr = INADDR_ANY;
      saddr.size = sizeof(saddr.s4);
   }
   else if (mAddr.sa.sa_family == AF_INET6)
   {
      saddr.s6.sin6_addr = in6addr_any;
      saddr.size = sizeof(saddr.s6);
   }
   else
      return -1;

   mAddr = saddr;
   return 0;
}

/**
 * @brief Initialize the internal sockaddr with the given socketaddr.
 * 
 * @param saddr : A filled socketaddr
 * @return int : zero on success.
 */
int Socket::setAddr(const socketaddr &saddr) noexcept
{
   mAddr = saddr;
   return 0;
}

/**
 * @brief Initialize the internal sockaddr with the given IPV4 socket address
 * 
 * @param sa : An IPV4 sockaddr struct
 * @return int : Always zero
 */
int Socket::setAddr(const sockaddr_in &sa) noexcept
{
   mAddr.s4 = sa;
   mAddr.size = sizeof(sa);
   return 0;
}

/**
 @brief Initialize the internal sockaddr with the given IPV6 socket address
 * 
 * @param sa : An IPV6 sockaddr struct
 * @return int : Always zero
 */
int Socket::setAddr(const sockaddr_in6 &sa) noexcept
{
   mAddr.s6 = sa;
   mAddr.size = sizeof(sa);
   return 0;
}

#ifdef OS_UNIX
int Socket::setAddr(const sockaddr_ll& sa) noexcept
{
   mAddr.ll = sa;
   mAddr.size = sizeof(sa);
   return 0;
}
#endif

/**
 * @brief Try to convert the tuple node, port to a valid sockaddr.
 *
 * @param domain : AF_INET or AF_INET6.
 * @param node : A hostname.
 * @param port : A valid port number.
 * @return int : zero on success.
 */
int Socket::setAddr(int domain, const std::string &node, uint16_t port) noexcept
{
   mDomain = domain;
   return setAddr(node, port);
}

/**
 * @brief Try to convert the tuple node, port to a valid sockaddr.
 *
 * If the domain isn't specified in the Socket constructor:
 *  - If node param is an IP address, we detect the IP version.
 *  - If node param is a hostname, we use the first valid socket found by getaddrinfo.
 *
 * @param node : An IPV4, IPV6 or a hostname
 * @param port : A valid port number.
 * @return int : zero on success.
 */
int Socket::setAddr(const std::string &node, uint16_t port) noexcept
{
   addrinfo hints = {};
   hints.ai_family = (mDomain == AF_UNSPEC) ? IpAddrDomain(node) : mDomain;
   hints.ai_socktype = mType;
   hints.ai_protocol = mProto;
   hints.ai_flags = AI_PASSIVE;
   hints.ai_canonname = nullptr;
   hints.ai_addr = nullptr;
   hints.ai_next = nullptr;

   auto sAddr = SockAddr(node, port, hints);
   if (sAddr.size == 0)
      return -1;

   mAddr = sAddr;
   mDomain = hints.ai_family;
   return 0;
}

/**
 * @brief Bind the underlying socket with the configured sockaddr.
 * 
 * @return int : zero on success.
 */
int Socket::bind() noexcept
{
   return ::bind(mSock, &mAddr.sa, mAddr.size);
}

/**
 * @brief Retrieve the underlying socket port.
 *
 * When you bind a socket with port 0, the created socket will use a random port.
 * getPort will retrieve that random port number.
 * 
 * @return uint16_t : the sockaddr port
 */
uint16_t Socket::getPort() const
{
   sockaddr sa;
   socklen_t size = sizeof(sa);
   if (getsockname(mSock, &sa, &size) != 0)
   {
      throw std::system_error(error(), std::system_category(), "getsockname failed");
   }

   if (sa.sa_family == AF_INET)
      return ntohs(((sockaddr_in *)&sa)->sin_port);
   else if (sa.sa_family == AF_INET6)
      return ntohs(((sockaddr_in6 *)&sa)->sin6_port);
   else
      throw std::runtime_error("getsockname failed : unsupported domain");
}

/**
 * @brief Readback the socket address.
 *
 * @return socketaddr : the current socketaddr value.
 */
socketaddr Socket::getSocketaddr() const noexcept
{
   return mAddr;
}

/**
 * @brief This method encapsulates errno under unix and WSAGetLastError under Windows.
 * 
 * @return int : errno value or WSAGetLastError value
 */
int Socket::error() const
{
#ifdef OS_UNIX
   return errno;
#elif defined OS_WINDOWS
   return WSAGetLastError();
#endif
}

/**
 * @brief This method encapsulates the setsockopt function.
 * 
 * @return int : zero on success.
 */
int Socket::setOption(int level, int option_name, const void *option_value, int option_len) noexcept
{
   return setsockopt(mSock, level, option_name, CPCHAR_WSCAST(option_value), (socklen_t)option_len);
}

/**
 * @brief This method encapsulates the getsockopt function.
 * 
 * @return int : zero on success.
 */
int Socket::getOption(int level, int option_name, void *option_value, int *option_len) noexcept
{
   return getsockopt(mSock, level, option_name, PCHAR_WSCAST(option_value), (socklen_t *)option_len);
}

/**
 * @brief Set a receive time out.
 * 
 * @param s : number of second(s).
 * @param ms : number of milli second(s).
 * @return int : zero on success.
 */
int Socket::setRecvTimeout(uint32_t s, uint32_t ms) noexcept
{
   struct timeval timeout = {};
   timeout.tv_sec = s;
   timeout.tv_usec = ms * 1000;

   return setsockopt(mSock, SOL_SOCKET, SO_RCVTIMEO, CPCHAR_WSCAST(&timeout), sizeof(timeout));
}

/**
 * @brief Set a send time out.
 * 
 * @param s : number of second(s).
 * @param ms : number of milli second(s).
 * @return int : zero on success.
 */
int Socket::setSendTimeout(uint32_t s, uint32_t ms) noexcept
{
   struct timeval timeout = {};
   timeout.tv_sec = s;
   timeout.tv_usec = ms * 1000;

   return setsockopt(mSock, SOL_SOCKET, SO_SNDTIMEO, CPCHAR_WSCAST(&timeout), sizeof(timeout));
}

/**
 * @brief Get the send/sendto flags bitmask
 * 
 * @return int : The send flags bitmask
 */
int Socket::getSendFlags() noexcept
{
   return mSendFlags;
}

/**
 * @brief Get the recv/recvfrom flags bitmask.
 * 
 * @return int : The receive flags bitmask.
 */
int Socket::getRecvFlags() noexcept
{
   return mRecvFlags;
}

/**
 * @brief Add a bit ot the current send bitmask.
 * 
 * @param value : A valid send/sendto flag.
 */
void Socket::setSendFlag(int value) noexcept
{
   mSendFlags |= value;
}

/**
 * @brief Add a bit to current receive bitmask.
 * 
 * @param value : A valid recv/recvfrom flag.
 */
void Socket::setRecvFlag(int value) noexcept
{
   mRecvFlags |= value;
}

/**
 * @brief Set the send/sendto bitmask.
 * 
 * @param value : A valid send/sendto flags bitmask.
 */
void Socket::setSendFlags(int value) noexcept
{
   mSendFlags = value;
}

/**
 * @brief Set the recv/recvfrom bitmask.
 * 
 * @param value : A valid recv/recvfrom flags bitmask.
 */
void Socket::setRecvFlags(int value) noexcept
{
   mRecvFlags = value;
}

/**
 * @brief Remove a bit from the current send bitmask.
 * 
 * @param value : A valid send/sendto flag.
 */
void Socket::resetSendFlag(int value) noexcept
{
   mSendFlags &= ~value;
}

/**
 * @brief Remove a bit from the current receive bitmask.
 * 
 * @param value : A valid recv/recvfrom flag.
 */
void Socket::resetRecvFlag(int value) noexcept
{
   mRecvFlags &= ~value;
}

/**
 * @brief Set the current send bitmask to zero.
 * 
 */
void Socket::resetSendFlags() noexcept
{
   mSendFlags = 0;
}

/**
 * @brief Set the current receive bitmask to zero.
 * 
 */
void Socket::resetRecvFlags() noexcept
{
   mRecvFlags = 0;
}

/**
 * @brief Scatter / Gather
 *
 * This method encapsulates the sendmsg BSD system call and
 * the winsoc WSASendMsg. 
 *
 * The winsoc API does not use the msghdr struct,
 * thus we have to translate msghdr struct to WSAMSG struct.
 * To avoid dynamic allocation, we use a fix array of 10 WSABUFF.
 * You can override this WSABUFF_ARRAY_SIZE in the cmake cache.
 * 
 * @param msg : the buffers collection to send.
 * @return int : zero on success.
 */
int Socket::send(const msghdr &msg) const noexcept
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

/**
 * @brief Scatter / Gather
 *
 * This method encapsulates the recvmsg BSD system call and
 * the winsoc WSARecvFrom. 
 *
 * The winsoc API does not use the msghdr struct,
 * thus we have to translate msghdr struct to WSAMSG struct.
 * To avoid dynamic allocation, we use a fix array of 10 WSABUFF.
 * You can override this WSABUFF_ARRAY_SIZE in the cmake cache.
 * 
 * @param msg : the buffers collection.
 * @return int : zero on success.
 */
int Socket::recv(msghdr &msg) noexcept
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
