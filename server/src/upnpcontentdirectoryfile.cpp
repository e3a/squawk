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
#include "upnpcontentdirectoryfile.h"

#include "upnpcontentdirectorydao.h"
#include "squawkserver.h"

namespace squawk {

bool UpnpContentDirectoryFile::match ( upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId ( request, "/file/" );
}
int UpnpContentDirectoryFile::getRootNode ( ::didl::DidlXmlWriter * didl_element ) {
    didl_element->container ( "/file/", "",
                              didl::DidlContainer (
                                  0, 0, "Filesystem", "", 0, 0, SquawkServer::instance()->dao()->childrenCount ( didl::object, 0 )
                              )
                            );
    return 1;
}
std::tuple<size_t, size_t> UpnpContentDirectoryFile::parseNode ( didl::DidlXmlWriter * didl_element, upnp::UpnpContentDirectoryRequest * request ) {

    size_t start_index_ = std::stoi ( request->getValue ( upnp::START_INDEX ) );
    size_t request_count_ = std::stoi ( request->getValue ( upnp::REQUESTED_COUNT ) );

    if ( request_count_ == 0 ) { request_count_ = 128; }

    int parent_id_ = ContentDirectoryModule::item_id ( request );
    std::tuple<size_t, size_t> res_;

    auto dao = SquawkServer::instance()->dao();
    std::list< didl::DidlObject > item_list_ = dao->children<didl::DidlObject> ( parent_id_, start_index_, request_count_ );

    for ( auto & item__ : item_list_ ) {
        switch ( item__.cls() ) {
        case didl::objectContainer:
        { didl_element->container ( "/file/{}", "/file/{}", dao->object<didl::DidlContainer> ( item__.id() ) ); break; }

        case didl::objectContainerAlbumMusicAlbum:
        { didl_element->container ( "/file/{}", "/file/{}", dao->object<didl::DidlContainerAlbum> ( item__.id() ) ); break; }

        case didl::objectContainerAlbumPhotoAlbum:
        { didl_element->container ( "/file/{}", "/file/{}", dao->object<didl::DidlContainerPhotoAlbum> ( item__.id() ) ); break; }

        case didl::objectItem:
        { didl_element->write ( "/file/{}", "/file/{}", "resource/{0}.{1}", dao->object<didl::DidlItem> ( item__.id() ) ); break; }

        case didl::objectItemImageItemPhoto:
        { didl_element->write ( "/file/{}", "/file/{}", http_uri ( "resource/{0}.{1}" ), dao->object<didl::DidlPhoto> ( item__.id() ) ); break; }

        case didl::objectItemAudioItemMusicTrack:
        { didl_element->write ( "/file/{}", "/file/{}", http_uri ( "resource/{0}.{1}" ), dao->object<didl::DidlMusicTrack> ( item__.id() ) ); break; }

        case didl::objectItemVideoItemMovie:
        { didl_element->write ( "/file/{}", "/file/{}", http_uri ( "resource/{0}.{1}" ), dao->object<didl::DidlMovie> ( item__.id() ) ); break; }

        default:
            break;
        }
    }

    return std::tuple<size_t, size_t> ( item_list_.size(), dao->childrenCount ( didl::object, parent_id_ ) );
}
}// namespace squawk
