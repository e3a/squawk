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
#include "mimetypes.h"

#define OBJECT_ID "ObjectID"

#define QUERY_ALBUM "select name, genre, year from tbl_cds_albums where ROWID = ?"
#define QUERY_SONGS_BY_ALBUM ""

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr UpnpMusicDirectoryModule::logger( log4cxx::Logger::getLogger( "squawk.servlet.UpnpMusicDirectoryModule" ) );

bool UpnpMusicDirectoryModule::match( commons::upnp::UpnpContentDirectoryRequest * request ) {
    if( request->contains( OBJECT_ID ) && commons::string::starts_with( request->getValue( OBJECT_ID ), "music" ) ) {
        return true;
    } else return false;
}
void UpnpMusicDirectoryModule::getRootNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result ) {

    commons::xml::Node didl_element = xmlWriter->element( "DIDL-Lite" );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DIDL );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PURL, "dc");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DLNA, "dlna");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_UPNP, "upnp");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PV, "pv");

    commons::xml::Node container_element = xmlWriter->element(didl_element, "", "container", "");
    xmlWriter->attribute(container_element, "id", "music");
    xmlWriter->attribute(container_element, "parentID", "0");
    xmlWriter->attribute(container_element, "restricted", "1");
    xmlWriter->attribute(container_element, "childCount", "2");

    xmlWriter->element(container_element, commons::upnp::XML_NS_PURL, "title", "Music");
    xmlWriter->element(container_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
    xmlWriter->element(container_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

    cds_result->number_returned( cds_result->number_returned()+1 );
    cds_result->total_matches( cds_result->total_matches()+1 );
}
void UpnpMusicDirectoryModule::parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result,
                                          commons::upnp::UpnpContentDirectoryRequest * request ) {

    commons::xml::Node didl_element = xmlWriter->element( "DIDL-Lite" );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DIDL );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PURL, "dc");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DLNA, "dlna");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_UPNP, "upnp");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PV, "pv");

    /* ----------- Root Node ----------- */
    if( request->contains( OBJECT_ID) && request->getValue( OBJECT_ID ) == "music" ) {

        commons::xml::Node albums_element = xmlWriter->element(didl_element, "", "container", "");
        xmlWriter->attribute(albums_element, "id", "music.albums");
        xmlWriter->attribute(albums_element, "parentID", "music");
        xmlWriter->attribute(albums_element, "restricted", "1");
        xmlWriter->attribute(albums_element, "childCount", commons::string::to_string( albumCount() ));
        xmlWriter->element(albums_element, commons::upnp::XML_NS_PURL, "title", "Albums");
        xmlWriter->element(albums_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
        xmlWriter->element(albums_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

        commons::xml::Node artist_element = xmlWriter->element(didl_element, "", "container", "");
        xmlWriter->attribute(artist_element, "id", "music.artists");
        xmlWriter->attribute(artist_element, "parentID", "music");
        xmlWriter->attribute(artist_element, "restricted", "1");
        xmlWriter->attribute(artist_element, "childCount", commons::string::to_string( artistCount() ));
        xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "title", "Artists");
        xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
        xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

        cds_result->number_returned( 2 );
        cds_result->total_matches( 2 );

        /* ----------- Artists ----------- */
    } else if( request->contains( OBJECT_ID) && request->getValue( OBJECT_ID ) == "music.artists" ) {

        artists([&] (const int id, const std::string & name) {
                commons::xml::Node artist_element = xmlWriter->element( didl_element, "", "container", "" );
                xmlWriter->attribute(artist_element, "id", "music.artists:" + std::to_string( id ) );
        xmlWriter->attribute(artist_element, "parentID", "music");
        xmlWriter->attribute(artist_element, "restricted", "1");
        xmlWriter->attribute(artist_element, "childCount", std::to_string( albumByArtistCount( id ) ) );

        xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "title", name );
        xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
        xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

    });
    int artist_count = artistCount();
    cds_result->number_returned( artist_count );
    cds_result->total_matches( artist_count );

    /* ----------- Albums by Artist ----------- */
    } else if( request->contains( OBJECT_ID) && commons::string::starts_with( request->getValue( OBJECT_ID ), "music.artists:" ) ) {
        unsigned long id = commons::string::parse_string<unsigned long>(
                    request->getValue( OBJECT_ID ).substr(request->getValue( OBJECT_ID ).find(":")+1, request->getValue( OBJECT_ID ).length()));

        albums( id, [&] (const int album_id, const std::string & name, const std::string & genre, const std::string & year) {
                commons::xml::Node artist_element = xmlWriter->element( didl_element, "", "container", "" );
                xmlWriter->attribute(artist_element, "id", "music.albums:" + std::to_string( album_id ) );
        xmlWriter->attribute(artist_element, "parentID", "music.artists");
        xmlWriter->attribute(artist_element, "restricted", "1");
        xmlWriter->attribute(artist_element, "childCount", std::to_string( songByAlbumCount( album_id ) ) );
        xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "title", name );
        xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
        xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");
        artist( album_id, [&] (const int id, const std::string & artist_name ) {
                // add the artists
                xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "artits", artist_name );
    });
    xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "date", year + "-01-01" );
    commons::xml::Node dlna_album_art_node = xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "albumArtURI",
                                                                "http://" + squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) +
                                                                "/album/" + std::to_string( album_id ) + "/cover.jpg" );
    xmlWriter->ns(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "dlna", false);
    xmlWriter->attribute(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "profileID", "JPEG_TN" );
    });

    int album_count = albumByArtistCount( id );
    cds_result->number_returned( album_count );
    cds_result->total_matches( album_count );

    /* ----------- Albums ----------- */
    } else if( request->contains( OBJECT_ID) && request->getValue( OBJECT_ID ) == "music.albums" ) {

        albums( [&xmlWriter, &didl_element, this] (const int id, const std::string & name, const std::string & genre, const std::string & year) {
                commons::xml::Node artist_element = xmlWriter->element( didl_element, "", "container", "" );
                xmlWriter->attribute(artist_element, "id", "music.albums:" + std::to_string( id ) );
        xmlWriter->attribute(artist_element, "parentID", "music.artists");
        xmlWriter->attribute(artist_element, "restricted", "1");
        //TODO xmlWriter->attribute(artist_element, "childCount", std::to_string( songByAlbumCount( id ) ) );
        xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "title", name );
        xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
        xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");
        artist( id, [&xmlWriter, &artist_element] (const int artist_id, const std::string & artist_name ) {
                // add the artists
                xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "artits", artist_name );
    });
    xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "date", year + "-01-01" );
    commons::xml::Node dlna_album_art_node = xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "albumArtURI",
                                                                "http://" + squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) +
                                                                "/album/" + std::to_string( id ) + "/cover.jpg" );
    xmlWriter->ns(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "dlna", false);
    xmlWriter->attribute(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "profileID", "JPEG_TN" );

    });
    int album_count = albumCount();
    cds_result->number_returned( album_count );
    cds_result->total_matches( album_count );


    /* ----------- Songs ----------- */
    } else if( request->contains( OBJECT_ID) && commons::string::starts_with( request->getValue( OBJECT_ID ), "music.albums:" ) ) {
        unsigned long id = commons::string::parse_string<unsigned long>(
                    request->getValue( OBJECT_ID ).substr( request->getValue( OBJECT_ID ).find(":")+1, request->getValue( OBJECT_ID ).length()));

        std::string album_, year_, genre_;
        album( id, [&xmlWriter, &didl_element, &genre_, &album_, &year_, this]
               (const int id, const std::string & name, const std::string & genre, const std::string & year) {
               album_ = name;
                year_ = year;
        genre_ = genre;
    });

    songs( id, [&] (const int & song_id, const int & track, const std::string & title, const std::string & album, const std::string & mtime,
                    const std::string & mime_type, const int & size, const int & playtime, const int & bitrate, const int & samplerate, const int & sampleFrequency) {

           commons::xml::Node item_element = xmlWriter->element( didl_element, "", "item", "" );
    xmlWriter->attribute(item_element, "id", "/music/albums/songs/" + commons::string::to_string<int>(song_id) );
    xmlWriter->attribute(item_element, "parentID", request->getValue( OBJECT_ID ) );
    xmlWriter->attribute(item_element, "restricted", "1");

    xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "originalTrackNumber", commons::string::to_string<int>( track ) );
    xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "title", title );
    xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "album", album_ );
    xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "class", "object.item.audioItem.musicTrack" );
    xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "date", year_ + "-01-01" );
    xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "genre", genre_ );
    artist( id, [&xmlWriter, &item_element] (const int artist_id, const std::string & artist_name ) {
            // add the artists
            xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "artits", artist_name );
    });
    commons::xml::Node dlna_album_art_node = xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "albumArtURI",
                                                                "http://" + squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) +
                                                                "/album/" + std::to_string( id ) + "/cover.jpg" );
    xmlWriter->ns(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "dlna", false);
    xmlWriter->attribute(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "profileID", "JPEG_TN" );

    xmlWriter->element(item_element, commons::upnp::XML_NS_PV, "modificationTime", mtime );
    /*           "&lt;pv:modificationTime&gt;" << (*list_iter).mtime << "&lt;/pv:modificationTime&gt;
                &lt;pv:addedTime&gt;" << (*list_iter).mtime << "&lt;/pv:addedTime&gt;" <<
               "&lt;pv:lastUpdated&gt;" << (*list_iter).mtime << "&lt;/pv:lastUpdated&gt; */

    commons::xml::Node dlna_res_node = xmlWriter->element(item_element, "", "res",
        "http://" +squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) +
        "/song/" + commons::string::to_string( song_id ) + "." + http::mime::extension( mime_type ) );
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
    LOG4CXX_WARN(logger,"unknown request: " << request->getValue( OBJECT_ID ) );
    }
}
}}
