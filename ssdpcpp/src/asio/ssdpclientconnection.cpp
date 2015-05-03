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

#include "ssdpclientconnection.h"
#include "httpresponseparser.h"

namespace ssdp {
inline namespace asio_impl {

void SSDPClientConnection::send ( const std::string & request_line, const std::map< std::string, std::string > & headers ) {
	std::string message = create_header ( request_line, headers );

	asio::ip::udp::endpoint endpoint ( asio::ip::address::from_string ( multicast_address.c_str() ), multicast_port );
	asio::ip::address _listen_address = asio::ip::address::from_string ( "0.0.0.0" );
	asio::ip::udp::endpoint listen_endpoint ( _listen_address, 65105 ); //TODO does it work without choosing port?

	socket.open ( listen_endpoint.protocol() );
	socket.set_option ( asio::ip::udp::socket::reuse_address ( true ) );
	socket.bind ( listen_endpoint );
	socket.async_receive_from ( asio::buffer ( data, max_length ), sender_endpoint,
								strand_.wrap (
									std::bind ( &SSDPClientConnection::handle_receive_from, this,
											std::placeholders::_1,
											std::placeholders::_2 ) ) );

	ssdp_runner = std::unique_ptr<std::thread> ( new std::thread (
					  std::bind ( static_cast<size_t ( asio::io_service::* ) () > ( &asio::io_service::run ), &io_service_ ) ) );
	std::cout << "SSDPClient started" << std::endl;

    socket.send_to ( asio::buffer ( message, message.length() ), endpoint );
}
void SSDPClientConnection::handle_receive_from ( const asio::error_code & error, size_t bytes_recvd ) {
	if ( !error ) {
		http::HttpResponse response;
		response.remote_ip = sender_endpoint.address().to_string();
        http::HttpResponseParser httpParser; //TODO make global
        httpParser.parse_http_response ( response, data, bytes_recvd );
		handler->handle_response ( response );

		socket.async_receive_from ( asio::buffer ( data, max_length ), sender_endpoint,
									strand_.wrap (
										std::bind ( &SSDPClientConnection::handle_receive_from, this,
												std::placeholders::_1,
												std::placeholders::_2 ) ) );
	}
}
} //asio_impl
} //ssdp
