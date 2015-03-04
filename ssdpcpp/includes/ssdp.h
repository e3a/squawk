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

#include <string>
#include <map>
#include <memory>

#include <commons.h>
#include <http.h>

#include "log4cxx/logger.h"
#include "asio.hpp"

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

/** hot many times the SSDP M-SEARCH and NOTIFY messages are sent. */
static const size_t NETWORK_COUNT = 3;

static const std::string NS_ROOT_DEVICE = "upnp:rootdevice";
static const std::string NS_MEDIASERVER = "urn:schemas-upnp-org:device:MediaServer:1";
static const std::string NS_CONTENT_DIRECTORY = "urn:schemas-upnp-org:service:ContentDirectory:1";
static const std::string NS_CONNECTION_MANAGER = "urn:schemas-upnp-org:service:ConnectionManager:1";
static const std::string NS_MEDIA_RECEIVER_REGISTRAR = "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1";

static const std::string HEADER_DATE = "Date";
static const std::string HTTP_REQUEST_LINE_OK = "HTTP/1.1 200 OK";

static const std::string UPNP_STATUS_ALIVE	= std::string("ssdp:alive");
static const std::string UPNP_STATUS_BYE	= std::string("ssdp:byebye");

/**
 * @brief UPNP device item.
 */
struct UpnpDevice {
  UpnpDevice() {}
  UpnpDevice(std::string host, std::string location, std::string nt, std::string nts, std::string server, std::string usn, time_t last_seen, time_t cache_control) : 
    host(host), location(location), nt(nt), nts(nts), server(server), usn(usn), last_seen(last_seen), cache_control(cache_control) {}
  
  std::string host;
  std::string location;
  std::string nt;
  std::string nts;
  std::string server;
  std::string usn;
  
  time_t last_seen;
  time_t cache_control;
  
  /**
    * Create the json stream.
    */
  friend std::ostream& operator<<(std::ostream& out, const ssdp::UpnpDevice & upnp_device) {
    out << "{\"host\":\"" << commons::string::escape_json(upnp_device.host) << "\",\"location\":\"" << commons::string::escape_json(upnp_device.location) << "\",\"nt\":\"" << commons::string::escape_json(upnp_device.nt) << "\"," <<
       "\"nts\":\"" << commons::string::escape_json(upnp_device.nts) << "\",\"server\":\"" << commons::string::escape_json(upnp_device.server) << "\",\"usn\":\"" << commons::string::escape_json(upnp_device.usn) << "\"," <<
	   "\"last_seen\":" << upnp_device.last_seen << ",\"cache_control\":" << upnp_device.cache_control << "}";
    return out;
  };
};

/**
 * @brief The SSDP Response
 */
struct Response {
  /** Response types */
  enum status_type {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } status;
  Response(status_type status, std::string request_line, std::map< std::string, std::string > headers) : status(status), request_line(request_line), headers(headers) {}
  std::string request_line;
  std::map< std::string, std::string > headers;
};

/**
 * @brief The SSDP Callback definition.
 * The callback method will be invoked when a multicast
 * message from the network is reeceived.
 */
class SSDPCallback {
public:  
/**
 * The SSDP Callback method.
 * \param headers the http request
 * \returns the response headers
 */
  virtual void handle_receive(::http::HttpRequest request) = 0;
};  
/**
 * The SSDP Connection..
 * \param headers the request headers
 * \returns the response headers
 */
class SSDPConnection {
public:
/**
 * Send a message to the network..
 * \param headers the messsage headers
 */
  virtual void send(std::string request_line, std::map< std::string, std::string > headers) = 0;
/**
 * Send a reponse to the sender.
 * \param response the response object
 */
  virtual void send(Response response) = 0;
/**
 * Set the SSDP Handlers.
 * \param handler the handler implementation
 */
  virtual void set_handler(SSDPCallback * handler) = 0;
};
/**
 * SSDP Server.
 */
class SSDPServerImpl : public SSDPCallback {

public:
   /**
    * Create a new SSDPServer.
    */
    explicit SSDPServerImpl(const std::string & uuid, const std::string & local_listen_address,
                            const std::string & multicast_address, const int & multicast_port);
    ~SSDPServerImpl() {}
    /**
     * Announce the services in the network.
     */
    void announce();
    /**
     * Suppress the services in the network.
     */
    void suppress();
    /**
    * Start the server.
    */
    void start();
    /**
    * Stop the server.
    */
    void stop() {
      std::cout << "server stop" << std::endl;
      io_service.stop();
    }
    /**
    * Register an UPNP Service.
    * \param ns the Service namespace
    * \param location the service description URL
    */
    void register_namespace(std::string ns, std::string location) {
        namespaces[ns] = location;
    }
    /**
    * Handle receive callback method..
    * \param headers the request headers
    */
    virtual void handle_receive(::http::HttpRequest request);
    /**
    * Register an UPNP Service.
    * \return map with the upnp devices, the map key is the service UUID
    */
    std::map< std::string, UpnpDevice > get_upnp_devices() {
        return upnp_devices;
    }

private:
    static log4cxx::LoggerPtr logger;
    std::string uuid, local_listen_address, multicast_address;
    int multicast_port;
    std::unique_ptr<SSDPConnection> connection;
    void send_anounce(std::string nt, std::string location);
    void send_suppress(std::string nt, std::string location);
    std::map< std::string, std::string > create_response(size_t bytes_recvd, std::string nt, std::string location);
    std::map< std::string, std::string > namespaces;
    std::map< std::string, UpnpDevice > upnp_devices;
    asio::io_service io_service;
};
}
#endif // SSDPCONNECTION_H
