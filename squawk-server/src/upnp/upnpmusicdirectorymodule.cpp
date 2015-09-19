/*
    <one line to give the program's name and a brief idea of what it does.>
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

#include "upnpmusicdirectorymodule.h"
#include "commons.h"
#include "http.h"

// #define OBJECT_ID "ObjectID"

// #define QUERY_ALBUM "select name, genre, year from tbl_cds_albums where ROWID = ?"
// #define QUERY_SONGS_BY_ALBUM ""

namespace squawk {
namespace upnp {

log4cxx::LoggerPtr UpnpMusicDirectoryModule::logger( log4cxx::Logger::getLogger( "squawk.upnp.UpnpMusicDirectoryModule" ) );

bool UpnpMusicDirectoryModule::match( commons::upnp::UpnpContentDirectoryRequest * request ) {
    if( request->contains( commons::upnp::OBJECT_ID ) &&
        commons::string::starts_with( request->getValue( commons::upnp::OBJECT_ID ), "music" ) ) {
        return true;
    } else return false;
}
void UpnpMusicDirectoryModule::getRootNode( commons::xml::XMLWriter * xmlWriter, commons::xml::Node * element, commons::upnp::CdsResult * cds_result ) {

    ::upnp::didl::node( xmlWriter, (*element), "music", "0", "3", "Music", commons::upnp::UPNP_CLASS_CONTAINER );
}
void UpnpMusicDirectoryModule::parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result,
                                          commons::upnp::UpnpContentDirectoryRequest * request ) {

    commons::xml::Node didl_element = xmlWriter->element( "DIDL-Lite" );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DIDL );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PURL, "dc");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DLNA, "dlna");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_UPNP, "upnp");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PV, "pv");

    int start_index = commons::string::parse_string<int>( request->getValue( commons::upnp::START_INDEX ) );
    int request_count = commons::string::parse_string<int>( request->getValue( commons::upnp::REQUESTED_COUNT ) );
    if( request_count == 0 ) request_count = 128;

    /* ----------- Root Node ----------- */
    if( request->contains( commons::upnp::OBJECT_ID ) &&
        request->getValue( commons::upnp::OBJECT_ID ) == "music" ) {

        ::upnp::didl::node( xmlWriter, didl_element, "music.albums", "music", commons::string::to_string( albumCount() ), "Albums", commons::upnp::UPNP_CLASS_CONTAINER );
        ::upnp::didl::node( xmlWriter, didl_element, "music.artists", "music", commons::string::to_string( artistCount() ), "Artists", commons::upnp::UPNP_CLASS_CONTAINER );
        ::upnp::didl::node( xmlWriter, didl_element, "music.new", "music", "100", "New Albums", commons::upnp::UPNP_CLASS_CONTAINER );
        cds_result->number_returned( 3 );
        cds_result->total_matches( 3 );

    /* ----------- Artists ----------- */
    } else if( request->contains( commons::upnp::OBJECT_ID ) &&
               request->getValue( commons::upnp::OBJECT_ID ) == "music.artists" ) {

        int number_returned = 0;
        try {
            squawk::db::db_statement_ptr stmt_artists = db->prepareStatement( squawk::sql::QUERY_ARTIST );
            stmt_artists->bind_int( 1, start_index );
            stmt_artists->bind_int( 2, request_count );
            while( stmt_artists->step() ) {
                ::upnp::didl::node( xmlWriter, didl_element, "music.artists:" + std::to_string( stmt_artists->get_int( 0 ) ), "music",
                                    std::to_string( albumByArtistCount( stmt_artists->get_int( 0 ) ) ), stmt_artists->get_string( 1 ),
                                    commons::upnp::UPNP_CLASS_MUSIC_ARTIST );
                ++number_returned;
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get artist, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in artist.");
            throw;
        }
        int artist_count = artistCount();
        cds_result->number_returned( number_returned );
        cds_result->total_matches( artist_count );

    /* ----------- Albums by Artist ----------- */
    } else if( request->contains( commons::upnp::OBJECT_ID) &&
               commons::string::starts_with( request->getValue( commons::upnp::OBJECT_ID ), "music.artists:" ) ) {
        unsigned long id = commons::string::parse_string<unsigned long>(
                    request->getValue( commons::upnp::OBJECT_ID ).substr(request->getValue( commons::upnp::OBJECT_ID ).find(":")+1,
                                                          request->getValue( commons::upnp::OBJECT_ID ).length()));

        int return_count = 0;
        try {
            const std::string uri_prefix = "http://" + http_address_ + ":" + http_port_;
            squawk::db::db_statement_ptr stmt_artist_albums = db->prepareStatement( squawk::sql::SQL_ARTIST_ALBUM );
            stmt_artist_albums->bind_int( 1, id );
            stmt_artist_albums->bind_int( 2, start_index );
            stmt_artist_albums->bind_int( 3, request_count );
            while( stmt_artist_albums->step() ) {
                ::upnp::didl::item(xmlWriter, didl_element, stmt_artist_albums->get_int( 2 ), stmt_artist_albums->get_string( 0 ), stmt_artist_albums->get_string( 1 ),
                                 artist( stmt_artist_albums->get_int( 2 ) ), uri_prefix);
                ++return_count;
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get albums by artist, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in albums by artist.");
            throw;
        }
        int album_count = albumByArtistCount( id );
        cds_result->number_returned( return_count );
        cds_result->total_matches( album_count );

    /* ----------- Albums ----------- */
    } else if( request->contains( commons::upnp::OBJECT_ID) && request->getValue( commons::upnp::OBJECT_ID ) == "music.albums" ) {

        int albums_returned = 0;
        try {
            const std::string uri_prefix = "http://" + http_address_ + ":" + http_port_;
            squawk::db::db_statement_ptr stmt_albums = db->prepareStatement( squawk::sql::SQL_ALBUM );
            stmt_albums->bind_int( 1, start_index );
            stmt_albums->bind_int( 2, request_count );
            std::cout << "start stmt:albums" << request_count  << std::endl;
            while( stmt_albums->step() ) {
                ::upnp::didl::item(xmlWriter, didl_element, stmt_albums->get_int( 2 ), stmt_albums->get_string( 0 ), stmt_albums->get_string( 1 ),
                                 artist( stmt_albums->get_int( 2 ) ), uri_prefix);
                ++albums_returned;
            }
            std::cout << "endt stmt:albums" << albums_returned << std::endl;

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get albums, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in albums.");
            throw;
        }
        int album_count = albumCount();
        cds_result->number_returned( albums_returned );
        cds_result->total_matches( album_count );

    /* ----------- New Albums ----------- */
    } else if( request->contains( commons::upnp::OBJECT_ID) && request->getValue( commons::upnp::OBJECT_ID ) == "music.new" ) {

        int albums_returned = 0;
        try {
            const std::string uri_prefix = "http://" + http_address_ + ":" + http_port_;
            squawk::db::db_statement_ptr stmt_albums = db->prepareStatement( squawk::sql::SQL_ALBUM_NEW );
            while( stmt_albums->step() ) {
                ::upnp::didl::item(xmlWriter, didl_element, stmt_albums->get_int( 2 ), stmt_albums->get_string( 0 ), stmt_albums->get_string( 1 ),
                                 artist( stmt_albums->get_int( 2 ) ), uri_prefix);
                ++albums_returned;
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get new albums, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in new albums.");
            throw;
        }
        int album_count = 100;
        cds_result->number_returned( albums_returned );
        cds_result->total_matches( album_count );

    /* ----------- Songs ----------- */
    } else if( request->contains( commons::upnp::OBJECT_ID) && commons::string::starts_with( request->getValue( commons::upnp::OBJECT_ID ), "music.albums:" ) ) {
        unsigned long id = commons::string::parse_string<unsigned long>(
                    request->getValue( commons::upnp::OBJECT_ID ).substr(
                        request->getValue( commons::upnp::OBJECT_ID ).find(":")+1, request->getValue( commons::upnp::OBJECT_ID ).length()));

        std::string album_, year_, genre_;
        // get the album
        squawk::db::db_statement_ptr stmt_album = db->prepareStatement( squawk::sql::SQL_ALBUM_ID );
        stmt_album->bind_int( 1, id );
        while( stmt_album->step() ) {
            album_ = stmt_album->get_string(0);
             year_ = stmt_album->get_string(2);
             genre_ = stmt_album->get_string(1);
        }

    songs( id, [&] (const int & song_id, const int & track, const std::string & title, const std::string & album, const std::string & mtime,
                    const std::string & mime_type, const int & size, const int & playtime, const int & bitrate, const int & samplerate, const int & sampleFrequency) {

        commons::xml::Node item_element = xmlWriter->element( didl_element, "", "item", "" );
        xmlWriter->attribute(item_element, "id", "/music/albums/songs/" + commons::string::to_string<int>(song_id) );
        xmlWriter->attribute(item_element, "parentID", request->getValue( commons::upnp::OBJECT_ID ) );
        xmlWriter->attribute(item_element, "restricted", "1");

        xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "originalTrackNumber", commons::string::to_string<int>( track ) );
        xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "title", title );
        xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "album", album_ );
        xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "class", commons::upnp::UPNP_CLASS_MUSIC_TRACK );
        xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "date", year_ + "-01-01" );
        xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "genre", genre_ );
        /* TODO artist( id, [&xmlWriter, &item_element] (const int artist_id, const std::string & artist_name ) {
                // add the artists
                xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "artits", artist_name );
                xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "creator", artist_name );
        }); */
    commons::xml::Node dlna_album_art_node = xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "albumArtURI",
        "http://" + http_address_ + ":" + http_port_ +
        "/api/album/" + std::to_string( id ) + "/cover.jpg" );
    xmlWriter->ns(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "dlna", false);
    xmlWriter->attribute(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "profileID", "JPEG_TN" );

    xmlWriter->element(item_element, commons::upnp::XML_NS_PV, "modificationTime", mtime );
    /*           "&lt;pv:modificationTime&gt;" << (*list_iter).mtime << "&lt;/pv:modificationTime&gt;
                &lt;pv:addedTime&gt;" << (*list_iter).mtime << "&lt;/pv:addedTime&gt;" <<
               "&lt;pv:lastUpdated&gt;" << (*list_iter).mtime << "&lt;/pv:lastUpdated&gt; */

    commons::xml::Node dlna_res_node = xmlWriter->element(item_element, "", "res",
        "http://" + http_address_ + ":" + commons::string::to_string( http_port_ ) +
        "/audio/" + commons::string::to_string( song_id ) + "." + http::mime::extension( mime_type ) );
    xmlWriter->attribute(dlna_res_node, "", "protocolInfo",
        "http-get:*:" + mime_type  + ":DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000" );
    xmlWriter->attribute(dlna_res_node, "", "size", commons::string::to_string<int>(size) );
    xmlWriter->attribute(dlna_res_node, "", "duration", commons::string::to_string<int>(playtime ) );
    xmlWriter->attribute(dlna_res_node, "", "bitrate", commons::string::to_string<int>(bitrate) );
    xmlWriter->attribute(dlna_res_node, "", "bitsPerSample", commons::string::to_string<int>(sampleFrequency) );
    xmlWriter->attribute(dlna_res_node, "", "sampleFrequency", commons::string::to_string<int>(samplerate) );

    });
    int song_count = songByAlbumCount( id );
    cds_result->number_returned( song_count );
    cds_result->total_matches( song_count );

    } else {
    LOG4CXX_WARN(logger,"unknown request: " << request->getValue( commons::upnp::OBJECT_ID ) );
    }
}
} // upnp
} // squawk
