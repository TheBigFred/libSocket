# libSocket is a portable low-level socket library.

The purpose of this library is to wrap the BSD & winsock2 API
to hidde difference between the two API.

Winsoc2 API is similare but not fully compatible with the BSD API,
which is a pity because that was its aim.

This library aims:

* To support both IPV4 and IPV6 addresses.
* Never allocate memory dynamically.
* KISS : Keep It Simple Stupid


The API is composed of:

* 3 objects :

   * Socket, the base virtual object that define commons methods.
   * SocketDGRAM, who encapsulate a dagram oriented socket.
   * SocketSTREAM, who encapsulate a stream oriented socket.

* 3 SockAddr() helpers functions, who encapsulate getaddrinfo and help to fillin a sockaddr struct in a IPV4, IPV6 independent way.
   
* 4 helpers methods : IpAddrDomain(), IfName(), IpAddr(), IfIndex().

##How to use it

Create a socketDGRAM or a socketSTREAM. Before opening the socket, the domain must be known.
Either you initialize it in the constructor, either you use one of the setAddr/setAnyAddr methods.

setAddr & setAnyAddr methods tries to guess the socket domain.


##Examples
Udp, broadcast and multicast examples are available in the tests folder.

A send/recv File is available in the unit test file socketSTREAM.cpp in the tests folder.

###UDP receiver example (aka udp server)
```
   #include <libSocket/socketdgram.h>
   ...
   SocketDGRAM sock(AF_INET);
   sock.setAnyAddr(2345);
   sock.open();
   sock.bind();
   sock.recv(...); // wait on AnyAddr on port 2345
   sock.send(...); // send to the emitter of the previous received datagram
```

###UDP sender example (aka udp client)
```
   #include <libSocket/socketdgram.h>
   ...
   SocketDGRAM sock;
   sock.setAddr("127.0.0.1",2345);
   sock.send(...); // send a datagram to 127.0.0.1 on port 2345
   sock.recv(...); // receive a datagram from 127.0.0.1:2345
```

###TCP receiver example (aka tcp server)
```
   #include <libSocket/socketstream.h>
   ...
   SocketSTREAM sock(AF_INET);
   sock.setAnyAddr(2345);
   sock.open();
   sock.bind();
   sock.listen();
   auto wsock = sock.accept(); // wait the client connection

   // wsock means working socket
   wsock.recv(...);
   wsock.send(...);
```

###TCP sender example (aka tcp client)
```
   #include <libSocket/socketstream.h>
   ...
   SocketSTREAM sock;
   sock.setAddr("127.0.0.1",2345);
   sock.connect();
   sock.send(...);
   sock.recv(...);
```
