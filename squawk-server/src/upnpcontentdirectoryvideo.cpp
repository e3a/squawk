/*
    Upnp video directory module.
    Copyright (C) 2013  <copyright holder> <email>

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

log4cxx::LoggerPtr UpnpContentDirectoryVideo::logger( log4cxx::Logger::getLogger( "squawk.UpnpContentDirectoryVideo" ) );

bool UpnpContentDirectoryVideo::match( ::upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId( request, "/movies/" );
}
int UpnpContentDirectoryVideo::getRootNode( ::didl::DidlXmlWriter * didl_element ) {
    didl_element->container( "/movies/", "", didl::DidlContainer( 0, 0,"Movies", "/movies/0", 0, 0, 0 /*TODO get child count */ ) );
    didl_element->container( "/series/", "", didl::DidlContainer( 0, 0,"Series", "/series/0", 0, 0, 0 /*TODO get child count */ ) );
    return 2;
}
std::tuple<size_t, size_t> UpnpContentDirectoryVideo::parseNode( didl::DidlXmlWriter * didl_element, ::upnp::UpnpContentDirectoryRequest * request ) {

    size_t start_index_ = std::stoi( request->getValue( upnp::START_INDEX ) );
    size_t request_count_ = std::stoi( request->getValue( upnp::REQUESTED_COUNT ) );
    if( request_count_ == 0 ) request_count_ = 128;
    size_t returned_ = 0, total = 0;
    int parent_id_ = ContentDirectoryModule::item_id( request );

    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "Request Movie: parent=" << parent_id_ << ", start_index=" << start_index_ << ", request_count=" << request_count_ );

    try {
        std::list< didl::DidlMovie > item_list =
            SquawkServer::instance()->dao()->objects< didl::DidlMovie >( start_index_, request_count_ );

        for( auto & item : item_list ) {
            didl_element->write( "/movies/{}", "/movies/{}", "resource/{0}.{1}", SquawkServer::instance()->dao()->object<didl::DidlMovie>( item.id() ) );
        }
    } catch( db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not get files, Movies:" << e.code() << "-> " << e.what());
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get movies.");
    }
    return( std::tuple< size_t, size_t >( returned_, total ) );
}
}
