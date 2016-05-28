/*
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

#include "upnpcontentdirectoryvideo.h"
#include "squawkserver.h"
#include "upnpcontentdirectorydao.h"

namespace squawk {

log4cxx::LoggerPtr UpnpContentDirectoryVideo::logger ( log4cxx::Logger::getLogger ( "squawk.UpnpContentDirectoryVideo" ) );

bool UpnpContentDirectoryVideo::match ( ::upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId ( request, "/movies/" ) || ContentDirectoryModule::matchObjectId ( request, "/series/" );
}
int UpnpContentDirectoryVideo::getRootNode ( ::didl::DidlXmlWriter * didl_element ) {
    didl_element->container ( "/movies/", "", didl::DidlContainer ( 0, 0,"Movies", "/movies/0", 0, 0, SquawkServer::instance()->dao()->objectsCount ( didl::objectItemVideoItemMovie ) ) );
    didl_element->container ( "/series/", "", didl::DidlContainer ( 0, 0,"Series", "/series/0", 0, 0, SquawkServer::instance()->dao()->seriesCount () ) );
    return 2;
}
std::tuple<size_t, size_t> UpnpContentDirectoryVideo::parseNode ( didl::DidlXmlWriter * didl_element, ::upnp::UpnpContentDirectoryRequest * request ) {

    size_t start_index_ = std::stoi ( request->getValue ( upnp::START_INDEX ) );
    size_t request_count_ = std::stoi ( request->getValue ( upnp::REQUESTED_COUNT ) );

    if ( request_count_ == 0 ) { request_count_ = 128; }

    size_t returned_ = 0, total = 0;
    int parent_id_ = ContentDirectoryModule::item_id ( request );

    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "Request Movie: parent=" << parent_id_ << ", start_index=" << start_index_ << ", request_count=" << request_count_ ); }


    if ( ContentDirectoryModule::matchObjectId ( request, "/movies/" ) ) {
        std::list< didl::DidlMovie > item_list =
            SquawkServer::instance()->dao()->objects< didl::DidlMovie > ( start_index_, request_count_ );

        for ( auto & item : item_list ) {
            didl_element->write ( "/movies/{}", "/movies/{}", http_uri ( "resource/{0}.{1}" ), SquawkServer::instance()->dao()->object<didl::DidlMovie> ( item.id() ) );
        }

        returned_ = item_list.size();
        total = SquawkServer::instance()->dao()->objectsCount ( didl::objectItemVideoItemMovie );

    } else if ( ContentDirectoryModule::matchObjectId ( request, "/series/" ) ) {
        if ( request->getValue ( "ObjectID" ) == "/series" || request->getValue ( "ObjectID" ) == "/series/" ) {
            std::list< didl::DidlContainer > item_list =
                SquawkServer::instance()->dao()->series ( start_index_, request_count_ );

            for ( auto & item : item_list ) {
                didl_element->container ( "/series/" + item.title(), "/series/", item );
            }

            returned_ = item_list.size();
            total = SquawkServer::instance()->dao()->seriesCount ();

        } else {
            std::string name_ = request->getValue ( "ObjectID" ).substr ( request->getValue ( "ObjectID" ).rfind ( '/' ) + 1 );

            std::list< didl::DidlMovie > item_list =
                SquawkServer::instance()->dao()->objects< didl::DidlMovie > (
                            start_index_, request_count_, std::map<std::string, std::string> ( { { "series_title", name_ } } ) );

            for ( auto & item : item_list ) {
                didl_element->write ( "/movies/{}", "/movies/{}", http_uri ( "resource/{0}.{1}" ), item );
            }

            returned_ = item_list.size();
            total = SquawkServer::instance()->dao()->seriesCount ( std::map<std::string, std::string> ( { { "series_title", name_ } } ) );
        }
    }

    return ( std::tuple< size_t, size_t > ( returned_, total ) );
}
}//namespace squawk
