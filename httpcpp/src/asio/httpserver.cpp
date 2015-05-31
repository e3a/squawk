/*
    asio server implementation
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

#include "httpserver.h"

namespace http {
inline namespace asio_impl {

HttpServer::HttpServer ( const std::string& address, const int & port, http::HttpRequestHandler * httpRequestHandler )
    : io_service_(), /* signals_ ( io_service_ ), */ acceptor_ ( io_service_ ), socket_ ( io_service_ ), httpRequestHandler_ ( httpRequestHandler ) {

	// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
	asio::ip::tcp::resolver resolver ( io_service_ );
    asio::ip::tcp::resolver::query query ( address, std::to_string( port ) );
	asio::ip::tcp::endpoint endpoint = *resolver.resolve ( query );
	acceptor_.open ( endpoint.protocol() );
	acceptor_.set_option ( asio::ip::tcp::acceptor::reuse_address ( true ) );
	acceptor_.bind ( endpoint );
	acceptor_.listen();

	start_accept();
}

HttpServer::~HttpServer() {}

void HttpServer::start() {
	for ( std::size_t i = 0; i < /* thread_pool_size_ */ 10; ++i ) {
		std::shared_ptr<std::thread> thread ( new std::thread (
				std::bind ( static_cast<size_t ( asio::io_service::* ) () > ( &asio::io_service::run ), &io_service_ ) ) );
		threads.push_back ( thread );
	}
}

void HttpServer::stop() {
    io_service_.stop();
    // Wait for all threads in the pool to exit.
    for ( std::size_t i = 0; i < threads.size(); ++i )
        { threads[i]->join(); }
}

void HttpServer::do_accept ( const std::error_code& e ) {
	if ( !e ) {
		new_connection_->start();
	}

	start_accept();
}

void HttpServer::start_accept() {
    new_connection_.reset ( new HttpConnection ( io_service_, httpRequestHandler_ ) );
	acceptor_.async_accept ( new_connection_->socket(),
                             std::bind ( &HttpServer::do_accept, this,
										 std::placeholders::_1 /* error */ ) );
}
} // asio_impl
} // http
