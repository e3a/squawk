#ifndef HTTPCLIENTCONNECTION_H
#define HTTPCLIENTCONNECTION_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "asio.hpp"

namespace http {
inline namespace asio_impl {

class HttpClientConnection {
public:
	HttpClientConnection ( const std::string & server, const std::string & service = "http" );
	~HttpClientConnection();
	void start ( const std::string & method, const std::string & path,
				 const std::map< std::string, std::string > & headers,
				 const std::map< std::string, std::string > & parameters,
				 const size_t & body_size, std::shared_ptr< std::istream > body );
private:
	static const size_t buffer_size = 8192;
	asio::io_service io_service_;
	asio::ip::tcp::resolver resolver_;
	asio::ip::tcp::socket socket_;
	std::string _server;
	asio::streambuf request_;
	asio::streambuf response_;

	void handle_resolve ( const asio::error_code& err,
						  asio::ip::tcp::resolver::iterator endpoint_iterator );
	void handle_connect ( const asio::error_code& err );
	void handle_write_request ( const asio::error_code& err );
	void handle_read_status_line ( const asio::error_code& err );
	void handle_read_headers ( const asio::error_code& err );
	void handle_read_content ( const asio::error_code& err );

	/* the runner thread */
	std::unique_ptr<std::thread> client_runner;
	std::unique_ptr<char[]> buffer;
	size_t strream_size;
	std::shared_ptr<std::istream> stream;
	bool resolve_complete = false;
};
} //asio_impl
} //http
#endif // HTTPCLIENTCONNECTION_H
