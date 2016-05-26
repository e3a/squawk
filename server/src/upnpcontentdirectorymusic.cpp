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

#include "upnpcontentdirectorymusic.h"
#include "upnpcontentdirectorydao.h"

namespace squawk {

log4cxx::LoggerPtr UpnpContentDirectoryMusic::logger ( log4cxx::Logger::getLogger ( "squawk.UpnpContentDirectoryMusic" ) );

bool UpnpContentDirectoryMusic::match ( upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId ( request, "/music/" );
}
int UpnpContentDirectoryMusic::getRootNode ( didl::DidlXmlWriter * didl_element ) {
    didl_element->container ( "/music/album", "{}",
                              didl::DidlContainer ( 0, 0,"Music Albums", "", 0, 0, SquawkServer::instance()->dao()->objectsCount ( didl::objectContainerAlbumMusicAlbum ) ) );
    didl_element->container ( "/music/artist", "{}", didl::DidlContainer ( 0, 0,"Music Artists", "", 0, 0, SquawkServer::instance()->dao()->artistsCount() ) );
    didl_element->container ( "/music/new", "{}",
                              didl::DidlContainer ( 0, 0,"New Albums", "", 0, 0, SquawkServer::instance()->dao()->objectsCount ( didl::objectContainerAlbumMusicAlbum ) ) );
    return 3;
}
std::tuple<size_t, size_t> UpnpContentDirectoryMusic::parseNode ( didl::DidlXmlWriter * didl_element, upnp::UpnpContentDirectoryRequest * request ) {

    int start_index_ = std::stoi ( request->getValue ( upnp::START_INDEX ) );
    int request_count_ = std::stoi ( request->getValue ( upnp::REQUESTED_COUNT ) );

    if ( request_count_ == 0 ) { request_count_ = 128; }

    std::tuple<size_t, size_t> res_;

    if ( request->contains ( upnp::OBJECT_ID ) ) {

        std::string object_id_ = request->getValue ( upnp::OBJECT_ID );

        /* ----------- Artists ----------- */
        if ( object_id_ == "/music/artist" ) {

            std::list< didl::DidlContainerArtist > artist_list_ =
                SquawkServer::instance()->dao()->artists ( start_index_, request_count_, std::map< std::string, std::string >(), std::make_pair ( "name", "asc" ) );
            std::for_each ( artist_list_.begin(), artist_list_.end(), [&didl_element] ( didl::DidlContainerArtist & a ) {
                didl_element->container ( "/music/artist/"+a.cleanName(), "/music/artist", a );
            } );

            res_ = std::tuple<size_t, size_t> ( artist_list_.size(), SquawkServer::instance()->dao()->artistsCount() );


            /* ----------- Albums by Artist ----------- */

        } else if ( request->contains ( upnp::OBJECT_ID ) &&
                    ContentDirectoryModule::matchObjectId ( request, "/music/artist" ) ) {

            std::string artist_name_ = ContentDirectoryModule::item_string ( request );
            std::map< std::string, std::string > filters_ ( { { "artist", artist_name_ }, { "contributor", artist_name_ } } );
            std::list< didl::DidlContainerAlbum > album_list_ = SquawkServer::instance()->dao()->objects<didl::DidlContainerAlbum> ( start_index_, request_count_, filters_ );
            std::for_each ( album_list_.begin(), album_list_.end(), [&didl_element] ( didl::DidlContainerAlbum & a ) {
                didl_element->container ( "/music/album/{}", "/music/artist/", http_uri ( "albumArtUri/{}.jpg" ), a );
            } );

            res_ = std::tuple<size_t, size_t> ( album_list_.size(), SquawkServer::instance()->dao()->objectsCount ( didl::objectContainerAlbumMusicAlbum, filters_ ) );

            /* ----------- Albums ----------- */

        } else if ( request->contains ( upnp::OBJECT_ID ) &&
                    request->getValue ( upnp::OBJECT_ID ) == "/music/album" ) {

            std::list< didl::DidlContainerAlbum > album_list_ = SquawkServer::instance()->dao()->objects<didl::DidlContainerAlbum> ( start_index_, request_count_ );
            std::for_each ( album_list_.begin(), album_list_.end(), [&] ( didl::DidlContainerAlbum & a ) {
                didl_element->container ( "/music/album/{}", "/music/album", http_uri ( "albumArtUri/{}.jpg" ), a );
            } );
            res_ = std::tuple<size_t, size_t> ( album_list_.size(), SquawkServer::instance()->dao()->objectsCount ( didl::objectContainerAlbumMusicAlbum ) );

            /* ----------- New Albums ----------- */

        } else if ( request->contains ( upnp::OBJECT_ID ) &&
                    request->getValue ( upnp::OBJECT_ID ) == "/music/new" ) {

            std::list< didl::DidlContainerAlbum > album_list_ = SquawkServer::instance()->dao()->objects<didl::DidlContainerAlbum> ( start_index_, request_count_,
                    std::map< std::string, std::string >(),
                    std::make_pair ( "mtime", "desc" ) );
            std::for_each ( album_list_.begin(), album_list_.end(), [&didl_element] ( didl::DidlContainerAlbum & a ) {
                didl_element->container ( "/music/album/{}", "/music/new", http_uri ( "albumArtUri/{}.jpg" ), a );
            } );

            res_ = std::tuple<size_t, size_t> ( album_list_.size(), SquawkServer::instance()->dao()->objectsCount ( didl::objectContainerAlbumMusicAlbum ) );

            /* ----------- Songs ----------- */

        } else if ( request->contains ( upnp::OBJECT_ID ) &&
                    ContentDirectoryModule::matchObjectId ( request, "/music/album/" ) ) {

            size_t album_id_ = ContentDirectoryModule::item_id ( request );
            std::list< didl::DidlObject > track_list_ =
                SquawkServer::instance()->dao()->children<didl::DidlObject> (
                    album_id_, start_index_, request_count_,
            std::map<std::string, std::string> ( {
                { "cls", std::to_string ( didl::objectContainer ) + ", " + std::to_string ( didl::objectItemAudioItemMusicTrack ) }
            } ),
            std::pair<std::string, std::string > ( "track, cls, title", "asc" )
                );

            for ( auto object_ : track_list_ ) {
                if ( object_.cls() == didl::objectItemAudioItemMusicTrack ) {
                    didl_element->write ( "/music/album/{}", "/music/album/{}", http_uri ( "resource/{0}.{1}" ),
                                          SquawkServer::instance()->dao()->object<didl::DidlMusicTrack> ( object_.id() ) );

                } else if ( object_.cls() == didl::objectItemImageItemPhoto ) {
                    didl_element->write ( "/music/album/{}", "/music/album/{}", http_uri ( "resource/{0}.{1}" ),
                                          SquawkServer::instance()->dao()->object<didl::DidlPhoto> ( object_.id() ) );

                } else if ( object_.cls() == didl::objectContainer ) {
                    didl_element->container ( "/music/album/{}", "/music/album/{}",
                                              SquawkServer::instance()->dao()->object<didl::DidlContainer> ( object_.id() ) );
                }
            }

            res_ = std::tuple<size_t, size_t> (
                       track_list_.size(), SquawkServer::instance()->dao()->childrenCount ( didl::object, album_id_,
            std::map<std::string, std::string> ( {
                { "cls", std::to_string ( didl::objectContainer ) + ", " + std::to_string ( didl::objectItemAudioItemMusicTrack ) }
            } )
                                                                                          ) );

        } else if ( squawk::SUAWK_SERVER_DEBUG ) {
            LOG4CXX_DEBUG ( logger,"unknown request: " << request->getValue ( upnp::OBJECT_ID ) );
        }

    } else if ( squawk::SUAWK_SERVER_DEBUG ) {
        LOG4CXX_DEBUG ( logger,"no ObjectId set in request: " << request );
    }

    return res_;
}
}//namespace squawk
