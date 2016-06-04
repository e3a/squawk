#ifndef SSDPSERVERIMPL_H
#define SSDPSERVERIMPL_H

#include <assert.h>

#include "gtest/gtest_prod.h"

#include "ssdp.h"

namespace ssdp {

/**
 * SSDP Server.
 */
class SSDPServerImpl : public SSDPCallback {
public:
        /**
         * Create a new SSDPServer.
         */
        explicit SSDPServerImpl (
            const std::string & uuid, const std::string & multicast_address, const int & multicast_port,
            const std::map< std::string, std::string > & namespaces );

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
        void search ( const std::string & service = SSDP_NS_ALL );
        /**
        * Start the server.
        */
        void start(); //TODO
        /**
        * Stop the server.
        */
        void stop(); //TODO
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
        /** hot many times the SSDP M-SEARCH and NOTIFY messages are sent. */
        static const size_t NETWORK_COUNT = 3;
        /** hot many times the SSDP M-SEARCH and NOTIFY messages are sent. */
        static const size_t ANNOUNCE_INTERVAL = 1800;
        static const std::string SSDP_HEADER_SERVER, SSDP_HEADER_DATE, SSDP_HEADER_ST,
            SSDP_HEADER_NTS, SSDP_HEADER_USN, SSDP_HEADER_LOCATION, SSDP_HEADER_NT, SSDP_HEADER_MX,
            SSDP_HEADER_MAN, SSDP_HEADER_EXT, SSDP_OPTION_MAX_AGE, SSDP_REQUEST_LINE_OK,
            SSDP_STATUS_DISCOVER, SSDP_STATUS_ALIVE, SSDP_STATUS_BYE, SSDP_NS_ALL, SSDP_MSEARCH,
            SSDP_NOTIFY, SSDP_HEADER_REQUEST_LINE, SSDP_HEADER_SEARCH_REQUEST_LINE;

        std::string uuid, local_listen_address, multicast_address;
        int multicast_port;
        std::unique_ptr<SSDPConnection> connection;
        std::map< std::string, std::string > namespaces; //the namespaces for this server
        std::vector< event_callback_t > listeners;

        SsdpEvent parseRequest ( http::HttpRequest & request );

        void send_anounce ( const std::string & nt, const std::string & location );
        void send_suppress ( const std::string & nt );

        std::map< std::string, std::string > create_response ( const std::string & nt, const std::string & location );

        void fireEvent ( SSDPEventListener::EVENT_TYPE type, std::string client_ip, SsdpEvent device ) const;

        bool announce_thread_run = true;
        std::unique_ptr<std::thread> annouceThreadRunner;
        std::chrono::high_resolution_clock::time_point _announce_time;
        void annouceThread();

        FRIEND_TEST( HeaderParseTest, Response );
        SsdpEvent parseResponse ( http::HttpResponse & response );

        FRIEND_TEST( TimerTest, ParseTimeTest );
        FRIEND_TEST( TimerTest, ParseTimeSpacesTest );
        static inline time_t parse_keep_alive(const std::string & cache_control ) {
            time_t time = 0;
            std::string cache_control_clean = boost::erase_all_copy( cache_control, " " );
            if( cache_control_clean.find( SSDP_OPTION_MAX_AGE ) == 0 ) {
                time = boost::lexical_cast< time_t > ( cache_control_clean.substr ( SSDP_OPTION_MAX_AGE.size() ) );

            } else assert( false ); //wrong cache control format
            return time;
        }
};
}//namespace ssdp
#endif // SSDPSERVERIMPL_H
