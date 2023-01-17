////////////////////////////////////////////////////////////////////////////////
// File      : socketstream.cpp
// Contents  : socket stream implementation
//
// Author(s) : Frederic Gerard - mailfge@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <thread>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>

#include "_endian.h"
#include "socketstream.h"

/**
 * @brief Construct a new SocketSTREAM object
 * 
 * @param domain : AF_INET for IPV4, AF_INET6 for IPV6, AF_UNSPEC for autodetect.
 * @param proto : IPPROTO_TCP, or a valid IPPROTO_xxxx.
 */
SocketSTREAM::SocketSTREAM(int domain, int proto) : Socket(domain, SOCK_STREAM, proto)
{
#ifdef MSG_NOSIGNAL
   mSendFlags = MSG_NOSIGNAL;
#endif
}

/**
 * @brief Construct a new SocketSTREAM object
 * 
 * @param domain : AF_INET for IPV4, AF_INET6 for IPV6, AF_UNSPEC for autodetect.
 * @param type : SOCK_STREAM or valid SOCK_xxxx.
 * @param proto : IPPROTO_TCP, or a valid IPPROTO_xxxx.
 */
SocketSTREAM::SocketSTREAM(int domain, int type, int proto) : Socket(domain, type, proto)
{
#ifdef MSG_NOSIGNAL
   mSendFlags = MSG_NOSIGNAL;
#endif
}

SocketSTREAM::SocketSTREAM(SOCKET wSock, const socketaddr &saddr) : Socket()
{
   mSock = wSock;
   mAddr = saddr;

#ifdef MSG_NOSIGNAL
   mSendFlags = MSG_NOSIGNAL;
#endif
}

/**
 * @brief Define the wait client queue.
 * 
 * @param n : Maximum number of connections that can be queued.
 * @return int : zero on success.
 */
int SocketSTREAM::listen(int n /*=1*/)
{
   return ::listen(mSock, n);
}

void SocketSTREAM::setNONBLOCK(bool on /*=true*/)
{
   if ((on && mNONBLOCK) || (!on && !mNONBLOCK))
      return; // nothing to do

   if (on && !mNONBLOCK)
      mNONBLOCK = true;

   if (!on && mNONBLOCK)
      mNONBLOCK = false;

#ifdef OS_UNIX

   int flags = fcntl(mSock, F_GETFL, 0);

   if (mNONBLOCK)
      flags |= O_NONBLOCK;
   else
      flags &= ~O_NONBLOCK;

   int rc = fcntl(mSock, F_SETFL, flags);
   if (rc == -1)
      throw std::system_error(errno, std::system_category(), "fcntl");

#elif defined OS_WINDOWS

   u_long iMode = 1; // iMode != 0, non-blocking mode is enabled
   if (!mNONBLOCK)
      iMode = 0;

   auto iRes = ioctlsocket(mSock, FIONBIO, &iMode);
   if (iRes != NO_ERROR)
   {
      auto msg = "ioctlsocket set O_NONBLOCK failed: " + std::to_string(iRes);
      throw std::runtime_error(msg);
   }

#endif
}

/**
 * @brief Wait a client connection
 * 
 * @param block : If false, set the O_NONBLOCK flag  on the underlying socket.
 * @return SocketSTREAM : The socket to use to communicate with the client or INVALID_SOCKET.
 */
SocketSTREAM SocketSTREAM::accept(bool block /*=true*/)
{
   setNONBLOCK(!block);
   socketaddr saddr = {};
   SOCKET wSock = ::accept(mSock, &saddr.sa, &saddr.size);
   if (wSock == -1)
   {
#ifndef OS_WINDOWS
      if (errno == EAGAIN)
#else
      if (WSAGetLastError() == WSAEWOULDBLOCK)
#endif
         return SocketSTREAM();
      else
         throw std::system_error(errno, std::system_category(), "accept");
   }

   return SocketSTREAM(wSock, saddr);
}

/**
 * @brief Connect a client to a server
 * 
 * @return int : zero on success.
 */
int SocketSTREAM::connect() noexcept
{
#if !defined(MSG_NOSIGNAL) && defined(SO_NOSIGPIPE)
   int set = 1;
   int rc = setsockopt(mSock, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(set));
   if (rc == -1)
      return -1;
#endif

   return ::connect(mSock, (sockaddr *)&mAddr.sa, mAddr.size);
}

/**
 * @brief Enable sending of keep-alive
 *
 * @return int : zero on success. 
 */
int SocketSTREAM::KeepAlive(bool enable /*=true*/) noexcept
{
   int n = enable;
   return setsockopt(mSock, SOL_SOCKET, SO_KEEPALIVE, (const char *)&n, sizeof(n));
}

int SocketSTREAM::send(const msghdr &message) const noexcept
{
  return Socket::send(message);
}

int SocketSTREAM::send(const void *buffer, uint32_t size) const noexcept
{
   if (buffer == nullptr || size == 0)
      return -1;

   return ::send(mSock, (const char *)buffer, size, mSendFlags);
}

int SocketSTREAM::send(const std::string &binary) const noexcept
{
   return ::send(mSock, binary.data(), INT_WSCAST(binary.size()), mSendFlags);
}

int SocketSTREAM::send(const char *txt) const noexcept
{
   if (txt == nullptr)
      return -1;

   return ::send(mSock, txt, INT_WSCAST(strlen(txt) + 1), mSendFlags);
}

int SocketSTREAM::send(uint8_t data) const noexcept
{
   return ::send(mSock, CPCHAR_WSCAST(&data), sizeof(uint8_t), mSendFlags);
}

int SocketSTREAM::send(uint16_t data) const noexcept
{
   uint16_t d = htons(data);
   return ::send(mSock, CPCHAR_WSCAST(&d), sizeof(uint16_t), mSendFlags);
}

int SocketSTREAM::send(uint32_t data) const noexcept
{
   uint32_t d = htonl(data);
   return ::send(mSock, CPCHAR_WSCAST(&d), sizeof(uint32_t), mSendFlags);
}

int SocketSTREAM::send(uint64_t data) const noexcept
{
   uint64_t d = htonll(data);
   return ::send(mSock, CPCHAR_WSCAST(&d), sizeof(uint64_t), mSendFlags);
}

int SocketSTREAM::send(int8_t data) const noexcept
{
   return ::send(mSock, CPCHAR_WSCAST(&data), sizeof(int8_t), mSendFlags);
}

int SocketSTREAM::send(int16_t data) const noexcept
{
   int16_t d = htons(data);
   return ::send(mSock, CPCHAR_WSCAST(&d), sizeof(int16_t), mSendFlags);
}

int SocketSTREAM::send(int32_t data) const noexcept
{
   int32_t d = htonl(data);
   return ::send(mSock, CPCHAR_WSCAST(&d), sizeof(int32_t), mSendFlags);
}

int SocketSTREAM::send(int64_t data) const noexcept
{
   int64_t d = htonll(data);
   return ::send(mSock, CPCHAR_WSCAST(&d), sizeof(int64_t), mSendFlags);
}

int SocketSTREAM::recv(msghdr &message) noexcept
{
  return Socket::recv(message);
}

int SocketSTREAM::recv(void *buffer, uint32_t size) noexcept
{
   if (buffer == nullptr || size <= 0)
      return -1;

   return ::recv(mSock, PCHAR_WSCAST(buffer), size, mRecvFlags);
}

int SocketSTREAM::recv(uint8_t &data) noexcept
{
   return ::recv(mSock, PCHAR_WSCAST(&data), sizeof(data), mRecvFlags);
}

int SocketSTREAM::recv(uint16_t &data) noexcept
{
   uint16_t d = 0;
   int rc = ::recv(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags);
   data = ntohs(d);
   return rc;
}

int SocketSTREAM::recv(uint32_t &data) noexcept
{
   uint32_t d = 0;
   int rc = ::recv(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags);
   data = ntohl(d);
   return rc;
}

int SocketSTREAM::recv(uint64_t &data) noexcept
{
   uint64_t d = 0;
   int rc = ::recv(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags);
   data = ntohll(d);
   return rc;
}

int SocketSTREAM::recv(int8_t &data) noexcept
{
   return ::recv(mSock, PCHAR_WSCAST(&data), sizeof(data), mRecvFlags);
}

int SocketSTREAM::recv(int16_t &data) noexcept
{
   int16_t d = 0;
   int rc = ::recv(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags);
   data = ntohs(d);
   return rc;
}

int SocketSTREAM::recv(int32_t &data) noexcept
{
   int32_t d = 0;
   int rc = ::recv(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags);
   data = ntohl(d);
   return rc;
}

int SocketSTREAM::recv(int64_t &data) noexcept
{
   int64_t d = 0;
   int rc = ::recv(mSock, PCHAR_WSCAST(&d), sizeof(data), mRecvFlags);
   data = ntohll(d);
   return rc;
}

/**
 * @brief This method provide the sendfile linux system call without cons.
 * 
 * The original linux sendfile fire a SIGPIPE if the connection is broken.
 * The winsock doesn't provide equivalent.
 *
 * @param fileName : The full path of the file to transfert.
 * @param callback : A callback to show transfert progress.
 */
void SocketSTREAM::sendFile(const std::string &fileName, void (*callback)(uint64_t Progress, uint64_t Target) /*=nullptr*/)
{
   int fd = -1;
   if ((fd = ::open((char *)fileName.c_str(), O_RDONLY)) == -1)
      throw std::system_error(errno, std::system_category(), "open file failed");

   struct stat st = {};
   if (stat(fileName.c_str(), &st) == -1)
   {
      ::close(fd);
      throw std::system_error(errno, std::system_category(), "cannot read fileSize");
   }

   uint64_t fileSize = (uint64_t)st.st_size;
   if (send(fileSize) == -1)
   {
      ::close(fd);
      throw std::system_error(errno, std::system_category(), "cannot send fileSize");
   }

   if (fileSize == 0)
   {
      ::close(fd);
      return;
   }

   uint8_t buffer[4096];
   ssize_t nbytes = 0;
   uint64_t sum = 0;

   do
   {
      ssize_t Size = ::read(fd, buffer, 4096);
      if (Size == -1)
      {
         ::close(fd);
         throw std::system_error(errno, std::system_category(), "cannot send fileSize");
      }
      if (Size == 0)
         break;

      bool next = false;
      uint8_t *pbuff = buffer;
      do
      {
         nbytes = ::send(mSock, (const char *)pbuff, UINT_WSCAST((size_t)Size), mSendFlags);
         if (nbytes == -1)
         {
            // Socket is non blocking and would block
#ifndef OS_WINDOWS
            if (errno == EAGAIN)
#else
            if (WSAGetLastError() == WSAEWOULDBLOCK)
#endif
            {
               std::this_thread::sleep_for(std::chrono::microseconds(1000));
               continue;
            }

            ::close(fd);
            throw std::system_error(errno, std::system_category(), "send failed");
         }
         else
         {
            if (nbytes < Size)
            {
               pbuff = buffer + nbytes;
               Size -= nbytes;
            }
            else
               next = true;
         }
      } while (!next);

      sum += (uint64_t)nbytes;
      if (callback)
         callback(sum, fileSize);

   } while (sum < fileSize);
   ::close(fd);
}

/**
 * @brief This method provide the client side of sendFile
 * 
 * @param fileName : The full path of where to store the received file.
 * @param callback : A callback to show transfert progress.
 */
void SocketSTREAM::recvFile(const std::string &fileName, void (*callback)(uint64_t Progress, uint64_t Target) /*=nullptr*/)
{
   int fd = -1;
   if ((fd = ::open((char *)fileName.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0666)) == -1)
      throw std::system_error(errno, std::system_category(), "open file failed");

   uint64_t fileSize = 0;
   if (recv(fileSize) == -1)
   {
      ::close(fd);
      throw std::system_error(errno, std::system_category(), "cannot receive fileSize");
   }

   if (fileSize == 0)
   {
      ::close(fd);
      return;
   }

   char buffer[1500];
   ssize_t nbytes = 0;
   uint64_t sum = 0;

   do
   {
      nbytes = ::recv(mSock, buffer, 1500, mRecvFlags);
      if (nbytes == -1)
      {
#ifndef OS_WINDOWS
         if (errno == EAGAIN)
#else
         if (WSAGetLastError() == WSAEWOULDBLOCK)
#endif
         {
            std::this_thread::sleep_for(std::chrono::microseconds(1000));
            continue;
         }

         ::close(fd);
         throw std::system_error(errno, std::system_category(), "recv failed");
      }
      if (nbytes == 0)
         break;

      bool next = false;
      char *pbuff = buffer;
      do
      {
         auto nb = write(fd, pbuff, (unsigned int)nbytes);
         if (nb == -1)
         {
            ::close(fd);
            throw std::system_error(errno, std::system_category(), "write failed");
         }
         else
         {
            if (nb < nbytes)
            {
               pbuff = buffer + nb;
               nbytes -= nb;
            }
            else
               next = true;
         }
      } while (!next);

      sum += (uint64_t)nbytes;
      if (callback != nullptr)
         callback(sum, fileSize);

   } while (sum < fileSize);

   ::close(fd);
}
