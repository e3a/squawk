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

#ifndef SSDPASIOCONNECTION_H
#define SSDPASIOCONNECTION_H

#include <array>
#include <memory>
#include <thread>

#include <asio.hpp>

#include "ssdp.h"

namespace ssdp {
namespace asio {

/**
 * ASIO implmentation of the SSDPConnection.
 */
class SSDPAsioConnection : public SSDPConnection {
public:
 /**
  * Create a new SSDPAsioConnection.
  */
  SSDPAsioConnection( const std::string & multicast_address, const int & port );
  virtual ~SSDPAsioConnection() {}
  
 /**
  * Start the server.
  */
  virtual void start();
 /**
  * Stop the server.
  */
  virtual void stop();
 /**
  * Multicast a message to the network.
  */
  virtual void send(std::string request_line, std::map< std::string, std::string > headers);
 /**
  * Send a response to the request host.
  */
  virtual void send(Response response);
 /**
  * Set the callback handler.
  */
  virtual void set_handler(SSDPCallback * handler);
    
private: 
  /* constuctor parameters */
  ::asio::io_service io_service;
  std::string multicast_address;
  int multicast_port;
  ::asio::ip::udp::socket socket;
  ::asio::ip::udp::endpoint sender_endpoint;
  ::asio::io_service::strand strand_;
  
  /* local variables */
  SSDPCallback * handler;  
  enum { max_length = 8192 };
  std::array< char, max_length > data;
  
  /* the runner thread */
  std::unique_ptr<std::thread> ssdp_runner;

  void handle_receive_from(const ::asio::error_code&, size_t bytes_recvd);
};
}}
#endif // SSDPASIOCONNECTION_H
