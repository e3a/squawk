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

#include "asioserver.h"
#include <functional>
#include <signal.h>
#include <utility>
#include <thread>

#include "commons.h"

namespace http {
namespace asio_impl {

server::server(const std::string& address, const int & port, http::HttpRequestHandler * httpRequestHandler)
  : io_service_(),
    signals_(io_service_),
    acceptor_(io_service_),
    /* connection_manager_(), */
    socket_(io_service_),
    httpRequestHandler(httpRequestHandler) {

    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
  #if defined(SIGQUIT)
    signals_.add(SIGQUIT);
  #endif // defined(SIGQUIT)
    signals_.async_wait(std::bind(&server::handle_stop, this));

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    asio::ip::tcp::resolver resolver(io_service_);
    asio::ip::tcp::resolver::query query(address, commons::string::to_string( port ) );
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept();


  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
/*  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

  do_await_stop();

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  asio::ip::tcp::resolver resolver(io_service_);
  asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept(); */
}

void server::run() {
  // The io_service::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  // io_service_.run();


// start changed for theading
  // Create a pool of threads to run all of the io_services.
    std::vector<std::shared_ptr<std::thread> > threads;
    for (std::size_t i = 0; i < /* thread_pool_size_ */ 10; ++i) {
      std::shared_ptr<std::thread> thread(new std::thread(
            std::bind(static_cast<size_t (asio::io_service::*)()>( &asio::io_service::run ), &io_service_)));
      threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < threads.size(); ++i)
      threads[i]->join();
    // end changed for theading
}


void server::do_accept(const std::error_code& e) {
    // start changed for theading
    if (!e) {
        new_connection_->start();
      }

      start_accept();
      // end changed for theading


      /*
  acceptor_.async_accept(socket_, [this](std::error_code ec) {
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!acceptor_.is_open()) {
          return;
        }

        if (!ec) {
          connection_manager_.start(std::make_shared<connection>(
              std::move(socket_), connection_manager_, httpRequestHandler));
        }

        do_accept();
      }); */
}

void server::start_accept() {
  new_connection_.reset(new connection(io_service_, httpRequestHandler));
  acceptor_.async_accept(new_connection_->socket(),
      std::bind(&server::do_accept, this,
        std::placeholders::_1 /* error */));
}

void server::handle_stop() {
  io_service_.stop();
}

void server::do_await_stop() {
  signals_.async_wait(
      [this](std::error_code /*ec*/, int /*signo*/) {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_service::run()
        // call will exit.
        acceptor_.close();
        // TODO connection_manager_.stop_all();
      });
}
}}
