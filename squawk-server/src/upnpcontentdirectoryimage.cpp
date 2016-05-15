/*
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
#include "upnpcontentdirectoryimage.h"
#include "upnpcontentdirectorydao.h"

namespace squawk {

log4cxx::LoggerPtr UpnpContentDirectoryImage::logger ( log4cxx::Logger::getLogger ( "squawk.UpnpContentDirectoryImage" ) );

bool UpnpContentDirectoryImage::match ( ::upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId ( request, "/images/" );
}
int UpnpContentDirectoryImage::getRootNode ( ::didl::DidlXmlWriter * didl_element ) {
    didl_element->container ( "/images/", "", didl::DidlContainer (
                                  0, 0,"Images", "/images", 0, 0, SquawkServer::instance()->dao()->childrenCount ( didl::objectItemImageItemPhoto, 0 ) ) );
    return 1;
}
std::tuple<size_t, size_t> UpnpContentDirectoryImage::parseNode ( didl::DidlXmlWriter * didl_element, ::upnp::UpnpContentDirectoryRequest * request ) {
    size_t start_index_ = std::stoi ( request->getValue ( upnp::START_INDEX ) );
    size_t request_count_ = std::stoi ( request->getValue ( upnp::REQUESTED_COUNT ) );

    if ( request_count_ == 0 ) { request_count_ = 128; }

    std::tuple<size_t, size_t> res_;

    if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "Request Image: parent=" << ContentDirectoryModule::item_id ( request ) << ", start_index=" << start_index_ << ", request_count=" << request_count_ ); }

    /* ----------- Photo Albums ----------- */
    if ( request->contains ( upnp::OBJECT_ID ) &&
            request->getValue ( upnp::OBJECT_ID ) == "/images/" ) {

        std::list< didl::DidlContainerPhotoAlbum > photo_album_list_ = SquawkServer::instance()->dao()->objects<didl::DidlContainerPhotoAlbum> ( start_index_, request_count_ );
        std::for_each ( photo_album_list_.begin(), photo_album_list_.end(), [&] ( didl::DidlContainerPhotoAlbum & a ) {
            didl_element->container ( "/images/{}", "/images/{}", http_uri ( "albumArtUri/{0}.jpg" ), a );
        } );
        res_ = std::tuple<size_t, size_t> ( photo_album_list_.size(), SquawkServer::instance()->dao()->objectsCount ( didl::objectContainerAlbumPhotoAlbum ) );

    } else {

        std::list< didl::DidlPhoto > photo_list_ =
            SquawkServer::instance()->dao()->children<didl::DidlPhoto> ( ContentDirectoryModule::item_id ( request ), start_index_, request_count_ );

        for ( auto & photo__ : photo_list_ ) {
            didl_element->write ( "/images/{}", "/images/{}", http_uri() + "resource/{0}.jpg", photo__ );
        }

        res_ = std::tuple<size_t, size_t> ( photo_list_.size(), SquawkServer::instance()->dao()->childrenCount ( didl::objectItemImageItemPhoto, ContentDirectoryModule::item_id ( request ) ) );
    }

    return ( res_ );
}
}//namespace squawk
