/*
    asio connection implementation
    Copyright (C) 2014  <e.knecht@netwings.ch>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "asioconnection.h"

#include <utility>
#include <iostream>
#include <vector>
#include "connection_manager.h"

#include "http.h"

namespace http {
namespace asio_impl {

connection::connection ( asio::ip::tcp::socket socket,
						 connection_manager& manager, http::HttpRequestHandler * httpRequestHandler )
	: socket_ ( std::move ( socket ) ),
	  connection_manager_ ( manager ),
	  httpRequestHandler ( httpRequestHandler ) {

	// TODO remove std::ostream * os = new std::ostream(&b);

	reply_ = new ::http::HttpResponse(); //TODO no new needed
	std::cout << "create new connection." << std::endl;
}
connection::~connection() {
	std::cout << "delete connection." << std::endl;
	// delete os;
	delete reply_; //TODO delete with new
}

/* start change threading */
connection::connection ( asio::io_service& io_service,
						 http::HttpRequestHandler * httpRequestHandler )
	: strand_ ( io_service ),
	  socket_ ( io_service ),
	  httpRequestHandler ( httpRequestHandler ) {}
asio::ip::tcp::socket& connection::socket() {
	return socket_;
}
/* end change threading */



void connection::start() {
	do_read();
}

void connection::stop() {
	socket_.close();
}

void connection::do_read() {
	auto self ( shared_from_this() );
	socket_.async_read_some ( asio::buffer ( buffer_ ),
	[this, self] ( std::error_code ec, std::size_t bytes_transferred ) {
		if ( !ec ) {
			http::PARSE_STATE result;

			result = ::http::HttpParser::parse_http_request ( request_, buffer_, bytes_transferred );
			request_.client_ip = socket_.remote_endpoint().address().to_string();

			if ( result == http::PARSE_STATE::TRUE ) {
				httpRequestHandler->handle_request ( request_, *reply_ );
				do_write();

			} else if ( result == http::PARSE_STATE::FALSE ) {
				//TODO HttpResponse::stock_reply(*reply_, http_status::BAD_REQUEST);
				do_write();

			} else {
				do_read();
			}

		} else if ( ec != asio::error::operation_aborted ) {
			connection_manager_.stop ( shared_from_this() );
		}
	} );
}

void connection::do_write() {
	auto self ( shared_from_this() );

	std::vector< asio::const_buffer > buffers;
	std::string response_header = reply_->get_message_header();
	buffers.push_back ( asio::buffer ( response_header.c_str(), response_header.size() ) );
	std::string response_body = reply_->get_message_body();
	buffers.push_back ( asio::buffer ( response_body.c_str(), response_body.size() ) );

	asio::/*async_*/write ( socket_, buffers ); /*,
      [this, self](std::error_code ec, std::size_t) {
    if (!ec) {
      // Initiate graceful connection closure.
      asio::error_code ignored_ec;
      socket_.shutdown(asio::ip::tcp::socket::shutdown_both,ignored_ec);
    }

    if (ec != asio::error::operation_aborted) {
      connection_manager_.stop(shared_from_this());
    }
  }); */
	std::cout << "writen close socket" << std::endl;
	asio::error_code ignored_ec;
	socket_.shutdown ( asio::ip::tcp::socket::shutdown_both, ignored_ec ); //TODO only with sync
	connection_manager_.stop ( shared_from_this() );
}
}
}
