/*
    Copyright (C) 2013  <etienne> <etienne.knecht@hotmail.com>

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

namespace squawk {

log4cxx::LoggerPtr UpnpContentDirectoryMusic::logger( log4cxx::Logger::getLogger( "squawk.upnp.UpnpMusicDirectoryModule" ) );

bool UpnpContentDirectoryMusic::match( upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId( request, "/music/" );
}
int UpnpContentDirectoryMusic::getRootNode( didl::DidlWriter * didl_element ) {
    didl_element->container( "/music/album", "%d", didl::DidlContainer( 0, 0,"Music Albums", "", 0, 0, _upnp_cds_dao->albumsCount() ) );
    didl_element->container( "/music/artist", "%d", didl::DidlContainer( 0, 0,"Music Artists", "", 0, 0, _upnp_cds_dao->artistsCount() ) );
    didl_element->container( "/music/new", "%d", didl::DidlContainer( 0, 0,"New Albums", "", 0, 0, _upnp_cds_dao->albumsCount() ) );
    return 3;
}
std::tuple<size_t, size_t> UpnpContentDirectoryMusic::parseNode( didl::DidlWriter * didl_element, upnp::UpnpContentDirectoryRequest * request ) {

    //TODO handle filter and sort values.

    int start_index_ = std::stoi( request->getValue( upnp::START_INDEX ) );
    int request_count_ = std::stoi( request->getValue( upnp::REQUESTED_COUNT ) );
    if( request_count_ == 0 ) request_count_ = 128;
    std::tuple<size_t, size_t> res_;
    try {

        /* ----------- Artists ----------- */
        if( request->contains( upnp::OBJECT_ID ) ) {

            std::string object_id_ = request->getValue( upnp::OBJECT_ID );
            if( object_id_ == "/music/artist" ) {

                std::list< didl::DidlContainerArtist > artist_list_ = _upnp_cds_dao->artists( start_index_, request_count_ );
                std::for_each( artist_list_.begin(), artist_list_.end(), [&didl_element]( didl::DidlContainerArtist & a ) {
                    didl_element->container( "/music/artist/"+a.cleanName(), "/music/artist", a );
                });

                res_ = std::tuple<size_t, size_t>( artist_list_.size(), _upnp_cds_dao->artistsCount() );


            /* ----------- Albums by Artist ----------- */
            } else if( request->contains( upnp::OBJECT_ID) &&
                       ContentDirectoryModule::matchObjectId( request, "/music/artist/" ) ) {

                std::string artist_name_ = ContentDirectoryModule::item_string( request );
                std::map< std::string, std::string > filters_( { { "artist", artist_name_ }, { "contributor", artist_name_ } } );
                std::list< didl::DidlContainerAlbum > album_list_ = _upnp_cds_dao->albums( start_index_, request_count_, filters_ );
                std::for_each( album_list_.begin(), album_list_.end(), [&didl_element]( didl::DidlContainerAlbum & a ) {
                    didl_element->container( "/music/album/%d", "/music/artist", a );
                });

                res_ = std::tuple<size_t, size_t>( album_list_.size(), _upnp_cds_dao->albumsCount( filters_ ) );

            /* ----------- Albums ----------- */
            } else if( request->contains( upnp::OBJECT_ID) &&
                       request->getValue( upnp::OBJECT_ID ) == "/music/album" ) {

                std::list< didl::DidlContainerAlbum > album_list_ = _upnp_cds_dao->albums( start_index_, request_count_ );
                std::for_each( album_list_.begin(), album_list_.end(), [&] ( didl::DidlContainerAlbum & a ) {
                    didl_element->container( "/music/album/%d", "/music/album", _http_uri + "art/%d.jpg", a );
                } );

                res_ = std::tuple<size_t, size_t>( album_list_.size(), _upnp_cds_dao->albumsCount() );

            /* ----------- New Albums ----------- */
            } else if( request->contains( upnp::OBJECT_ID) &&
                       request->getValue( upnp::OBJECT_ID ) == "/music/new" ) {

                std::list< didl::DidlContainerAlbum > album_list_ = _upnp_cds_dao->albums( start_index_, request_count_,
                                                                                           std::map< std::string, std::string >(),
                                                                                           "mtime" );
                std::for_each( album_list_.begin(), album_list_.end(), [&didl_element]( didl::DidlContainerAlbum & a ) {
                    didl_element->container( "/music/album/%d", "/music/new", a );
                });

                res_ = std::tuple<size_t, size_t>( album_list_.size(), _upnp_cds_dao->albumsCount() );

            /* ----------- Songs ----------- */
            } else if( request->contains( upnp::OBJECT_ID) &&
                       ContentDirectoryModule::matchObjectId( request, "/music/album/" ) ) {

                size_t album_id_ = ContentDirectoryModule::item_id( request );
                std::list< didl::DidlMusicTrack > track_list_ = _upnp_cds_dao->tracks( album_id_, start_index_, request_count_ );
                std::for_each( track_list_.begin(), track_list_.end(), [&]( didl::DidlMusicTrack & m ) {
                    didl_element->write( "/music/song/%d", "/music/album/%d", _http_uri + "res/%d.%s", m );
                });

                res_ = std::tuple<size_t, size_t>(
                            track_list_.size(), _upnp_cds_dao->objectsCount( didl::objectItemAudioItemMusicTrack,  album_id_ ) );

            } else if( squawk::DEBUG ) {
                LOG4CXX_DEBUG(logger,"unknown request: " << request->getValue( upnp::OBJECT_ID ) );
            }
        } else if( squawk::DEBUG ) {
            LOG4CXX_DEBUG(logger,"no ObjectId set in request: " << request );
        }

    } catch( upnp::UpnpException & e ) {
        LOG4CXX_FATAL(logger, "Can not get id, Exception:" << e.code() << " -> " << e.what());
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in id.");
    }
    return res_;
}
} // squawk
