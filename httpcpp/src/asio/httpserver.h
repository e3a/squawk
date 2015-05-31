/*
    asio connection implementation header
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

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <asio.hpp>

#include "http.h"

#include "httpconnection.h"

namespace http {
inline namespace asio_impl {

/**
 * @brief The ASIO server class.
 */
class HttpServer : public IHttpServer {
public:
    HttpServer ( const HttpServer& ) = delete;
    HttpServer& operator= ( const HttpServer& ) = delete;

	/**
	 * @brief Construct the server to listen on the specified TCP address and port.
	 * @param address
	 * @param port
	 * @param httpRequestHandler
	 */
    explicit HttpServer ( const std::string & address, const int & port, http::HttpRequestHandler * httpRequestHandler_ );
    virtual ~HttpServer();

	/**
     * @brief Start the server's io_service loop.
	 */
    virtual void start();
    /**
     * @brief Stop the server's io_service loop.
     */
    virtual void stop();

private:
    /** The io_service used to perform asynchronous operations. */
	asio::io_service io_service_;
    /** Acceptor used to listen for incoming connections. */
	asio::ip::tcp::acceptor acceptor_;
    /** The next socket to be accepted. */
	asio::ip::tcp::socket socket_;
    /** The handler for all incoming requests. */
    http::HttpRequestHandler * httpRequestHandler_;
    /** the new connection */
	connection_ptr new_connection_;
    /** the io_service runner threads */
    std::vector<std::shared_ptr<std::thread> > threads;

    /** Perform an asynchronous accept operation. */
    void do_accept ( const std::error_code& e );
    /** start accept connections */
    void start_accept();
};
} // asio_impl
} // http
#endif // HTTP_SERVER_HPP
