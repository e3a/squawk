/*
    asio client connection implementation
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

#include "httpclientconnection.h"

namespace http {
inline namespace asio_impl {

HttpClientConnection::HttpClientConnection ( const std::string & server, const std::string & service ) :
	io_service_(), resolver_ ( io_service_ ), socket_ ( io_service_ ) {

	/*
	asio::ip::tcp::resolver::query query ( server, service );
	asio::ip::tcp::resolver::iterator endpoint_iterator = resolver_.resolve ( query );
	asio::ip::tcp::resolver::iterator end;

	// Try each endpoint until we successfully establish a connection.
	asio::error_code error = asio::error::host_not_found;

	while ( error && endpoint_iterator != end ) {
		socket_.close();
		socket_.connect ( *endpoint_iterator++, error );
	}

	if ( error ) { throw error; }

	std::cout << "socket connected" << std::endl;
	*/

	buffer = std::unique_ptr<char[]> ( new char[ BUFFER_SIZE ] );

	asio::ip::tcp::resolver::query query ( server, service );
	resolver_.async_resolve ( query,
							  std::bind ( &HttpClientConnection::handle_resolve, this,
										  std::placeholders::_1,
										  std::placeholders::_2 ) );

	client_runner = std::unique_ptr<std::thread> ( new std::thread (
						std::bind ( static_cast<size_t ( asio::io_service::* ) () > ( &asio::io_service::run ), &io_service_ ) ) );
}
HttpClientConnection::~HttpClientConnection() {
	std::cout << "destroy client" << std::endl;
	io_service_.stop();
	client_runner->join();
}

void HttpClientConnection::start ( HttpRequest * request, HttpResponse * response, std::function<void ( HttpResponse& ) > callback ) {
	httpRequest_ = request;
	httpResponse_ = response;
	callback_ = callback;

	strream_size = request->bodySize();
	// TODO stream = body;

	//wait for the connection
	while ( ! resolve_complete ) {}

	std::cout << "start: connected" << std::endl;

	std::ostream request_stream ( &request_ );
	request_stream << request->method() << " " << request->uri() << " " << "HTTP/1.1\r\n"; //TODO replace protocol and version

	for ( auto header : request->parameterMap() ) {
		request_stream << header.first << ": " << header.second << "\r\n";
	}

	if ( ! request->containsParameter ( header::HOST ) ) {
		request_stream << header::HOST << ": " << _server << "\r\n";
	}

	if ( ! request->containsParameter ( header::CONTENT_LENGTH ) ) {
		request_stream << header::CONTENT_LENGTH << ": " << strream_size << "\r\n";
	}

	if ( ! request->containsParameter ( header::ACCEPT ) ) {
		request_stream << header::ACCEPT << ": */*\r\n";
	}

	request_stream << "\r\n";


//    socket_.send( asio::buffer ( request_.data() ) );

	asio::async_write ( socket_, request_,
						std::bind ( &HttpClientConnection::handle_write_request, this,
									std::placeholders::_1 ) );

	/*	client_runner = std::unique_ptr<std::thread> ( new std::thread (
							std::bind ( static_cast<size_t ( asio::io_service::* ) () > ( &asio::io_service::run ), &io_service_ ) ) );
	*/
	std::cout << "start: written: ii_service_stopped:" << io_service_.stopped() << std::endl;
}

void HttpClientConnection::handle_resolve ( const asio::error_code& err,
		asio::ip::tcp::resolver::iterator endpoint_iterator ) {
	std::cout << "handle resolve" << "\n";

	if ( !err ) {
		asio::async_connect ( socket_, endpoint_iterator,
							  std::bind ( &HttpClientConnection::handle_connect, this,
										  std::placeholders::_1 ) );

	} else {
		std::cout << "Error: " << err.message() << "\n";
	}
}
void HttpClientConnection::handle_connect ( const asio::error_code& err ) {
	if ( !err ) {
		// The connection was successful. Send the request.
		std::cout << "handle connected" << std::endl;
		resolve_complete = true;
		//TODO write

		socket_.async_read_some ( asio::buffer ( buffer_ ), std::bind (
									  &HttpClientConnection::readHeaders, this,
									  std::placeholders::_1, std::placeholders::_2 ) );

	} else {
		std::cout << "Error: " << err.message() << "\n";
	}
}
void HttpClientConnection::handle_write_request ( const asio::error_code& err ) {
	std::cout << "handle_write_request" << std::endl;

	if ( !err ) {

		size_t read_size = 0; //TODO
		/* httpRequest_->outBody()->read ( buffer.get(), buffer_size );
		size_t read_size = httpRequest_->outBody()->gcount(); */

		if ( read_size > 0 ) {
			asio::async_write ( socket_, asio::buffer ( buffer.get(), read_size ),
								std::bind ( &HttpClientConnection::handle_write_request, this,
											std::placeholders::_1 ) );

			std::cout << "chunk written" << std::endl;

		} else {
			std::cout << "start read" << std::endl;
			/*	socket_.async_read_some ( asio::buffer ( buffer_ ), std::bind (
			                                  &HttpClientConnection::readHeaders, this,
			                                  std::placeholders::_1, std::placeholders::_2 ) ); */
		}
	}

	else
	{
		std::cout << "Error: " << err.message() << "\n";
	}
}

void HttpClientConnection::readHeaders ( const asio::error_code& err, const size_t size ) {
	if ( !err ) {
		std::cout << "parse response, size:" << size << ", response size: " << httpResponse_->size() << std::endl;

		size_t position = responseParser_.parse_http_response ( *httpResponse_, buffer_, size );

		size_t content_length = std::stoi ( httpResponse_->parameter ( header::CONTENT_LENGTH ) );
		std::cout << "Response, position:" << position << ", size:" << content_length << std::endl;

		if ( content_length > size - position + httpResponse_->size() ) {
			std::cout << "body not complete." << std::endl;
			( *httpResponse_ ) << std::string ( buffer_.data(), position, size - position );
			socket_.async_read_some ( asio::buffer ( buffer_ ), std::bind (
										  &HttpClientConnection::readContent, this,
										  std::placeholders::_1, std::placeholders::_2 ) );

		} else {
			std::cout << "body complete, size:" << ( size - position ) << std::endl;
			std::string body ( buffer_.data(), position, size - position );
			std::cout << body << std::endl;
		}

	} else {
		std::cout << "Error: " << err << "\n";
	}
}
void HttpClientConnection::readContent ( const asio::error_code& err, const size_t size ) {
	if ( !err ) {
		std::cout << "handle_read_content, size:" << size << ", response size: " << httpResponse_->size() << std::endl;
		( *httpResponse_ ) << std::string ( buffer_.data(), size );

		size_t content_length = std::stoi ( httpResponse_->parameter ( header::CONTENT_LENGTH ) );
		std::cout << "Response, position:" << content_length << ", size:" << httpResponse_->size() << std::endl;

		if ( content_length > httpResponse_->size() ) {
			std::cout << "body not complete." << std::endl;
			socket_.async_read_some ( asio::buffer ( buffer_ ), std::bind (
										  &HttpClientConnection::readContent, this,
										  std::placeholders::_1, std::placeholders::_2 ) );

		} else {
			std::cout << "body complete." << std::endl;
			callback_ ( *httpResponse_ );
		}

	} else if ( err != asio::error::eof ) {
		std::cout << "Error: " << err << "\n";

	} else {
		std::cout << std::endl << "EOF" << std::endl;
	}
}
} //asio_impl
} //http
