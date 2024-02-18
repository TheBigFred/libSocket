////////////////////////////////////////////////////////////////////////////////
// File      : poll.h
// Contents  : poll wrapper
//
// Author    : TheBigFred - thebigfred.github@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
// LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "platform.h"

#ifdef OS_UNIX

#include <poll.h>

#elif defined OS_WINDOWS

#include <winsock2.h>
#define nfds_t ULONG

int poll(struct pollfd* fds, nfds_t nfds, int timeout)
{
   return WSAPoll(fds, nfds, timeout);
}

#endif
