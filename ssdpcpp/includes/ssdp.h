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
namespace didl {

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

    } else if( didl::DEBUG ) {
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
      * //TODO JSON ESCAPE || is it used?
      */
    friend std::ostream& operator<< ( std::ostream& out, const didl::SsdpEvent & upnp_device ) {
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
 * @brief The Icon struct
 */
struct Icon {
public:
    Icon() : width_(0), height_(0), depth_(0) {}
    Icon ( const Icon& ) = default;
    Icon ( Icon&& ) = default;
    Icon& operator= ( const Icon& ) = default;
    Icon& operator= ( Icon&& ) = default;
    ~Icon() {}

    /**
     * @brief width
     * @return
     */
    int width() const { return width_; }
    /**
     * @brief width
     * @param width
     */
    void width( const int & width ) { width_ = width; }
    /**
     * @brief height
     * @return
     */
    int height() const { return height_; }
    /**
     * @brief height
     * @param height
     */
    void height( const int & height ) { height_ = height; }
    /**
     * @brief depth
     * @return
     */
    int depth() const { return depth_; }
    /**
     * @brief depth
     * @param depth
     */
    void depth( const int & depth ) { depth_ = depth; }
    /**
     * @brief mimeType
     * @return
     */
    std::string mimeType() const { return mimeType_; }
    /**
     * @brief mimeType
     * @param mimeType
     */
    void mimeType( const std::string & mimeType ) { mimeType_ = mimeType; }
    /**
     * @brief url
     * @return
     */
    std::string url() const { return url_; }
    /**
     * @brief url
     * @param url
     */
    void url( const std::string & url ) { url_ = url; }

private:
    int width_, height_, depth_;
    std::string mimeType_, url_;
};

struct Service {
public:
    Service() {}
    Service ( const Service& ) = default;
    Service ( Service&& ) = default;
    Service& operator= ( const Service& ) = default;
    Service& operator= ( Service&& ) = default;
    ~Service() {}

    std::string serviceType() const { return service_type_; }
    void serviceType( const std::string & service_type ) { service_type_ = service_type; }
    std::string serviceId() const { return service_id_; }
    void serviceId( const std::string & service_id ) { service_id_ = service_id; }
    std::string scpdUrl() const { return scpd_url_; }
    void scpdUrl( const std::string & scpd_url ) { scpd_url_ = scpd_url; }
    std::string controlUrl() const { return control_url_; }
    void controlUrl( const std::string & control_url ) { control_url_ = control_url; }
    std::string eventSubUrl() const { return event_sub_url_; }
    void eventSubUrl( const std::string & event_sub_url ) { event_sub_url_ = event_sub_url; }

private:
    std::string service_type_, service_id_, scpd_url_, control_url_, event_sub_url_;
};

/**
 * @brief The UpnpDevice item
 */
struct UpnpDevice {
public:
    UpnpDevice() {}
    UpnpDevice ( const UpnpDevice& ) = default;
    UpnpDevice ( UpnpDevice&& ) = default;
    UpnpDevice& operator= ( const UpnpDevice& ) = default;
    UpnpDevice& operator= ( UpnpDevice&& ) = default;
    ~UpnpDevice() {}

    /**
     * Required. Major version of the UPnP Device Architecture. Shall be 2 for devices implemented
     * on a UPnP 2. 0 architecture.
     * @brief set version Major
     * @return
     */
    int versionMajor() const { return versionMajor_; }
    /**
     * @brief get version Major
     * @param versionMajor
     */
    void versionMajor( const int & versionMajor) { versionMajor_ = versionMajor; }
    /**
     * Required. Minor version of the UPnP Device Architecture. Shall be 0 for devices implemented
     * on a UPnP 2.0 architecture. Shall accurately reflect the version number of  the UPnP Device
     * Architecture supported by the device.
     * @brief get version Minor
     * @return
     */
    int versionMinor() const { return versionMinor_; }
    /**
     * @brief set version Minor
     * @param versionMinor
     */
    void versionMinor( const int & versionMinor) { versionMinor_ = versionMinor; }
    /**
     * Required. UPnP device type. Single URI.
     * <ul><li>For standard devices  defined by a UPnP Forum working committee, shall begin with
     * "urn :schemas -upnp-org:device:"  followed  by the standardized device type suffix, a colon,
     * and an integer device version i.e. urn:schemas-upnp-org:device:deviceType:ver. The
     * highest supported version of  the device type shall be specified.
     * </li><li>For non-standard devices  specified by UPnP vendors ,  shall begin with “urn:” , followed by
     * a  Vendor Doma in  Name, followed by “:device:”, followed by a  device type suffix, colon,
     * and an integer version, i.e., “urn:domain-name:device:deviceType:ver”. Period
     * characters in the Vendor Domain Name shall be replaced with hyphens in accordance
     * with RFC 2141. The highest supported version of the device type shall be specified.</li></ul>
     * The device type suffix defined by a UPnP Forum working committee or specif ied by a UPnP
     * vendor shall be <= 64 chars, not counting the version suffix and separating colon.
     * @brief device Type
     * @return
     */
    std::string deviceType() const { return deviceType_; }
    /**
     * @brief device Type
     * @param deviceType
     */
    void deviceType( const std::string & deviceType) { deviceType_ = deviceType; }
    /**
     * Required. Short description for end user. Specified by UPnP vendor. String.
     * Should be < 64 characters.
     * @brief friendlyName
     * @return
     */
    std::string friendlyName() const { return friendlyName_; }
    /**
     * @brief friendlyName
     * @param friendlyName
     */
    void friendlyName( const std::string & friendlyName) { friendlyName_ = friendlyName; }
    /**
     * Required.  Manufacturer's name. Specified by UPnP vendor. String. Should be < 64
     * characters.
     * @brief manufacturer
     * @return
     */
    std::string manufacturer() const { return manufacturer_; }
    /**
     * @brief manufacturer
     * @param manufacturer
     */
    void manufacturer( const std::string & manufacturer) { manufacturer_ = manufacturer; }
    /**
     * Allowed. Web site for Manufacturer. Single URL.
     * @brief manufacturerUrl
     * @return
     */
    std::string manufacturerUrl() const { return manufacturerUrl_; }
    /**
     * @brief manufacturerUrl
     * @param manufacturerUrl
     */
    void manufacturerUrl( const std::string & manufacturerUrl) { manufacturerUrl_ = manufacturerUrl; }
    /**
     * Recommended. Long description for end user. Specified by UPnP vendor. String.
     * Should be < 128 characters.
     * @brief modelDescription
     * @return
     */
    std::string modelDescription() const { return modelDescription_; }
    /**
     * @brief modelDescription
     * @param modelDescription
     */
    void modelDescription( const std::string & modelDescription) { modelDescription_ = modelDescription; }
    /**
     * Required. Model name. Specified by UPnP vendor. String. Should be < 32 characters.
     * @brief modelName
     * @return
     */
    std::string modelName() const { return modelName_; }
    /**
     * @brief modelName
     * @param modelName
     */
    void modelName( const std::string & modelName) { modelName_ = modelName; }
    /**
     * Recommended. Model number. Specified by UPnP vendor. String. Should be < 32 characters.
     * @brief modelNumber
     * @return
     */
    std::string modelNumber() const { return modelNumber_; }
    /**
     * @brief modelNumber
     * @param modelNumber
     */
    void modelNumber( const std::string & modelNumber) { modelNumber_ = modelNumber; }
    /**
     * Allowed. Web site for model. Specified by UPnP vendor. Single URL.
     * @brief modelUrl
     * @return
     */
    std::string modelUrl() const { return modelUrl_; }
    /**
     * @brief modelUrl
     * @param modelUrl
     */
    void modelUrl( const std::string & modelUrl) { modelUrl_ = modelUrl; }
    /**
     * Recommended. Serial number.  Specified by UPnP vendor. String. Should be < 64 characters.
     * @brief serialNumber
     * @return
     */
    std::string serialNumber() const { return serialNumber_; }
    /**
     * @brief serialNumber
     * @param serialNumber
     */
    void serialNumber( const std::string & serialNumber) { serialNumber_ = serialNumber; }
    /**
     * Required.  Unique Device Name. Universally- unique identifier for the device, whether  root or
     * embedded. shall be the same over time for a specific device instance ( i. e. , shall  survive
     * reboots) . shall match the field value of the NT header field in device discovery messages.
     * shall match the prefix of the USN header field in all disc overy messages. shall begin with
     * “uuid:” followed by a UUID suffix specified by a UPnP vendor.
     * @brief udn
     * @return
     */
    std::string udn() const { return udn_; }
    /**
     * @brief udn
     * @param udn
     */
    void udn( const std::string & udn) { udn_ = udn; }
    /**
     * Allowed. Universal Product Code.  12-digit,  all- numericc ode that  identifies the consumer
     * package. Managed by the Uniform Code Council. Specified by UPnP vendor. Single UPC.
     * @brief upc
     * @return
     */
    std::string upc() const { return upc_; }
    /**
     * @brief upc
     * @param upc
     */
    void upc( const std::string & upc) { upc_ = upc; }
    /**
     * Recommended. URL to presentation for device. shall be
     * relative to the URL at which the device description is located in accordance with the rules
     * spec ified in clause 5 of  RFC 3986. Specified by UPnP vendor. Single URL.
     * @brief presentationUrl
     * @return
     */
    std::string presentationUrl() const { return presentationUrl_; }
    /**
     * @brief presentationUrl
     * @param presentationUrl
     */
    void presentationUrl( const std::string & presentationUrl) { presentationUrl_ = presentationUrl; }

    /**
     * Required if  and only if  device has one or more icons. Specified by  UPnP vendor.
     * @brief iconList
     * @return
     */
    std::vector< Icon > iconList() const { return images_; }
    /**
     * @brief addIcon
     * @param icon
     */
    void addIcon( Icon icon ) { images_.push_back( icon ); }

    /**
     * @brief serviceList
     * @return
     */
    std::vector< Service > serviceList() const { return services_; }
    /**
     * @brief addService
     * @param service
     */
    void addService( Service service ) { services_.push_back( service ); }

private:
    int versionMajor_, versionMinor_;
    std::string deviceType_, friendlyName_, manufacturer_, manufacturerUrl_, modelDescription_, modelName_,
                modelNumber_, modelUrl_, serialNumber_, udn_, upc_, presentationUrl_;
    std::vector< Icon > images_;
    std::vector< Service > services_;
    std::vector< UpnpDevice > devices_;
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
	explicit SSDPServerImpl ( const std::string & uuid, const std::string & multicast_address, const int & multicast_port );
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
        * Get the UPNP Devices.
	* \return map with the upnp devices, the map key is the service UUID
	*/
        std::map< std::string, SsdpEvent > get_upnp_devices() {
		return upnp_devices;
	}
       /**
        * Get the UPNP DEvices filtered by usn.
        * \return map with the upnp devices, the map key is the service UUID
        */
        std::map< std::string, SsdpEvent > getUpnpDevices( const std::string & usn );

        /**
         * @brief get device description
         * @param event
         * @return
         */
        UpnpDevice deviceDescription( const SsdpEvent & event );
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
	std::map< std::string, std::string > namespaces;
        std::map< std::string, SsdpEvent > upnp_devices;

        std::vector< event_callback_t > listeners;
        void fireEvent ( SSDPEventListener::EVENT_TYPE type, std::string client_ip, SsdpEvent device ) const;

	bool announce_thread_run = true;
	std::unique_ptr<std::thread> annouceThreadRunner;
	std::chrono::high_resolution_clock::time_point start_time;
	void annouceThread();

        FRIEND_TEST( DeviceDescriptionParserTest, SimpleXML );
        void parseDescription( const std::string & description, UpnpDevice & device );
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
}//namespace didl
#endif // SSDPCONNECTION_H
