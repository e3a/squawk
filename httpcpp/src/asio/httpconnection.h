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

#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <asio.hpp>
#include <asio/steady_timer.hpp>

#include "http.h"

namespace http {
inline namespace asio_impl {

/**
 * @brief Represents a single connection from a client.
 */
class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
public:
	/**
	 * @brief Construct a connection with the given io_service.
	 * @param io_service
	 * @param httpRequestHandler
	 */
    explicit HttpConnection ( asio::io_service& io_service, http::HttpRequestHandler * httpRequestHandler_ );

	/**
	 * Delete the Connection.
	 */
    ~HttpConnection();

	/**
	 * @brief Get the socket associated with the connection.
	 * @return
	 */
	asio::ip::tcp::socket& socket();

	/**
	 * @brief Start the first asynchronous operation for the connection.
	 */
	void start();

private:
	/** Strand to ensure the connection's handlers are not called concurrently. */
	asio::io_service::strand strand_;
	/** Socket for the connection. */
	asio::ip::tcp::socket socket_;
	/** Timer for the connection timeout. */
	asio::steady_timer timer_;
	/** The handler used to process the incoming request. */
    http::HttpRequestHandler * httpRequestHandler_;
	/** the HttpResponse pointer */
	http::response_ptr httpResponse_;
    /** The incoming request. */
    std::unique_ptr< http::HttpRequest > request_;
    /** Buffer for data. */
    std::array<char, BUFFER_SIZE> buffer_;
    /** the request parser */
    HttpRequestParser http_parser_;

    /** Handle completion of the read header operation. */
    void handle_read_header ( const asio::error_code& e, std::size_t bytes_transferred );
    /** Handle completion of the read body operation. */
    void handle_read_body ( const asio::error_code& e, std::size_t bytes_transferred );
    /** Handle completion of a write operation. */
    void handle_write ( const asio::error_code& e, int bytes_transferred );
    /** the callback method from the respose parser. */
	void send_response();

    /** TODO what is that? */
	size_t body_read = 0;
	void timer_expired ( const asio::error_code & error );
};

typedef std::shared_ptr<HttpConnection> connection_ptr;

} // namespace asio_impl
} // namespace http

#endif // HTTP_CONNECTION_HPP
