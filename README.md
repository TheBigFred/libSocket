# libSocket is a portable low-level socket library.

The purpose of this library is to wrap the BSD & winsock2 API
to hidde difference between the two API.

Winsoc2 API is similare but not fully compatible with the BSD API,
which is a pity because that was its aim.

This library aims:

* To support both IPV4 and IPV6 addresses.
* Never allocate memory dynamically.
* KISS : Keep It Simple Stupid.

This library is tested on

* Linux (Ubuntu 20.04)
* MacOSX
* Windows with Visual Studio Community 2019


## How to build

This library use cmake.

* Either you build and install this lib
```
   git clone https://github.com/TheBigFred/libSocket.git
   mkdir build
   cmake -S libSocket -B build
   make -j4
   sudo make install
```

   The default install dir is /usr/local. You can change it by adding _-DCMAKE_INSTALL_PREFIX:PATH="your install path"_ at the end of the cmake command line.

   The lib provide a cmake package and is found with find_package(libSocket). Available targets are: _Socket-static_ and _Socket-shared_.

* Either you include the sources in a subfolder of your project and you include it with an _add_subdirectory_.
   The libSocket CMakeLists is written carrefully to be added in a parent CMakeLists.

Two cmake options are available:

* -DENABLE_DOC_libSocket:BOOL=ON : add the make target libSocket-doxygen which generate the doxygen documentation.
* -DENABLE_TEST_libSocket:BOOL=ON : enable the unitests and examples build.
