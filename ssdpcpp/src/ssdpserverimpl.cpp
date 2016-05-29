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
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/utsname.h>

#include <boost/algorithm/string.hpp>

#include "ssdp.h"
#include "ssdpserverconnection.h"
#include "ssdpclientconnection.h"

namespace ssdp {

inline std::string uname() {
  struct utsname uts;
  uname(&uts);
  std::ostringstream system;
  system << uts.sysname << "/" << uts.version;
  return system.str();
};
inline std::string time_string() {
  time_t rawtime;
  time (&rawtime);
  std::string str_time = std::string( std::ctime ( &rawtime ) );
  boost::trim( str_time );
  return str_time;
};

SSDPServerImpl::SSDPServerImpl ( const std::string & uuid, const std::string & multicast_address, const int & multicast_port ) :
	uuid ( uuid ), multicast_address ( multicast_address ), multicast_port ( multicast_port ) {
}

void SSDPServerImpl::start() {
	//start the server
    connection = std::unique_ptr<SSDPServerConnection> (
                     new SSDPServerConnection ( multicast_address, multicast_port ) );
	connection->set_handler ( this );
	connection->start();

	//start reannounce thread
	announce_thread_run = true;
	annouceThreadRunner = std::unique_ptr<std::thread> (
                    new std::thread ( &SSDPServerImpl::annouceThread, this ) );
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
void SSDPServerImpl::handle_response ( http::HttpResponse & response ) {
    if ( response.status() == http::http_status::OK ) {
		if ( response.parameter ( UPNP_HEADER_USN ).find ( uuid ) == string::npos ) {
//TODO			upnp_devices[response.parameter ( UPNP_HEADER_USN )] = parseResponse ( response );
			fireEvent ( SSDPEventListener::ANNOUNCE, response.remote_ip, parseResponse ( response ) );
        }//fi no self announcement
	}
}
void SSDPServerImpl::handle_receive ( http::HttpRequest & request ) {
    // do not process own messages received over other interface
    if ( request.parameter( UPNP_HEADER_USN ).find ( uuid ) == string::npos ) {

        if ( request.method() == REQUEST_METHOD_MSEARCH ) {
            if ( request.parameter( UPNP_HEADER_ST ) == NS_ROOT_DEVICE || request.parameter( UPNP_HEADER_ST ) == UPNP_NS_ALL ) {
                for ( auto & iter : namespaces ) {
                    Response response ( Response::ok, HTTP_REQUEST_LINE_OK, create_response ( iter.first, iter.second ) );
                    connection->send ( response );
                }

            } else if ( namespaces.find ( request.parameter( UPNP_HEADER_ST ) ) != namespaces.end() ) {
                connection->send ( Response ( Response::ok, HTTP_REQUEST_LINE_OK,
                                              create_response ( request.parameter( UPNP_HEADER_ST ),
                                                      namespaces[request.parameter( UPNP_HEADER_ST ) ] ) ) );
            }

        } else if ( request.method() == REQUEST_METHOD_NOTIFY ) {

            if ( request.parameter( UPNP_HEADER_NTS ) == UPNP_STATUS_ALIVE ) {
//TODO                upnp_devices[ request.parameter( UPNP_HEADER_USN ) ] = parseRequest ( request );
                fireEvent ( SSDPEventListener::ANNOUNCE, request.remoteIp(), parseRequest ( request ) );
            } else {
                fireEvent ( SSDPEventListener::BYE, request.remoteIp(), parseRequest ( request ) );
//TODO                upnp_devices.erase ( request.parameter( UPNP_HEADER_USN ) );
            }

        } else {
            std::cerr << "other response: " << request << std::endl;
        }
    }
}
SsdpEvent SSDPServerImpl::parseRequest ( http::HttpRequest & request ) {
	time_t cache_control = 0;
    if ( request.containsParameter( http::header::CACHE_CONTROL ) ) {
        cache_control = parse_keep_alive( request.parameter( http::header::CACHE_CONTROL ) );
    } else std::cerr << "no cache control in request:" << request << std::endl;
    return SsdpEvent ( request.parameter( http::header::HOST ), request.parameter( UPNP_HEADER_LOCATION ),
                        request.parameter( UPNP_HEADER_NT ), request.parameter( UPNP_HEADER_NTS ),
                        request.parameter( UPNP_HEADER_SERVER ), request.parameter( UPNP_HEADER_USN ),
						std::time ( 0 ), cache_control );
}
SsdpEvent SSDPServerImpl::parseResponse ( http::HttpResponse & response ) {
	time_t cache_control = 0;
    if ( response.containsParameter( http::header::CACHE_CONTROL ) ) {
        cache_control = parse_keep_alive( response.parameter( http::header::CACHE_CONTROL ) );
    } else std::cerr << "no cache control in response:" << response << std::endl;

    return SsdpEvent ( response.parameter ( http::header::HOST ), response.parameter ( UPNP_HEADER_LOCATION ),
                        response.parameter ( UPNP_HEADER_ST ), response.parameter ( UPNP_HEADER_NTS ),
						response.parameter ( UPNP_HEADER_SERVER ), response.parameter ( UPNP_HEADER_USN ),
						std::time ( 0 ), cache_control );
}
void SSDPServerImpl::announce() {
	suppress();
	for ( size_t i = 0; i < NETWORK_COUNT; i++ ) {
		for ( auto & iter : namespaces ) {
			send_anounce ( iter.first, iter.second );
		}
	}
}
void SSDPServerImpl::suppress() {
	for ( size_t i = 0; i < NETWORK_COUNT; i++ ) {
		for ( auto & iter : namespaces ) {
			send_suppress ( iter.first );
		}
	}
}
void SSDPServerImpl::search ( const std::string & service ) {

	std::async ( std::launch::async, [this, &service]() {

		std::map< std::string, std::string > map;
        map[http::header::HOST] = multicast_address + std::string ( ":" ) + std::to_string( multicast_port );
        map[boost::to_upper_copy( UPNP_HEADER_ST )] = service;
        map[boost::to_upper_copy( UPNP_HEADER_MX )] = "2";
        map[boost::to_upper_copy( UPNP_HEADER_MAN )] = UPNP_STATUS_DISCOVER;
        map[http::header::CONTENT_LENGTH] = std::string ( "0" );

		SSDPClientConnection connection ( this, multicast_address, multicast_port );
		connection.send ( SSDP_HEADER_SEARCH_REQUEST_LINE, map );
		std::this_thread::sleep_for ( std::chrono::seconds ( 20 ) );
	} );
}
std::map< std::string, std::string > SSDPServerImpl::create_response ( const std::string & nt, const std::string & location ) {

	std::map< std::string, std::string > map;
    map[http::header::CACHE_CONTROL] = UPNP_OPTION_MAX_AGE + std::to_string( ANNOUNCE_INTERVAL );
	map[UPNP_HEADER_LOCATION] = location;
    map[UPNP_HEADER_SERVER] = uname() + std::string ( " DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0" );
    map[boost::to_upper_copy( UPNP_HEADER_ST )] = nt;
    map[boost::to_upper_copy( UPNP_HEADER_USN )] = std::string ( "uuid:" ) + uuid + std::string ( "::" ) + nt;
    map[boost::to_upper_copy( UPNP_HEADER_EXT )] = "";
    map[http::header::DATE] = time_string();
    map[http::header::CONTENT_LENGTH] = std::string ( "0" );

	return map;
}
void SSDPServerImpl::send_anounce ( const std::string & nt, const std::string & location ) {

	std::map< std::string, std::string > map;
    map[http::header::HOST] = multicast_address + std::string ( ":" ) + std::to_string( multicast_port );
    map[http::header::CACHE_CONTROL] = UPNP_OPTION_MAX_AGE + std::to_string( ANNOUNCE_INTERVAL );
    map[boost::to_upper_copy( UPNP_HEADER_LOCATION )] = location;
    map[boost::to_upper_copy( UPNP_HEADER_SERVER )] = uname() + " " + USER_AGENT;
    map[boost::to_upper_copy( UPNP_HEADER_NT )] = nt;
    map[boost::to_upper_copy( UPNP_HEADER_USN )] = "uuid:" + uuid + "::" + nt;
    map[boost::to_upper_copy( UPNP_HEADER_NTS )] = UPNP_STATUS_ALIVE;
    map[boost::to_upper_copy( UPNP_HEADER_EXT )] = std::string ( "" );
    map[boost::to_upper_copy( UPNP_HEADER_DATE )] = time_string();
    map[http::header::CONTENT_LENGTH] = std::string ( "0" );

	connection->send ( SSDP_HEADER_REQUEST_LINE, map );
}
void SSDPServerImpl::send_suppress ( const std::string & nt ) {

	std::map< std::string, std::string > map;
    map[http::header::HOST] = multicast_address + std::string ( ":" ) + std::to_string( multicast_port );
    map[boost::to_upper_copy( UPNP_HEADER_NT )] = nt;
    map[boost::to_upper_copy( UPNP_HEADER_USN )] = "uuid:" + uuid + "::" + nt;
    map[boost::to_upper_copy( UPNP_HEADER_NTS )] = UPNP_STATUS_BYE;
    map[boost::to_upper_copy( UPNP_HEADER_SERVER )] = uname() + " " + USER_AGENT;
    map[boost::to_upper_copy( UPNP_HEADER_EXT )] = std::string ( "" );
    map[boost::to_upper_copy( UPNP_HEADER_DATE )] = time_string();
    map[http::header::CONTENT_LENGTH] = std::string ( "0" );

	connection->send ( SSDP_HEADER_REQUEST_LINE, map );
}
void SSDPServerImpl::annouceThread() {
    _announce_time = std::chrono::high_resolution_clock::now();

	while ( announce_thread_run ) {

        //check reanounce timer
		auto end_time = std::chrono::high_resolution_clock::now();
        auto dur = end_time - _announce_time;
		auto f_secs = std::chrono::duration_cast<std::chrono::duration<unsigned int>> ( dur );

		if ( f_secs.count() >= ( ANNOUNCE_INTERVAL / 3 ) ) {
			for ( size_t i = 0; i < NETWORK_COUNT; i++ ) {
				for ( auto & iter : namespaces ) {
					send_anounce ( iter.first, iter.second );
				}
			}
            _announce_time = std::chrono::high_resolution_clock::now();
		}
		std::this_thread::sleep_for ( std::chrono::milliseconds ( 5000 ) );
	}
}
void SSDPServerImpl::fireEvent ( SSDPEventListener::EVENT_TYPE type, std::string  client_ip, SsdpEvent device ) const {
	for ( auto & listener : listeners ) {
        listener ( type, client_ip, device );
	}
}
}//namespace ssdp
