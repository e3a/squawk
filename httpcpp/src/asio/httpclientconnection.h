/*
    asio client connection definition
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

#ifndef HTTPCLIENTCONNECTION_H
#define HTTPCLIENTCONNECTION_H

#include "asio.hpp"

#include "http.h"

namespace http {
inline namespace asio_impl {

class HttpClientConnection : public http::IHttpClientConnection {
public:
	HttpClientConnection ( const std::string & server, const std::string & service = "http" );
	virtual ~HttpClientConnection();
	void start ( http::HttpRequest * request, http::HttpResponse * response, std::function<void ( HttpResponse& ) > callback );
private:
	asio::io_service io_service_;
	asio::ip::tcp::resolver resolver_;
	asio::ip::tcp::socket socket_;
	std::string _server;
	HttpRequest * httpRequest_;
	HttpResponse * httpResponse_;
    HttpResponseParser responseParser_;

	asio::streambuf request_;
	asio::streambuf response_;

	void handle_resolve ( const asio::error_code& err,
						  asio::ip::tcp::resolver::iterator endpoint_iterator );
	void handle_connect ( const asio::error_code& err );
	void handle_write_request ( const asio::error_code& err );
	void readHeaders ( const asio::error_code& err, const size_t size );
	void readContent ( const asio::error_code& err, const size_t size );

	/* the runner thread */
	std::unique_ptr<std::thread> client_runner;
	std::unique_ptr<char[]> buffer; //TODO remove
	std::array<char, BUFFER_SIZE> buffer_;
	size_t strream_size;
	// std::shared_ptr<std::istream> stream;
	bool resolve_complete = false;

	std::function<void ( HttpResponse& ) > callback_;
};
} //asio_impl
} //http
#endif // HTTPCLIENTCONNECTION_H
