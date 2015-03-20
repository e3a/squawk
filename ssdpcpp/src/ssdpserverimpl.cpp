/*
    SSDP Server
    
    Copyright (C) 2015  <etienne> <e.knecht@netwings.ch>

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

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include "ssdp.h"
#include "asio/ssdpasioconnection.h"

namespace ssdp {

log4cxx::LoggerPtr SSDPServerImpl::logger(log4cxx::Logger::getLogger("ssdp.SSDPServer")); //TODO remove

inline bool is_root_requested( ::http::HttpRequest request ) {
    return( request.request_lines[UPNP_HEADER_ST] == NS_ROOT_DEVICE || request.request_lines[UPNP_HEADER_ST] == UPNP_NS_ALL );
}

SSDPServerImpl::SSDPServerImpl(const std::string & uuid, const std::string & local_listen_address, 
			       const std::string & multicast_address, const int & multicast_port) :
  uuid(uuid), local_listen_address(local_listen_address), multicast_address(multicast_address), multicast_port(multicast_port) {
}

void SSDPServerImpl::start() {
  //start the server
    connection = std::unique_ptr<asio::SSDPAsioConnection>( 
      new ssdp::asio::SSDPAsioConnection( local_listen_address, multicast_address, multicast_port) );
    connection->set_handler(this);
    connection->start();

  //start reannounce thread
   announce_thread_run = true;  
   annouceThreadRunner = std::unique_ptr<std::thread>(
    new std::thread( &SSDPServerImpl::annouceThread, this ) );
}
/**
 * Stop the server.
 */
void SSDPServerImpl::stop() {
  //stop reannounce thread
  suppress();
  announce_thread_run = false;      
  annouceThreadRunner->join();
  
  //stop the server
  connection->stop();
}
void SSDPServerImpl::handle_receive(::http::HttpRequest request) {
    if( request.request_method == REQUEST_METHOD_MSEARCH ) {
        if( is_root_requested( request ) ) {
            for(auto & iter : namespaces) {
                Response response(Response::ok, HTTP_REQUEST_LINE_OK, create_response(0, iter.first, iter.second));
                connection->send(response);
            }
        } else if(namespaces.find(request.request_lines[UPNP_HEADER_ST]) != namespaces.end()) {
            connection->send(Response(Response::ok, HTTP_REQUEST_LINE_OK, 
				      create_response(0, request.request_lines[UPNP_HEADER_ST], 
						      namespaces[request.request_lines[UPNP_HEADER_ST]])));
        }

    } else if( request.request_method == REQUEST_METHOD_NOTIFY ) {

        if(request.request_lines[UPNP_HEADER_NTS] == UPNP_STATUS_ALIVE) {
            time_t cache_control = 
	      ( commons::string::starts_with(request.request_lines[HTTP_HEADER_CACHE_CONTROL], UPNP_OPTION_MAX_AGE) ?
		commons::string::parse_string<time_t>(request.request_lines[HTTP_HEADER_CACHE_CONTROL].substr(UPNP_OPTION_MAX_AGE.size() ) ) : 
		0 );
            upnp_devices[request.request_lines[UPNP_HEADER_USN ]] =
                    UpnpDevice(request.request_lines[HTTP_HEADER_HOST], request.request_lines[UPNP_HEADER_LOCATION], 
			       request.request_lines[UPNP_HEADER_NT], request.request_lines[UPNP_HEADER_NTS],
                               request.request_lines[UPNP_HEADER_SERVER], request.request_lines[UPNP_HEADER_USN], 
			       std::time(0), cache_control );
		    
	    fireEvent(SSDPEventListener::ANNOUNCE, request.client_ip, UpnpDevice(
	      request.request_lines[HTTP_HEADER_HOST], request.request_lines[UPNP_HEADER_LOCATION], request.request_lines[UPNP_HEADER_NT], 
	      request.request_lines[UPNP_HEADER_NTS], request.request_lines[UPNP_HEADER_SERVER], request.request_lines[UPNP_HEADER_USN], 
	      std::time(0), cache_control ) );
	  
	} else {
	    fireEvent(SSDPEventListener::BYE, request.client_ip, upnp_devices[request.request_lines[UPNP_HEADER_USN ] ] );
            upnp_devices.erase(request.request_lines[UPNP_HEADER_USN ]);
        }
    }
}
void SSDPServerImpl::announce() {
    suppress();
    for(size_t i=0; i<NETWORK_COUNT; i++) {
        for(auto & iter : namespaces ) {
            send_anounce(iter.first, iter.second);
        }
    }
}
void SSDPServerImpl::suppress() {
    for(size_t i=0; i<NETWORK_COUNT; i++) {
        for(auto & iter : namespaces ) {
            send_suppress(iter.first, iter.second);
        }
    }
}
std::map< std::string, std::string > SSDPServerImpl::create_response(size_t bytes_recvd, std::string nt, std::string location) {

    std::map< std::string, std::string > map;
    map[HTTP_HEADER_CACHE_CONTROL] = UPNP_OPTION_MAX_AGE + commons::string::time_to_string( ANNOUNCE_INTERVAL );
    map[UPNP_HEADER_LOCATION] = location;
    map[UPNP_HEADER_SERVER] = commons::system::uname() + std::string(" DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0"); //TODO
    map[UPNP_HEADER_ST] = nt;
    map[UPNP_HEADER_USN ] = std::string("uuid:") + uuid + std::string("::") + nt;
    map[commons::string::to_upper( UPNP_HEADER_EXT )] = "";
    map["DATE"] = commons::system::time_string();
    map[HTTP_HEADER_CONTENT_LENGTH] = std::string("0");

    return map;
}
void SSDPServerImpl::send_anounce(std::string nt, std::string location) {

    std::map< std::string, std::string > map;
    map[HTTP_HEADER_HOST] = multicast_address + std::string(":") + commons::string::to_string<int>(multicast_port);
    map[HTTP_HEADER_CACHE_CONTROL] = UPNP_OPTION_MAX_AGE + commons::string::time_to_string( ANNOUNCE_INTERVAL );
    map[commons::string::to_upper( UPNP_HEADER_LOCATION )] = location;
    map[commons::string::to_upper( UPNP_HEADER_SERVER)] = commons::system::uname() + " " + USER_AGENT;
    map[commons::string::to_upper( UPNP_HEADER_NT )] = nt;
    map[commons::string::to_upper( UPNP_HEADER_USN )] = "uuid:" + uuid + ":" + nt;
    map[commons::string::to_upper( UPNP_HEADER_NTS )] = UPNP_STATUS_ALIVE;
    map[commons::string::to_upper( UPNP_HEADER_EXT )] = std::string("");
    map[commons::string::to_upper( UPNP_HEADER_DATE )] = commons::system::time_string();
    map[HTTP_HEADER_CONTENT_LENGTH] = std::string("0");

    connection->send(SSDP_HEADER_REQUEST_LINE, map);
}
void SSDPServerImpl::send_suppress(std::string nt, std::string location) {

    std::map< std::string, std::string > map;
    map[HTTP_HEADER_HOST] = multicast_address + std::string(":") + commons::string::to_string<int>(multicast_port);
    map[commons::string::to_upper( UPNP_HEADER_NT )] = nt;
    map[commons::string::to_upper( UPNP_HEADER_USN )] = "uuid:" + uuid + ":" + nt;
    map[commons::string::to_upper( UPNP_HEADER_NTS )] = UPNP_STATUS_BYE;
    map[commons::string::to_upper( UPNP_HEADER_SERVER )] = commons::system::uname() + " " + USER_AGENT;
    map[commons::string::to_upper( UPNP_HEADER_EXT )] = std::string("");
    map[commons::string::to_upper( UPNP_HEADER_DATE )] = commons::system::time_string();
    map[HTTP_HEADER_CONTENT_LENGTH] = std::string("0");

    connection->send(SSDP_HEADER_REQUEST_LINE, map);
}
void SSDPServerImpl::annouceThread() {
  start_time = std::chrono::high_resolution_clock::now();
  while( announce_thread_run ) {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto dur = end_time - start_time;
    auto f_secs = std::chrono::duration_cast<std::chrono::duration<int>>(dur);
    if( f_secs.count() >= (ANNOUNCE_INTERVAL / 3) ) {  
      std::cout  << "reannounce thread:" << f_secs.count() << std::endl; //TODO remove
      //TODO announement refreshs shall be distrubuted over time.
      for(size_t i=0; i<NETWORK_COUNT; i++) {
	  for(auto & iter : namespaces ) {
	      send_anounce(iter.first, iter.second);
	  }
      }
      start_time = std::chrono::high_resolution_clock::now();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
void SSDPServerImpl::fireEvent( SSDPEventListener::EVENT_TYPE type, std::string  client_ip, UpnpDevice device ) {
  for( auto & listener : listeners ) {
    listener->ssdpEvent( type, client_ip, device );
  }
}
}
