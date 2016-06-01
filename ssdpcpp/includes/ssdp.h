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

static const bool DEBUG = true;

/** hot many times the SSDP M-SEARCH and NOTIFY messages are sent. */
static const size_t NETWORK_COUNT = 3;
/** hot many times the SSDP M-SEARCH and NOTIFY messages are sent. */
static const size_t ANNOUNCE_INTERVAL = 1800;

static const std::string USER_AGENT = "DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0";

static const std::string NS_ROOT_DEVICE = "upnp:rootdevice";
static const std::string NS_MEDIASERVER = "urn:schemas-upnp-org:device:MediaServer:1";
static const std::string NS_CONTENT_DIRECTORY = "urn:schemas-upnp-org:service:ContentDirectory:1";
static const std::string NS_CONNECTION_MANAGER = "urn:schemas-upnp-org:service:ConnectionManager:1";
static const std::string NS_MEDIA_RECEIVER_REGISTRAR = "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1";

static const std::string HEADER_DATE = "Date";

/**  Specified by UPnP vendor. String. Field value MUST begin with the following “product tokens” (defined by
     HTTP/1.1). The first product token identifes the operating system in the form OS name/OS version, the second token
     represents the UPnP version and MUST be UPnP/1.1, and the third token identifes the product using the form
     product name/product version. For example, “SERVER: unix/5.1 UPnP/1.1 MyProduct/1.0”. Control points MUST be
     prepared to accept a higher minor version number of the UPnP version than the control point itself implements. For
     example, control points implementing UDA version 1.0 will be able to interoperate with devices implementing
     UDA version 1.1. */
static const std::string UPNP_HEADER_SERVER = "Server";
static const std::string UPNP_HEADER_DATE = "Date";
static const std::string UPNP_HEADER_ST = "St";
/**  Field value contains Notification Sub Type. MUST be ssdp:alive. Single URI. */
static const std::string UPNP_HEADER_NTS = "Nts";
/**  Field value contains Unique Service Name. */
static const std::string UPNP_HEADER_USN = "Usn";
/**  Field value contains a URL to the UPnP description of the root device. */
static const std::string UPNP_HEADER_LOCATION = "Location";
/**  Field value contains Notification Type. */
static const std::string UPNP_HEADER_NT = "Nt";
static const std::string UPNP_HEADER_MX = "Mx";
static const std::string UPNP_HEADER_MAN = "Man";
static const std::string UPNP_HEADER_EXT = "Ext";
static const std::string UPNP_OPTION_MAX_AGE = "max-age=";

static const std::string HTTP_REQUEST_LINE_OK = "HTTP/1.1 200 OK";

static const std::string UPNP_STATUS_DISCOVER	= std::string ( "ssdp:discover" );
static const std::string UPNP_STATUS_ALIVE	= std::string ( "ssdp:alive" );
static const std::string UPNP_STATUS_BYE	= std::string ( "ssdp:byebye" );

static const std::string UPNP_NS_ALL = "ssdp:all";

static const std::string  REQUEST_METHOD_MSEARCH = "M-SEARCH";
static const std::string  REQUEST_METHOD_NOTIFY = "NOTIFY";

static const std::string SSDP_HEADER_REQUEST_LINE = "NOTIFY * HTTP/1.1";
static const std::string SSDP_HEADER_SEARCH_REQUEST_LINE = "M-SEARCH * HTTP/1.1";

inline bool check_timeout(const time_t & last_seen_, const double & cache_control_) {
    double elapsed_time = difftime( std::time ( 0 ), last_seen_ );
    return ( elapsed_time > cache_control_ );
}
inline time_t parse_keep_alive(const std::string & cache_control ) {
    time_t time = 0;
    std::string cache_control_clean = boost::erase_all_copy( cache_control, " " );
    if( cache_control_clean.find( UPNP_OPTION_MAX_AGE ) == 0 ) {
        time = boost::lexical_cast<time_t> ( cache_control_clean.substr ( UPNP_OPTION_MAX_AGE.size() ) );

    } else if( ssdp::DEBUG ) {
        std::cerr << "wrong cache control format: " << cache_control << std::endl;
    }
    return time;
}

/**
 * @brief SSDP event item.
 */
struct SsdpEvent {
public:
    SsdpEvent() {}
    SsdpEvent ( std::string host, std::string location, std::string nt, std::string nts, std::string server, std::string usn, time_t last_seen, time_t cache_control ) :
            host_ ( host ), location_ ( location ), nt_ ( nt ), nts_ ( nts ), server_ ( server ), usn_ ( usn ), last_seen_ ( last_seen ), cache_control_ ( cache_control ) {}

    SsdpEvent ( const SsdpEvent& ) = default;
    SsdpEvent ( SsdpEvent&& ) = default;
    SsdpEvent& operator= ( const SsdpEvent& ) = default;
    SsdpEvent& operator= ( SsdpEvent&& ) = default;
    ~SsdpEvent() {}

    /**
     * @brief host
     * REQUIRED. Field value contains multicast address and port reserved for SSDP by Internet Assigned Numbers Authority (IANA). MUST be 239.255.255.250:1900.
     * If the port number (“:1900”) is omitted, the receiver MUST assume the default SSDP port number of 1900.
     * @return
     */
    std::string host() const { return host_; }
    /**
     * @brief Set Host
     * @param host
     */
    void host(const std::string & host) { host_ = host; }
    /**
     * @brief Description Location
     * REQUIRED. Field value contains a URL to the UPnP description of the root device. Normally the host portion contains a
     * literal IP address rather than a domain name in unmanaged networks. Specified by UPnP vendor. Single absolute URL (see
     * RFC 3986).
     * @return
     */
    std::string location() const { return location_; }
    /**
     * @brief Set Description Location
     * @param location
     */
    void setLocation(const std::string &location) { location_ = location; }
    /**
     * @brief Notification Type
     * @return
     */
    std::string nt() const { return nt_; }
    /**
     * @brief Set Notification Type
     * @param nt
     */
    void setNt(const std::string & nt) { nt_ = nt; }
    /**
     * @brief Notification Sub Type
     * @return
     */
    std::string nts() const { return nts_; }
    /**
     * @brief Set Notification Sub Type
     * REQUIRED. Field value contains Notification Sub Type. MUST be ssdp:alive or ssdp:byebye Single URI.
     * @param nts
     */
    void setNts(const std::string &nts) { nts_ = nts; }
    /**
     * @brief Server
     * REQUIRED. Specified by UPnP vendor. String. Field
     * @return
     */
    std::string server() const { return server_; }
    /**
     * @brief Set Server
     * @param server
     */
    void server(const std::string &server) { server_ = server; }
    /**
     * @brief Unique Service Name
     * REQUIRED. Field value contains Unique Service Name. Identifies a unique instance of a device or service. MUST be one of
     * the following. (See Table  1-1, “Root device discovery messages”, Table  1-2, “Embedded device discovery messages”,
     * and Table  1-3, “Service discovery messages” above.) The prefix (before the double colon) MUST match the value of the U
     * element in the device description. (Section 2, “Description” explains the UDN element.) Single URI.
     * @return
     */
    std::string usn() const { return usn_; }
    /**
     * @brief Set Unique Service Name
     * @param usn
     */
    void usn(const std::string &usn) { usn_ = usn; }
    /**
     * @brief last seen
     * @return
     */
    time_t lastSeen() const { return last_seen_; }
    /**
     * @brief Set last een
     * @param last_seen
     */
    void lastSeen(const time_t &last_seen) { last_seen_ = last_seen; }
    /**
     * @brief Cache Control
     * REQUIRED. Field value MUST have the max-age directive (“max-age=”) followed by an integer that specifies the number of
     * seconds the advertisement is valid. After this duration, control points SHOULD assume the device (or service) is no longer
     * available; as long as a control point has received at least one advertisement that is still valid from a root device, any of its
     * embedded devices or any of its services, then the control point can assume that all are available. The number of seconds
     * SHOULD be greater than or equal to 1800 seconds (30 minutes), although exceptions are defined in the text above. Specified
     * by UPnP vendor. Other directives MUST NOT be sent and MUST be ignored when received.
     * @return
     */
    double cacheControl() const { return cache_control_; }
    /**
     * @brief setCache_control
     * @param cache_control
     */
    void cacheControl(const double &cache_control) { cache_control_ = cache_control; }

    /**
      * Create the json stream.
      */
    friend std::ostream& operator<< ( std::ostream& out, const ssdp::SsdpEvent & upnp_device ) {
            out << "{\"host\":\"" << upnp_device.host_ << "\",\"location\":\"" << upnp_device.location_ << "\",\"nt\":\"" << upnp_device.nt_ << "\"," <<
                    "\"nts\":\"" <<  upnp_device.nts_ << "\",\"server\":\"" << upnp_device.server_ << "\",\"usn\":\"" << upnp_device.usn_ << "\"," <<
                    "\"last_seen\":" << upnp_device.last_seen_ << ",\"cache_control\":" << upnp_device.cache_control_ << "}";
            return out;
    }

private:
    std::string host_;
    std::string location_;
    std::string nt_;
    std::string nts_;
    std::string server_;
    std::string usn_;

    time_t last_seen_;
    double cache_control_;
};

/**
 * \brief Interface for the SSDP event listener
 */
class SSDPEventListener {
public:
	enum EVENT_TYPE { ANNOUNCE, BYE };

	SSDPEventListener() {}
	virtual ~SSDPEventListener() {}

	/**
	 * \brief event method
	 */
        virtual void ssdpEvent ( SSDPEventListener::EVENT_TYPE type, std::string  client_ip, SsdpEvent device ) = 0;
};

typedef std::function< void( SSDPEventListener::EVENT_TYPE type, std::string  client_ip, SsdpEvent device ) > event_callback_t;

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
	Response ( status_type status, std::string request_line, std::map< std::string, std::string > headers ) : status ( status ), request_line ( request_line ), headers ( headers ) {}
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
	SSDPCallback() {}
	virtual ~SSDPCallback() {}

	/**
	 * The SSDP Callback method.
	 * \param headers the http request
	 */
	virtual void handle_receive ( ::http::HttpRequest & request ) = 0;
	/**
	 * The SSDP Callback method.
	 * \param headers the http response
	 */
	virtual void handle_response ( ::http::HttpResponse & response ) = 0;
};
/**
 * The SSDP Connection..
 * \param headers the request headers
 * \returns the response headers
 */
class SSDPConnection {
public:

	SSDPConnection() {}
	virtual ~SSDPConnection() {}

	/**
	 * Start the server.
	 */
	virtual void start() = 0;
	/**
	 * Stop the server.
	 */
	virtual void stop() = 0;
	/**
	 * Send a message to the network.
	 * \param headers the messsage headers
	 */
	virtual void send ( std::string request_line, std::map< std::string, std::string > headers ) = 0;
	/**
	 * Send a reponse to the sender.
	 * \param response the response object
	 */
	virtual void send ( Response response ) = 0;
	/**
	 * Set the SSDP Handlers.
	 * \param handler the handler implementation
	 */
	virtual void set_handler ( SSDPCallback * handler ) = 0;
};
/**
 * SSDP Server.
 */
class SSDPServerImpl : public SSDPCallback {

public:
	/**
	 * Create a new SSDPServer.
	 */
        explicit SSDPServerImpl ( const std::string & uuid, const std::string & multicast_address, const int & multicast_port, const std::map< std::string, std::string > & namespaces );
	virtual ~SSDPServerImpl() {}
	/**
	 * Announce the services in the network.
	 */
	void announce();
	/**
	 * Suppress the services in the network.
	 */
	void suppress();
	/**
	 * Search for services in the network. The call is asynchronous, the services are notified.
	 * @brief Search Services
	 * @param service the service, default ssdp:all
	 */
	void search ( const std::string & service = UPNP_NS_ALL );
	/**
	* Start the server.
	*/
	void start();
	/**
	* Stop the server.
	*/
	void stop();
	/**
	* Register an UPNP Service.
	* \param ns the Service namespace
	* \param location the service description URL
	*/
	void register_namespace ( std::string ns, std::string location ) {
		namespaces[ns] = location;
	}
	/**
	* Handle response callback method..
	* \param headers the responset headers
	*/
	virtual void handle_response ( ::http::HttpResponse & response );
	/**
	* Handle receive callback method..
	* \param headers the request headers
	*/
	virtual void handle_receive ( ::http::HttpRequest & request );
        /**
	 * \brief Subscribe for events.
	 */
        void subscribe ( event_callback_t listener ) {
		listeners.push_back ( listener );
	}

private:
	std::string uuid, local_listen_address, multicast_address;
	int multicast_port;
	std::unique_ptr<SSDPConnection> connection;

        SsdpEvent parseRequest ( http::HttpRequest & request );

        FRIEND_TEST( HeaderParseTest, Response );
        SsdpEvent parseResponse ( http::HttpResponse & response );

	void send_anounce ( const std::string & nt, const std::string & location );
	void send_suppress ( const std::string & nt );
	std::map< std::string, std::string > create_response ( const std::string & nt, const std::string & location );
        std::map< std::string, std::string > namespaces; //the namespaces for this server
//        std::map< std::string, SsdpEvent > upnp_devices;

        std::vector< event_callback_t > listeners;
        void fireEvent ( SSDPEventListener::EVENT_TYPE type, std::string client_ip, SsdpEvent device ) const;

	bool announce_thread_run = true;
	std::unique_ptr<std::thread> annouceThreadRunner;
        std::chrono::high_resolution_clock::time_point _announce_time;
	void annouceThread();
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
