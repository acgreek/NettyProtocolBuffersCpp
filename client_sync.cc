//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include "test.pb.h"
#include "nettyprotocolbuffers.hpp"


using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    tcp::socket s(io_service);
#if BOOST_VERSION < 104801
    s.connect(*iterator);
#else 
    boost::asio::connect(s, iterator);
#endif 
    NettyProtocolBuffersSocket<boost::asio::ip::tcp::socket> nettypbserializer(s);
    

    using namespace std; // For strlen.
    std::cout << "Enter message: " <<std::endl;
    TestMessage input;
    input.set_foo("blabal");

    nettypbserializer.write(input);

    TestMessage output;
    nettypbserializer.read(output);
    std::cout << "Reply is: -";
    std::cout << output.foo();
    std::cout << "-\n";
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
