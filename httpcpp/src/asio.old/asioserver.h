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
#include <string>
#include <system_error>
#include "asioconnection.h"
#include "connection_manager.h"

#include "http.h"

namespace http {
namespace asio_impl {

class server {
public:
  server(const server&) = delete;
  server& operator=(const server&) = delete;

  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit server(const std::string& address, const std::string& port, http::HttpRequestHandler * httpRequestHandler);

  /// Run the server's io_service loop.
  void run();

private:
  /// Perform an asynchronous accept operation.
  void do_accept(const std::error_code& e);
  void start_accept();

  /// Wait for a request to stop the server.
  void do_await_stop();

  /// The io_service used to perform asynchronous operations.
  asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  asio::ip::tcp::acceptor acceptor_;

  /// The connection manager which owns all live connections.
  connection_manager connection_manager_;

  /// The next socket to be accepted.
  asio::ip::tcp::socket socket_;

  /// The handler for all incoming requests.
  // request_handler request_handler_;
  http::HttpRequestHandler * httpRequestHandler;

  connection_ptr new_connection_;

};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP
