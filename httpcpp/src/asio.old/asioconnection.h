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

#include "http.h"

namespace http {
namespace asio_impl {

class connection_manager;

/// Represents a single connection from a client.
class connection : public std::enable_shared_from_this<connection> {
public:
	connection ( const connection& ) = delete;
	connection& operator= ( const connection& ) = delete;

	/// Construct a connection with the given socket.
	explicit connection ( asio::ip::tcp::socket socket,
						  connection_manager& manager, http::HttpRequestHandler * httpRequestHandler );
	~connection();

	/// Start the first asynchronous operation for the connection.
	void start();

	/// Stop all asynchronous operations associated with the connection.
	void stop();

	/* start change threading */
	explicit connection ( asio::io_service& io_service,
						  http::HttpRequestHandler * httpRequestHandler );

	asio::ip::tcp::socket& socket();
	asio::io_service::strand strand_;
	/* end change threading */

private:
	/// Perform an asynchronous read operation.
	void do_read();

	/// Perform an asynchronous write operation.
	void do_write();

	/// Socket for the connection.
	asio::ip::tcp::socket socket_;

	/// The manager for this connection.
	connection_manager& connection_manager_;

	/// The handler used to process the incoming request.
	http::HttpRequestHandler * httpRequestHandler;

	/// Buffer for incoming data.
	std::array<char, 8192> buffer_;

	/// The incoming request.
	http::HttpRequest request_;

	/// The reply to be sent back to the client.
	http::HttpResponse * reply_ = nullptr;
//  std::ostream * os;
//  asio::streambuf b;

};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP
