#include "httpclientconnection.h"

#include <iostream>

namespace http {
inline namespace asio_impl {

HttpClientConnection::HttpClientConnection ( const std::string & server, const std::string & service ) :
	io_service_(), resolver_ ( io_service_ ), socket_ ( io_service_ ) {

	buffer = std::unique_ptr<char[]> ( new char[ buffer_size ] );

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

void HttpClientConnection::start ( const std::string & method, const std::string & path,
								   const std::map< std::string, std::string > & headers,
								   const std::map< std::string, std::string > & parameters,
								   const size_t & body_size, std::shared_ptr< std::istream > body ) {

	strream_size = body_size;
	stream = body;

	//wait for the connection
	while ( ! resolve_complete ) {}

	std::cout << "connected" << std::endl;

	std::ostream request_stream ( &request_ );
	request_stream << method << " " << path << " " << "HTTP/1.1\r\n"; //TODO replace protocol and version

	for ( auto header : headers ) {
		request_stream << header.first << ": " << header.second << "\r\n";
	}

	if ( headers.find ( "Host" ) == headers.end() ) {
		request_stream << "Host: " << _server << "\r\n";
	}

	if ( headers.find ( "Content-Length" ) == headers.end() ) {
		request_stream << "Content-Length: " << body_size << "\r\n";
	}

	if ( headers.find ( "Accept" ) == headers.end() ) {
		request_stream << "Accept: */*\r\n";
	}

	request_stream << "\r\n";

	asio::async_write ( socket_, request_,
						std::bind ( &HttpClientConnection::handle_write_request, this,
									std::placeholders::_1 ) );
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
		resolve_complete = true;
		//TODO write
		std::cout << "start readt" << std::endl;
		asio::async_read_until ( socket_, response_, "\r\n",
								 std::bind ( &HttpClientConnection::handle_read_status_line, this,
											 std::placeholders::_1 ) );

	} else {
		std::cout << "Error: " << err.message() << "\n";
	}
}
void HttpClientConnection::handle_write_request ( const asio::error_code& err ) {
	if ( !err ) {

		stream->read ( buffer.get(), buffer_size );
		size_t read_size = stream->gcount();

		if ( read_size > 0 ) {
			asio::async_write ( socket_, asio::buffer ( buffer.get(), read_size ),
								std::bind ( &HttpClientConnection::handle_write_request, this,
											std::placeholders::_1 ) );

			std::cout << "chunk written" << std::endl;
		}
	}

	else
	{
		std::cout << "Error: " << err.message() << "\n";
	}
}

void HttpClientConnection::handle_read_status_line ( const asio::error_code& err ) {
	std::cout << "read status line" << std::endl;

	if ( !err )
	{
		// Check that response is OK.
		std::istream response_stream ( &response_ );
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline ( response_stream, status_message );

		if ( !response_stream || http_version.substr ( 0, 5 ) != "HTTP/" ) {
			std::cout << "Invalid response\n";
			return;
		}

		if ( status_code != 200 ) {
			std::cout << "Response returned with status code ";
			std::cout << status_code << "\n";
			return;
		}

		// Read the response headers, which are terminated by a blank line.
		asio::async_read_until ( socket_, response_, "\r\n\r\n",
								 std::bind ( &HttpClientConnection::handle_read_headers, this,
											 std::placeholders::_1 ) );

	} else {
		std::cout << "Error: " << err << "\n";
	}
}
void HttpClientConnection::handle_read_headers ( const asio::error_code& err ) {
	if ( !err ) {
		// Process the response headers.
		std::istream response_stream ( &response_ );
		std::string header;

		while ( std::getline ( response_stream, header ) && header != "\r" )
		{ std::cout << header << "\n"; }

		std::cout << "\n";

		// Write whatever content we already have to output.
		if ( response_.size() > 0 )
		{ std::cout << &response_; }

		// Start reading remaining data until EOF.
		asio::async_read ( socket_, response_,
						   asio::transfer_at_least ( 1 ),
						   std::bind ( &HttpClientConnection::handle_read_content, this,
									   std::placeholders::_1 ) );

	} else {
		std::cout << "Error: " << err << "\n";
	}
}
void HttpClientConnection::handle_read_content ( const asio::error_code& err ) {
	if ( !err ) {
		// Write all of the data that has been read so far.
		std::cout << &response_;

		// Continue reading remaining data until EOF.
		asio::async_read ( socket_, response_,
						   asio::transfer_at_least ( 1 ),
						   std::bind ( &HttpClientConnection::handle_read_content, this,
									   std::placeholders::_1 ) );

	} else if ( err != asio::error::eof ) {
		std::cout << "Error: " << err << "\n";

	} else {
		std::cout << std::endl << "EOF" << std::endl;
	}
}
} //asio_impl
} //http
