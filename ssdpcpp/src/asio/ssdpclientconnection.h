/*
    ASIO ssdp client connection implmementation.

    Copyright (C) 2015  <etienne> <e.knecht@netwings.ch>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SSDPCLIENTCONNECTION_H
#define SSDPCLIENTCONNECTION_H

#include <string>

#include <asio.hpp>

#include "ssdp.h"

namespace ssdp {
inline namespace asio_impl {

class SSDPClientConnection {
public:
	SSDPClientConnection ( SSDPCallback * handler, const std::string & multicast_address, const int & multicast_port )
		: handler ( handler ), io_service_(), strand_ ( io_service_ ), socket ( io_service_ ), multicast_address ( multicast_address ), multicast_port ( multicast_port ) {
	}

	~SSDPClientConnection() {
		io_service_.stop();
		ssdp_runner->join();
	}

	void send ( const std::string & request_line, const std::map< std::string, std::string > & headers );
	void handle_receive_from ( const asio::error_code&, size_t bytes_recvd );

private:
	SSDPCallback * handler;
	asio::io_service io_service_;
	asio::io_service::strand strand_;
	asio::ip::udp::socket socket;
	std::string multicast_address;
	int multicast_port;
	asio::ip::udp::endpoint sender_endpoint;

	enum { max_length = 8192 };
	std::array< char, max_length > data;

	/* the runner thread */
	std::unique_ptr<std::thread> ssdp_runner;
};
}
}
#endif // SSDPCLIENTCONNECTION_H
