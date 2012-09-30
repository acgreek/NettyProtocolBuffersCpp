//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include "test.pb.h"
#include "nettyprotocolbuffers.hpp"

double bytes_read=0;
double bytes_written=0;

boost::chrono::high_resolution_clock::time_point startTime;
boost::chrono::high_resolution_clock::time_point endTime ;

using boost::asio::ip::tcp;

class client
{
	public:
		client(boost::asio::io_service& io_service,
				const std::string& server, const std::string& port)
			: resolver_(io_service),
			socket_(io_service),resultMessage_(), nettypbserializer_(socket_)

	{
		// Start an asynchronous resolve to translate the server and service names
		// into a list of endpoints.
		tcp::resolver::query query(server, port);
		resolver_.async_resolve(query, boost::bind(&client::handle_resolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
	}

	private:
		void handle_resolve(const boost::system::error_code& err,
				tcp::resolver::iterator endpoint_iterator)
		{
			if (!err)
			{
				// Attempt a connection to each endpoint in the list until we
				// successfully establish a connection.
#if BOOST_VERSION < 104801
				tcp::endpoint endpoint = *endpoint_iterator;
				socket_.async_connect(endpoint, boost::bind(&client::handle_connect, this, boost::asio::placeholders::error,++endpoint_iterator));

#else 
				boost::asio::async_connect(socket_, endpoint_iterator, boost::bind(&client::handle_connect, this, boost::asio::placeholders::error));
#endif 
			}
			else
			{
				std::cout << "Error: " << err.message() << "\n";
			}
		}


#if BOOST_VERSION < 104801
		void handle_connect(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator)
#else 
			void handle_connect(const boost::system::error_code& err)
#endif 
			{
				if (!err)
				{
					startTime = boost::chrono::high_resolution_clock::now();

					// The connection was successful. Send the request.
					sendMessage();
					nettypbserializer_.async_read(resultMessage_, boost::bind(&client::handle_read_headers, this, boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
				}
				else
				{
					std::cout << "Error: " << err.message() << "\n";
				}
			}
		void sendMessage() {
//			std::cout << "sent" << std::endl;
			TestMessage input;

			input.set_foo("blabal");
			nettypbserializer_.async_write(input, boost::bind(&client::handle_write_request, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred )); 
			//sleep(1);
			
		}

		void handle_write_request(const boost::system::error_code& err, size_t written)
		{
			if (!err)
			{
				bytes_written+= written;
				sendMessage();
			}
			else
			{
				std::cout << "Error: " << err.message() << "\n";
			}
		}

		void handle_read_status_line(const boost::system::error_code& err)
		{
			if (!err)
			{


//				std::cout << "received" << std::endl;
				// Read the response headers, which are terminated by a blank line.
				nettypbserializer_.async_read(resultMessage_, boost::bind(&client::handle_read_headers, this, boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
			}
			else
			{
				std::cout << "Error: " << err << "\n";
			}
		}

		void handle_read_headers(const boost::system::error_code& err,std::size_t size)
		{
			if (!err)
			{
				bytes_read+= size;
				handle_read_status_line(err);
			}
			else
			{
				std::cout << "Error: " << err << "\n";
			}
		}

		void handle_read_content(const boost::system::error_code& err)
		{
			if (!err)
			{
				// Write all of the data that has been read so far.
				std::cout << &response_;

				// Continue reading remaining data until EOF.
				boost::asio::async_read(socket_, response_,
						boost::asio::transfer_at_least(1),
						boost::bind(&client::handle_read_content, this,
							boost::asio::placeholders::error));
			}
			else if (err != boost::asio::error::eof)
			{
				std::cout << "Error: " << err << "\n";
			}
		}

		tcp::resolver resolver_;
		tcp::socket socket_;
		boost::asio::streambuf request_;
		boost::asio::streambuf response_;

		TestMessage resultMessage_;
		NettyProtocolBuffersSocket<boost::asio::ip::tcp::socket> nettypbserializer_;
};

int main(int argc, char* argv[])
{
	endTime=startTime =boost::chrono::high_resolution_clock::now();
	try
	{
		if (argc != 3)
		{
			std::cout << "Usage: async_client <server> <path>\n";
			std::cout << "Example:\n";
			std::cout << "  async_client www.boost.org /LICENSE_1_0.txt\n";
			return 1;
		}

		boost::asio::io_service io_service;
		client c(io_service, argv[1], argv[2]);
		io_service.run();
		endTime =boost::chrono::high_resolution_clock::now();
		typedef boost::chrono::duration<int,boost::milli> millisecs_t ;
		typedef boost::chrono::duration<int,boost::micro> microsecs_t ;
		typedef boost::chrono::duration<double> secs_t ;
		millisecs_t lMDuration( boost::chrono::duration_cast<millisecs_t>(endTime-startTime) ) ;
		microsecs_t lUDuration( boost::chrono::duration_cast<millisecs_t>(endTime-startTime) ) ;
		secs_t lDuration( boost::chrono::duration_cast<millisecs_t>(endTime-startTime) ) ;

		std::cout << "Time spent " << lUDuration.count() << " [us] " << std::endl;
		std::cout << "Time spent " << lMDuration.count() << " [ms] " << std::endl;
		std::cout << "Time spent " << lDuration.count() << " [s] " << std::endl; 

		std::cout << std::fixed<< "bytes_read/msec " << bytes_read/ lDuration.count() << " [s] " << std::endl; 
		std::cout << std::fixed<<"bytes_written/msec " << bytes_written/ lDuration.count() << " [s] " << std::endl; 

	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
	}

	return 0;
}


