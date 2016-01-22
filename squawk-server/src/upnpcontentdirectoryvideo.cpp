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

#include "http.h"

namespace squawk {

log4cxx::LoggerPtr UpnpContentDirectoryVideo::logger( log4cxx::Logger::getLogger( "squawk.upnp.UpnpVideoDirectory" ) );

bool UpnpContentDirectoryVideo::match( ::upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId( request, "video" );
}
int UpnpContentDirectoryVideo::getRootNode( ::didl::DidlWriter * didl_element ) {
    didl_element->container( "/movies/", "", didl::DidlContainer( 0, 0,"Movies", "/movies/0", 0, 0, 0 /*TODO get child count */ ) );
    didl_element->container( "/series/", "", didl::DidlContainer( 0, 0,"Series", "/series/0", 0, 0, 0 /*TODO get child count */ ) );
    return 2;
}
std::tuple<size_t, size_t> UpnpContentDirectoryVideo::parseNode( didl::DidlWriter * didl_element, ::upnp::UpnpContentDirectoryRequest * request ) {

    std::tuple< size_t, size_t > res_;
    /* ----------- Root Node ----------- */
    if( request->contains( "ObjectID" ) && request->getValue( "ObjectID" ) == "video.movies" ) {

//TODO        videos( std::bind((void(didl::DidlElement::*)(didl::DidlMovie))&didl::DidlElement::write, didl_element, std::placeholders::_1 ) );
//TODO        int video_count = videoCount();
//        res_ = std::tuple< size_t, size_t >( video_count, video_count );

   } else {
        LOG4CXX_WARN(logger,"unknown request: " << request->getValue( "ObjectID" ) );
   }
    return res_;
}
}
