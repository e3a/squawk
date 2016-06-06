/*
    SSDP header file,
    Copyright (C) 2013  <etienne> <e.knecht@netwings.ch>

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

#ifndef SSDPCONNECTION_H
#define SSDPCONNECTION_H

#include <chrono>
#include <functional>
#include <string>
#include <map>
#include <memory>
#include <thread>

#include <boost/lexical_cast.hpp>

#include <http.h>

#include "gtest/gtest_prod.h"

/**
 * \brief SSDP Server Implementation.
 *
 * <h2>About</h2>
 * <p>The Simple Service Discovery Protocol (SSDP) is a network protocol based on the Internet Protocol Suite for advertisement and discovery of
   network services and presence information. SSDP is the basis of the discovery protocol of Universal Plug and Play (UPnP).</p>
 *
 * <ul>
 * <li><a href="https://tools.ietf.org/html/draft-cai-ssdp-v1-03">Expired IETF Internet draft</a></li>
 * <li><a href="http://www.upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf">UPNP Device Architecture 1.1</a></li>
 * </ul>
 *
 * <h2>Usage</h2>
 *
 * <p>Create a server instance.</p>
<pre>
ssdp::SSDPServerImpl * ssdp_server = new ssdp::SSDPServerImpl(uuid, localListenAddress, multicastAddress, multicastPort );
</pre>
 * <ul>
 * <li><strong>uuid:</strong> The device UUID</a></li>
 * <li><strong>localListenAddress:</strong> The local IP to bind to, use 0.0.0.0 for all nics.</a></li>
 * <li><strong>multicastAddress:</strong> The multicast IP address. (239.255.255.250).</a></li>
 * <li><strong>multicastPort</strong> The multicast port. (1900)</a></li>
 * </ul>
 *
 * <p>Register device classes.</p>
 *
<pre>
ssdp_server->register_namespace(namespace, configUri);
</pre>
 *
 * <ul>
 * <li><strong>namespace:</strong> The device namespace.</a></li>
 * <li><strong>localListenAddress:</strong>The URI to the device dscription.</a></li>
 * </ul>
 *
 * <p>Announce the local device.</p>
 *
<pre>
ssdp_server->announce();
</pre>
 *
 * <p>Terminate the local device.</p>
 *
<pre>
ssdp_server->stop();
</pre>
 *
 */
namespace ssdp {

enum class SSDP_EVENT_TYPE { ANNOUNCE, BYE };

static const std::string NS_ROOT_DEVICE = "upnp:rootdevice";
static const std::string NS_MEDIASERVER = "urn:schemas-upnp-org:device:MediaServer:1";
static const std::string NS_CONTENT_DIRECTORY = "urn:schemas-upnp-org:service:ContentDirectory:1";
static const std::string NS_CONNECTION_MANAGER = "urn:schemas-upnp-org:service:ConnectionManager:1";
static const std::string NS_MEDIA_RECEIVER_REGISTRAR = "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1";

/**
 * @brief SSDP event item.
 */
struct SsdpEvent {
public:

    SsdpEvent ( const SsdpEvent& ) = default;
    SsdpEvent ( SsdpEvent&& ) = default;
    SsdpEvent& operator= ( const SsdpEvent& ) = default;
    SsdpEvent& operator= ( SsdpEvent&& ) = default;
    ~SsdpEvent() {}

    /**
      * Create the json stream.
      */
    friend std::ostream& operator<< ( std::ostream& out, const ssdp::SsdpEvent & upnp_device ) {
            out << "{\"host\":\"" << upnp_device.host << "\",\"location\":\"" << upnp_device.location << "\",\"nt\":\"" << upnp_device.nt << "\"," <<
                    "\"nts\":\"" <<  upnp_device.nts << "\",\"server\":\"" << upnp_device.server << "\",\"usn\":\"" << upnp_device.usn << "\"," <<
                    "\"last_seen\":" << upnp_device.last_seen << ",\"cache_control\":" << upnp_device.cache_control << "}";
            return out;
    }

    std::string host;
    std::string location;
    std::string nt;
    std::string nts;
    std::string server;
    std::string usn;

    time_t last_seen;
    time_t cache_control;
};

typedef std::function< void( SSDP_EVENT_TYPE type, std::string  client_ip, SsdpEvent device ) > event_callback_t;

/**
 * @brief The SSDP Response
 */
struct Response {

    Response ( http::http_status status, std::string request_line, std::map< std::string, std::string > headers ) :
        status ( status ), request_line ( request_line ), headers ( headers ) {}

    http::http_status status;
    std::string request_line;
	std::map< std::string, std::string > headers;
};

inline std::string create_header ( std::string request_line, std::map< std::string, std::string > headers ) {
	std::ostringstream os;
	os << request_line + std::string ( "\r\n" );

	for ( auto & iter : headers ) {
		os << iter.first << ": " << iter.second << "\r\n";
	}

	os << "\r\n";
	return os.str();
}
}//namespace ssdp
#endif // SSDPCONNECTION_H
