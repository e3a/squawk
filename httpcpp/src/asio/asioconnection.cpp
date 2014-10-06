/*
    asio connection implementation
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

#include "asioconnection.h"

#include <sys/sendfile.h>

#include <utility>
#include <iostream>
#include <vector>

#include <thread>
#include <future>

#include "http.h"

#define BUFFER_SIZE 8192

namespace http {
namespace asio_impl {

connection::connection(asio::io_service& io_service,
    http::HttpRequestHandler * httpRequestHandler)
  : strand_(io_service),
    socket_(io_service),
    timer_( io_service ),
    httpRequestHandler(httpRequestHandler) {

    reply_= new ::http::HttpResponse(); //TODO no new needed
    std::cout << "create new connection." << std::endl;

}
connection::~connection() {
    std::cout << "delete connection." << std::endl;
    if( socket_.is_open() ) {
        socket_.close();
    }
    delete reply_;
}

asio::ip::tcp::socket & connection::socket() {
  return socket_;
}

void connection::start() {
/*      timer_.expires_from_now( std::chrono::seconds( 30 ) );
      timer_.async_wait( std::bind( &connection::timer_expired, shared_from_this(), std::placeholders::_1 ) ); */

      socket_.async_read_some(asio::buffer(buffer_),
      strand_.wrap(
        std::bind(&connection::handle_read, shared_from_this(),
          std::placeholders::_1,
          std::placeholders::_2)));
}

void connection::handle_read(const asio::error_code& e, std::size_t bytes_transferred) {
    std::cout << "handle read" << std::endl;
  if (!e) {
      http::PARSE_STATE result;

      result = ::http::HttpParser::parse_http_request(request_, buffer_, bytes_transferred);
      request_.client_ip = socket_.remote_endpoint().address().to_string();

      if (result == http::PARSE_STATE::TRUE) {

          httpRequestHandler->handle_request( request_, *reply_,  std::function<void()> ( std::bind( &connection::send_response, shared_from_this() ) ) );

            char * buffer = new char[BUFFER_SIZE];

            std::vector< asio::const_buffer > buffers;
            std::string response_header = reply_->get_message_header();
            buffers.push_back( asio::buffer(response_header.c_str(), response_header.size()) );

            asio::async_write(socket_, buffers,
                strand_.wrap(
                  std::bind(&connection::handle_write, shared_from_this(), buffer, std::placeholders::_1, std::placeholders::_2)));

      } else if (result == http::PARSE_STATE::FALSE) {
        //TODO HttpResponse::stock_reply(*reply_, http_status::BAD_REQUEST);
          std::cout << "ERROR: can not READ request"<< std::endl;
          std::vector< asio::const_buffer > buffers;
          std::string response_header = reply_->get_message_header();
          buffers.push_back( asio::buffer(response_header.c_str(), response_header.size()) );
          /* TODO std::string response_body = reply_->get_message_body(); */
          /* buffers.push_back( asio::buffer(response_body.c_str(), response_body.size()) ); */

      } else {
          std::cout << "READ MORE"<< std::endl;
          socket_.async_read_some(asio::buffer(buffer_),
              strand_.wrap(
                std::bind(&connection::handle_read, shared_from_this(),
                  std::placeholders::_1,
                  std::placeholders::_2)));
      }

} //TODO: what to do on error case?

  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
}

void connection::send_response() {
    std::cout << "callback" << std::endl;
}

void connection::handle_write( char * buffer, const asio::error_code& e, int bytes_transferred ) {
  if ( !e ) {
      int next_size = reply_->fill_buffer(buffer, BUFFER_SIZE);
      if(next_size > 0 ) {

        //write next chunk
          asio::async_write(socket_, asio::buffer(buffer, next_size),
              strand_.wrap(
                std::bind(&connection::handle_write, shared_from_this(), buffer, std::placeholders::_1, std::placeholders::_2)));

      } else if( request_.is_persistent() ) { //TODO case sensitive

          //persistend connection, restart
          delete[] buffer;
          reply_->reset();
          start();

      } else {

          std::cout << "handle write: close" << std::endl;

          delete[] buffer;
          std::cout << "Connection close " << std::endl;
        // Initiate graceful connection closure.
        asio::error_code ignored_ec;
        socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
      }
  } else {
      std::cerr << "error in handle_write: " << e.message() << std::endl;
  }

  // No new asynchronous operations are started. This means that all shared_ptr
  // references to the connection object will disappear and the object will be
  // destroyed automatically after this handler returns. The connection class's
  // destructor closes the socket.
}
void connection::timer_expired(const asio::error_code & error) {
  if (!error) {
    std::cout << "timer expired." <<  std::this_thread::get_id() << std::endl;
    socket_.cancel();
  } else {
      std::cout << "timer expired."<< error.message() << std::endl;
  }
}
}}
