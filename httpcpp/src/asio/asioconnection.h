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

#include <array>
#include <memory>
#include <asio.hpp>
#include <asio/steady_timer.hpp>

#include "http.h"
#include "httprequestparser.h"

namespace http {
namespace asio_impl {

/**
 * @brief Represents a single connection from a client.
 */
class connection : public std::enable_shared_from_this<connection> {
public:
	/**
	 * @brief Construct a connection with the given io_service.
	 * @param io_service
	 * @param httpRequestHandler
	 */
	explicit connection ( asio::io_service& io_service, http::HttpRequestHandler * httpRequestHandler );

	/**
	 * Delete the Connection.
	 */
	~connection();

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
	http::HttpRequestHandler * httpRequestHandler;
	/** the HttpResponse pointer */
	http::response_ptr httpResponse_;




	/// Handle completion of a read operation.
	void handle_read_header ( const asio::error_code& e,
							  std::size_t bytes_transferred );
	void handle_read_body ( const asio::error_code& e,
							std::size_t bytes_transferred );

	/// Handle completion of a write operation.
	void handle_write ( /* char * buffer, */ const asio::error_code& e, int bytes_transferred );

	void send_response();




	/// Buffer for incoming data.
	std::array<char, BUFFER_SIZE> buffer_;

	// asio::streambuf response_;

	size_t body_read = 0;
	/// The incoming request.
	http::HttpRequest request_;
	HttpRequestParser _http_parser;

	void timer_expired ( const asio::error_code & error );
};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace asio_impl
} // namespace http

#endif // HTTP_CONNECTION_HPP
