////////////////////////////////////////////////////////////////////////////////
// File      : main.cpp
// Contents  : Unit test entry point
//
// Author(s) : Frederic Gerard - mailfge@gmail.com
// URL       : https://github.com/TheBigFred/libSocket
//
//-----------------------------------------------------------------------------
//  LGPL V3.0 - https://www.gnu.org/licences/lgpl-3.0.txt
//-----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <iostream>
#include <random>
#include <string>

uint16_t port;
uint16_t portOffset;
std::string IfIpAddr;
std::string path;

int main(int argc, char **argv)
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<> distrib(1025, 65500);
   port = distrib(gen);

   ::testing::InitGoogleTest(&argc, argv);

   if (argc < 2)
   {
      std::cout << "Usage: " << argv[0] << " Network_Interface_IP_Address" << std::endl;
      std::cout << "   " << argv[0] << " 192.168.0.1" << std::endl;
      return 1;
   }

   std::string arg0(argv[0]);
   auto found = arg0.find_last_of("/\\");
   path = arg0.substr(0,found);
   
   IfIpAddr.append(argv[1]);

   return RUN_ALL_TESTS();
}
