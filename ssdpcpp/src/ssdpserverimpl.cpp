/*
    SSDP Server
    
    Copyright (C) 2013  <copyright holder> <email>

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
#include "asio/ssdpasioconnection.h" //TODO 



#define UPNP_NS_ALL 		std::string("ssdp:all")

#define UPNP_HEADER_EXT 	std::string("Ext")		/*  TODO. */
#define UPNP_HEADER_USN 	std::string("Usn")		/*  Field value contains Unique Service Name. */
#define UPNP_HEADER_LOCATION	std::string("Location")		/*  Field value contains a URL to the UPnP description of the root device. */
#define UPNP_HEADER_NT		std::string("Nt")		/*  Field value contains Notification Type. */
#define UPNP_HEADER_NTS		std::string("Nts")		/*  Field value contains Notification Sub Type. MUST be ssdp:alive. Single URI. */
#define UPNP_HEADER_ST 		std::string("St")
#define UPNP_HEADER_SERVER	std::string("Server")		/*  Specified by UPnP vendor. String. Field value MUST begin with the following “product tokens” (defined by
								    HTTP/1.1). The first product token identifes the operating system in the form OS name/OS version, the second token
								    represents the UPnP version and MUST be UPnP/1.1, and the third token identifes the product using the form
								    product name/product version. For example, “SERVER: unix/5.1 UPnP/1.1 MyProduct/1.0”. Control points MUST be
								    prepared to accept a higher minor version number of the UPnP version than the control point itself implements. For
								    example, control points implementing UDA version 1.0 will be able to interoperate with devices implementing
								    UDA version 1.1. */
#define SSDP_MULTICAST_IP 	std::string("239.255.255.250")

#define UPNP_OPTION_MAX_AGE 	std::string("max-age=")

#define REQUEST_KEY_METHOD "method"
#define REQUEST_METHOD_MSEARCH "M-SEARCH"
#define REQUEST_METHOD_NOTIFY "NOTIFY"

#define SSDP_HEADER_REQUEST_LINE std::string("NOTIFY * HTTP/1.1")

#define HTTP_PROTOCOL std::string("http://")

//TODO reanounce thread
//TODO cleanup thread
//TODO deregister from network

namespace ssdp {

log4cxx::LoggerPtr SSDPServerImpl::logger(log4cxx::Logger::getLogger("ssdp.SSDPServer"));

inline bool is_root_requested( ::http::HttpRequest request ) {
    return( request.request_lines[UPNP_HEADER_ST] == NS_ROOT_DEVICE || request.request_lines[UPNP_HEADER_ST] == UPNP_NS_ALL );
}

SSDPServerImpl::SSDPServerImpl(const std::string & uuid, const std::string & local_listen_address, const std::string & multicast_address, const int & multicast_port) :
  uuid(uuid), local_listen_address(local_listen_address), multicast_address(multicast_address), multicast_port(multicast_port) {
}

void SSDPServerImpl::start() {
    LOG4CXX_INFO(logger, "Start UPNP Server:" << local_listen_address << ":" << multicast_port << " -> " << multicast_address)
    connection = std::unique_ptr<asio::SSDPAsioConnection>( new ssdp::asio::SSDPAsioConnection(io_service, local_listen_address, multicast_address, multicast_port) );
    connection->set_handler(this);
    io_service.run();
}

void SSDPServerImpl::handle_receive(::http::HttpRequest request) {
    LOG4CXX_TRACE(logger, "handle:receive:" << request)
    if( request.request_method == REQUEST_METHOD_MSEARCH ) {
        LOG4CXX_TRACE(logger, "M-SEARCH: " << ", ST:" << request.request_lines[UPNP_HEADER_ST])

        if( is_root_requested( request ) ) {
            for(std::map< std::string, std::string >::iterator iter = namespaces.begin(); iter != namespaces.end(); ++iter) {
                Response response(Response::ok, HTTP_REQUEST_LINE_OK, create_response(0, iter->first, iter->second));
                connection->send(response);
            }
        } else if(namespaces.find(request.request_lines[UPNP_HEADER_ST]) != namespaces.end()) {
            connection->send(Response(Response::ok, HTTP_REQUEST_LINE_OK, create_response(0, request.request_lines[UPNP_HEADER_ST], namespaces[request.request_lines[UPNP_HEADER_ST]])));
        }

    } else if( request.request_method == REQUEST_METHOD_NOTIFY ) {
        if( logger->isDebugEnabled() ) {
            std::stringstream buffer;
            bool first = true;
            for(std::map< std::string, std::string >::iterator iter = request.request_lines.begin(); iter != request.request_lines.end(); ++iter) {
                if(first) first=false;
                else buffer << ", ";
                buffer << iter->first << "=" << iter->second;
            }
            LOG4CXX_TRACE(logger, "NOTIFY: {" << buffer.str() << "}")
        }

        if(request.request_lines[UPNP_HEADER_NTS] == UPNP_STATUS_ALIVE) {
            time_t cache_control = ( commons::string::starts_with(request.request_lines[HTTP_HEADER_CACHE_CONTROL], UPNP_OPTION_MAX_AGE) ?
                                         commons::string::parse_string<time_t>(request.request_lines[HTTP_HEADER_CACHE_CONTROL].substr(UPNP_OPTION_MAX_AGE.size())) : 0 );
            upnp_devices[request.request_lines[UPNP_HEADER_USN ]] =
                    UpnpDevice(request.request_lines[HTTP_HEADER_HOST], request.request_lines[UPNP_HEADER_LOCATION], request.request_lines[UPNP_HEADER_NT], request.request_lines[UPNP_HEADER_NTS],
                               request.request_lines[UPNP_HEADER_SERVER], request.request_lines[UPNP_HEADER_USN], std::time(0), cache_control );
        } else {
            upnp_devices.erase(request.request_lines[UPNP_HEADER_USN ]);
        }
    } else if( logger->isInfoEnabled() ) {
        std::stringstream buffer;
        bool first = true;
        for(std::map< std::string, std::string >::iterator iter = request.request_lines.begin(); iter != request.request_lines.end(); ++iter) {
            if(first) first=false;
            else buffer << ", ";
            buffer << iter->first << "=" << iter->second;
        }
        LOG4CXX_DEBUG(logger, "UMNKOWN METHOD: {" << buffer.str() << "}" )
    }
}
void SSDPServerImpl::announce() {
    suppress();
    LOG4CXX_DEBUG(logger, "Announce UPNP Server")
    for(size_t i=0; i<NETWORK_COUNT; i++) {
        for(auto & iter : namespaces ) {
            std::cout << "announce:" << iter.first << std::endl;
            send_anounce(iter.first, iter.second);
        }
    }
}
void SSDPServerImpl::suppress() {
    LOG4CXX_DEBUG(logger, "Suppress UPNP Server")
    for(size_t i=0; i<NETWORK_COUNT; i++) {
        for(auto & iter : namespaces ) {
            send_suppress(iter.first, iter.second);
        }
    }
}
std::map< std::string, std::string > SSDPServerImpl::create_response(size_t bytes_recvd, std::string nt, std::string location) {

    std::map< std::string, std::string > map;
    map[HTTP_HEADER_CACHE_CONTROL] = std::string("max-age=1800"); //TODO
    map[UPNP_HEADER_LOCATION] = location;
    map[UPNP_HEADER_SERVER] = commons::system::uname() + std::string(" DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0"); //TODO
    map[UPNP_HEADER_ST] = nt;
    map[UPNP_HEADER_USN ] = std::string("uuid:") + uuid + std::string("::") + nt;
    map["EXT"] = "";
    map["DATE"] = commons::system::time_string();
    map[HTTP_HEADER_CONTENT_LENGTH] = std::string("0");

    return map;
}
void SSDPServerImpl::send_anounce(std::string nt, std::string location) {

    std::map< std::string, std::string > map;
    map[HTTP_HEADER_HOST] = SSDP_MULTICAST_IP + std::string(":") + commons::string::to_string<int>(multicast_port);
    map[HTTP_HEADER_CACHE_CONTROL] = std::string("max-age=1800"); //TODO
    map[commons::string::to_upper( UPNP_HEADER_LOCATION )] = location;
    map[UPNP_HEADER_SERVER] = commons::system::uname() + std::string(" DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0"); //TODO
    map[commons::string::to_upper( UPNP_HEADER_NT )] = nt;
    map[commons::string::to_upper( UPNP_HEADER_USN )] = "uuid:" + uuid + ":" + nt;
    map[commons::string::to_upper( UPNP_HEADER_NTS )] = UPNP_STATUS_ALIVE;
    map["EXT"] = std::string("");
    map["DATE"] = commons::system::time_string();
    map[HTTP_HEADER_CONTENT_LENGTH] = std::string("0");

    connection->send(SSDP_HEADER_REQUEST_LINE, map);
}
void SSDPServerImpl::send_suppress(std::string nt, std::string location) {

    std::map< std::string, std::string > map;
    map[HTTP_HEADER_HOST] = SSDP_MULTICAST_IP + std::string(":") + commons::string::to_string<int>(multicast_port);
    map[commons::string::to_upper( UPNP_HEADER_NT )] = nt;
    map[commons::string::to_upper( UPNP_HEADER_USN )] = "uuid:" + uuid + ":" + nt;
    map[commons::string::to_upper( UPNP_HEADER_NTS )] = UPNP_STATUS_BYE;
    map[UPNP_HEADER_SERVER] = commons::system::uname() + std::string(" DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0"); //TODO
    map["EXT"] = std::string("");
    map["DATE"] = commons::system::time_string();
    map[HTTP_HEADER_CONTENT_LENGTH] = std::string("0");

    connection->send(SSDP_HEADER_REQUEST_LINE, map);
}
}
