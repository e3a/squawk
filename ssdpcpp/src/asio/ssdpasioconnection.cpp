/*
    ASIO ssdp connection implmementation.

    Copyright (C) 2015  <etienne> <e.knecht@netwings.ch>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ssdpasioconnection.h"
#include <mimetypes.h>
#include <http.h>

#include <thread>
#include <future>

namespace ssdp {
namespace asio {

void SSDPAsioConnection::set_handler(SSDPCallback * _handler) {
  handler = _handler;
}

SSDPAsioConnection::SSDPAsioConnection( std::string listen_address, std::string multicast_address, int multicast_port )
  : io_service(), strand_(io_service), socket(io_service), multicast_address(multicast_address), multicast_port(multicast_port) {
  
  ::asio::ip::address _multicast_address = ::asio::ip::address::from_string(multicast_address);
  ::asio::ip::address _listen_address = ::asio::ip::address::from_string(listen_address);

  // Create the socket so that multiple may be bound to the same address.
  ::asio::ip::udp::endpoint listen_endpoint(_listen_address, multicast_port);
  socket.open(listen_endpoint.protocol());
  socket.set_option(::asio::ip::udp::socket::reuse_address(true));
  socket.bind(listen_endpoint);

  // Join the multicast group.
  socket.set_option(
      ::asio::ip::multicast::join_group(_multicast_address));

  socket.async_receive_from(::asio::buffer(data, max_length), sender_endpoint,
      strand_.wrap(
        std::bind(&SSDPAsioConnection::handle_receive_from, this,
          std::placeholders::_1,
          std::placeholders::_2)));
}

void SSDPAsioConnection::start() {
  ssdp_runner = std::unique_ptr<std::thread>(new std::thread(
    std::bind(static_cast<size_t (::asio::io_service::*)()>( &::asio::io_service::run ), &io_service ) ) );
}

void SSDPAsioConnection::stop() {
  io_service.stop();
  ssdp_runner->join();
}

inline std::string create_header(std::string request_line, std::map< std::string, std::string > headers) {
  std::ostringstream os;
  os << request_line + std::string("\r\n");

  for(auto & iter : headers) {
    os << iter.first << ": " << iter.second << "\r\n";
  }
  os << "\r\n";
  return os.str();
};

void SSDPAsioConnection::send(std::string request_line, std::map< std::string, std::string > headers) {

  std::string message = create_header(request_line, headers);

  ::asio::io_service io_service_;
  ::asio::ip::udp::endpoint endpoint(::asio::ip::address::from_string(multicast_address.c_str()), multicast_port);
  ::asio::ip::udp::socket socket(io_service_, endpoint.protocol());
  socket.send_to(
    ::asio::buffer(message, message.length()), endpoint);
}

void SSDPAsioConnection::send(Response response) {
  std::string buffer = create_header(response.request_line, response.headers);
  socket.send_to(
      ::asio::buffer(buffer, buffer.length()), sender_endpoint);
}

void SSDPAsioConnection::handle_receive_from(const ::asio::error_code & error, size_t bytes_recvd) {
  if (!error) {
    http::HttpRequest request;
    request.client_ip = sender_endpoint.address().to_string();
    http::HttpParser::parse_http_request(request, data, bytes_recvd);
    handler->handle_receive(request);

    socket.async_receive_from(::asio::buffer(data, max_length), sender_endpoint,
        strand_.wrap(
          std::bind(&SSDPAsioConnection::handle_receive_from, this,
            std::placeholders::_1,
            std::placeholders::_2)));
  }
}
}}
