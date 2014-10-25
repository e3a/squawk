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

#define CDS_MEDIA_ROOT "music"
#define CDS_MEDIA_ARTIST "music.artists"

#define OBJECT_ID "ObjectID" //TODO move to upnp.h

#define QUERY_ARTISTS "select ROWID, name from tbl_cds_artists order by name"
#define QUERY_ARTISTS_COUNT "select count(*) from tbl_cds_artists"
#define QUERY_ALBUMS_COUNT "select count(*) from tbl_cds_albums"
#define QUERY_ARTIST_BY_ALBUM "select artist.ROWID, artist.name from tbl_cds_artists artist " \
                              "JOIN tbl_cds_artists_albums m ON artist.ROWID = m.artist_id " \
                              "where m.album_id=?"
#define QUERY_ALBUM "select name, genre, year from tbl_cds_albums where ROWID = ?"
#define QUERY_ALBUMS "select name, genre, year, ROWID from tbl_cds_albums ORDER BY name"
#define QUERY_ALBUMS_BY_ARTISTS "select album.name, album.genre, album.year, album.ROWID from tbl_cds_albums album, tbl_cds_artists_albums m, tbl_cds_artists artist " \
                         "where artist.ROWID = ? and album.ROWID = m.album_id and artist.ROWID = m.artist_id"
#define QUERY_SONGS_BY_ALBUM "select songs.ROWID, songs.title, songs.track, songs.filename, songs.length, songs.bitrate, songs.sample_rate, songs.bits_per_sample, songs.channels, songs.mime_type, songs.disc, songs.mtime, songs.filesize " \
                             "from tbl_cds_audiofiles songs, tbl_cds_albums album where album.ROWID = ? and album.ROWID = songs.album_id order by songs.track, songs.disc"

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr UpnpMusicDirectoryModule::logger( log4cxx::Logger::getLogger( "squawk.servlet.UpnpMusicDirectoryModule" ) );

bool UpnpMusicDirectoryModule::match( commons::upnp::UpnpContentDirectoryRequest * request ) {
  if( request->contains( OBJECT_ID ) && commons::string::starts_with( request->getValue( OBJECT_ID ), CDS_MEDIA_ROOT ) ) {
    return true;
  } else return false;
}
void UpnpMusicDirectoryModule::getRootNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result ) {

    commons::xml::Node didl_element = xmlWriter->element("", "DIDL-Lite", "");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DIDL, "", true);
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
void UpnpMusicDirectoryModule::parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result, commons::upnp::UpnpContentDirectoryRequest * request ) {

    commons::xml::Node didl_element = xmlWriter->element("", "DIDL-Lite", "");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DIDL, "", true);
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PURL, "dc");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DLNA, "dlna");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_UPNP, "upnp");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PV, "pv");

    /* ----------- Root Node ----------- */
    if( request->contains( OBJECT_ID) && request->getValue( OBJECT_ID ) == CDS_MEDIA_ROOT ) {

      squawk::db::Sqlite3Statement * stmt_artists_count = NULL;
      squawk::db::Sqlite3Statement * stmt_albums_count = NULL;
      int artist_count = 0;
      int albums_count = 0;
      try {
          stmt_artists_count = db->prepare_statement( QUERY_ARTISTS_COUNT );
          stmt_albums_count = db->prepare_statement( QUERY_ALBUMS_COUNT );

          if( stmt_artists_count->step() ) {
              artist_count = stmt_artists_count->get_int( 0 );
          }
          if( stmt_albums_count->step() ) {
              albums_count = stmt_albums_count->get_int( 0 );
          }
      } catch( squawk::db::DaoException & e ) {
          LOG4CXX_FATAL(logger, "Can not get artist_count, Exception:" << e.code() << "-> " << e.what());
          if(stmt_artists_count != NULL) db->release_statement( stmt_artists_count );
          if(stmt_albums_count != NULL) db->release_statement( stmt_albums_count );
          throw;
      } catch( ... ) {
          LOG4CXX_FATAL(logger, "Other Excpeption in get_artist_count.");
          throw;
      }

      commons::xml::Node albums_element = xmlWriter->element(didl_element, "", "container", "");
      xmlWriter->attribute(albums_element, "id", "music.albums");
      xmlWriter->attribute(albums_element, "parentID", "music");
      xmlWriter->attribute(albums_element, "restricted", "1");
      xmlWriter->attribute(albums_element, "childCount", commons::string::to_string( albums_count ));
      xmlWriter->element(albums_element, commons::upnp::XML_NS_PURL, "title", "Albums");
      xmlWriter->element(albums_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
      xmlWriter->element(albums_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

      commons::xml::Node artist_element = xmlWriter->element(didl_element, "", "container", "");
      xmlWriter->attribute(artist_element, "id", "music.artists");
      xmlWriter->attribute(artist_element, "parentID", "music");
      xmlWriter->attribute(artist_element, "restricted", "1");
      xmlWriter->attribute(artist_element, "childCount", commons::string::to_string( artist_count ));
      xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "title", "Artists");
      xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
      xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

      cds_result->number_returned( 2 );
      cds_result->total_matches( 2 );

  /* ----------- Artists ----------- */
  } else if( request->contains( OBJECT_ID) && request->getValue( OBJECT_ID ) == CDS_MEDIA_ARTIST ) {

    squawk::db::Sqlite3Statement * stmt_artists = NULL;
    squawk::db::Sqlite3Statement * stmt_artists_count = NULL;
    try {
        stmt_artists = db->prepare_statement( QUERY_ARTISTS );
        stmt_artists_count = db->prepare_statement( QUERY_ARTISTS_COUNT );

        if( stmt_artists_count->step() ) {

            cds_result->number_returned( stmt_artists_count->get_int( 0 ) );
            cds_result->total_matches( stmt_artists_count->get_int( 0 ) );

            while( stmt_artists->step() ) {

                commons::xml::Node artist_element = xmlWriter->element( didl_element, "", "container", "" );
                xmlWriter->attribute(artist_element, "id", "music.artists:" + std::to_string(stmt_artists->get_int( 0 ) ) );
                xmlWriter->attribute(artist_element, "parentID", "music");
                xmlWriter->attribute(artist_element, "restricted", "1");
                xmlWriter->attribute(artist_element, "childCount", std::to_string( stmt_artists_count->get_int( 0 ) ) );
                xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "title", stmt_artists->get_string(1) );
                xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
                xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");
            }
        }
        stmt_artists->reset();
        db->release_statement( stmt_artists );
        db->release_statement( stmt_artists_count );

    } catch( squawk::db::DaoException & e ) {
        LOG4CXX_FATAL(logger, "Can not get artists, Exception:" << e.code() << "-> " << e.what());
        if(stmt_artists != NULL) db->release_statement( stmt_artists );
        if(stmt_artists_count != NULL) db->release_statement( stmt_artists_count );
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_artists.");
        throw;
    }

    /* ----------- Albums by Artist ----------- */
    } else if( request->contains( OBJECT_ID) && commons::string::starts_with( request->getValue( OBJECT_ID ), "music.artists:" ) ) {
    unsigned long id = commons::string::parse_string<unsigned long>( request->getValue( OBJECT_ID ).substr(request->getValue( OBJECT_ID ).find(":")+1, request->getValue( OBJECT_ID ).length()));

    squawk::db::Sqlite3Statement * stmt_albums = NULL;
    squawk::db::Sqlite3Statement * stmt_artist = NULL;
    try {
        stmt_albums = db->prepare_statement( QUERY_ALBUMS_BY_ARTISTS );
        stmt_artist = db->prepare_statement( QUERY_ARTIST_BY_ALBUM );

        stmt_albums->bind_int( 1, id );
        while( stmt_albums->step() ) {

            commons::xml::Node artist_element = xmlWriter->element( didl_element, "", "container", "" );
            xmlWriter->attribute(artist_element, "id", "music.albums:" + std::to_string( stmt_albums->get_int( 3 ) ) );
            xmlWriter->attribute(artist_element, "parentID", "music.artists");
            xmlWriter->attribute(artist_element, "restricted", "1");
            xmlWriter->attribute(artist_element, "childCount", std::to_string( 1 ) ); //TODO count songs in album
            xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "title", stmt_albums->get_string(0) );
            xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
            xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

            // add the artists
            stmt_artist->bind_int(1, stmt_albums->get_int(3) );
            while( stmt_artist->step() ) {
                xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "artits", stmt_artist->get_string(1) );
            }
            stmt_artist->reset();

            xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "date", stmt_albums->get_string(2) + "-01-01" );
            commons::xml::Node dlna_album_art_node = xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "albumArtURI",
                "http://" + squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) +
                "/album/" + std::to_string(stmt_albums->get_int(3)) + "/cover.jpg" );
            xmlWriter->ns(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "dlna", false);
            xmlWriter->attribute(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "profileID", "JPEG_TN" );
        }

        stmt_albums->reset();
        db->release_statement(stmt_artist);
        db->release_statement(stmt_albums);

    } catch(squawk::db::DaoException & e) {
        LOG4CXX_FATAL(logger, "Can not get albums by artists, Exception:" << e.code() << "-> " << e.what());
        if(stmt_albums != NULL) db->release_statement( stmt_albums );
        if(stmt_artist != NULL) db->release_statement( stmt_artist );
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get albums by artists.");
        throw;
    }

  /* ----------- Albums ----------- */
  } else if( request->contains( OBJECT_ID) && request->getValue( OBJECT_ID ) == "music.albums" ) {

    squawk::db::Sqlite3Statement * stmt_albums = NULL;
    squawk::db::Sqlite3Statement * stmt_artist = NULL;
    try {

        stmt_albums = db->prepare_statement( QUERY_ALBUMS );        
        stmt_artist = db->prepare_statement( QUERY_ARTIST_BY_ALBUM );
        while( stmt_albums->step() ) {

            commons::xml::Node artist_element = xmlWriter->element( didl_element, "", "container", "" );
            xmlWriter->attribute(artist_element, "id", "music.albums:" + std::to_string( stmt_albums->get_int( 3 ) ) );
            xmlWriter->attribute(artist_element, "parentID", "music.artists");
            xmlWriter->attribute(artist_element, "restricted", "1");
            xmlWriter->attribute(artist_element, "childCount", std::to_string( 1 ) ); //TODO count songs in album
            xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "title", stmt_albums->get_string(0) );
            xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
            xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

            stmt_artist->bind_int(1, stmt_albums->get_int(3));
            while( stmt_artist->step() ) {
                xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "artits", stmt_artist->get_string(1) );
            }
            stmt_artist->reset();

            xmlWriter->element(artist_element, commons::upnp::XML_NS_PURL, "date", stmt_albums->get_string(2) + "-01-01" );
            commons::xml::Node dlna_album_art_node = xmlWriter->element(artist_element, commons::upnp::XML_NS_UPNP, "albumArtURI",
                "http://" + squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) +
                "/album/" + std::to_string(stmt_albums->get_int(3)) + "/cover.jpg" );
            xmlWriter->ns(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "dlna", false);
            xmlWriter->attribute(dlna_album_art_node, commons::upnp::XML_NS_DLNA_METADATA, "profileID", "JPEG_TN" );
        }

        stmt_albums->reset();
        db->release_statement(stmt_artist);
        db->release_statement(stmt_albums);

    } catch( squawk::db::DaoException & e ) {
        LOG4CXX_FATAL(logger, "Can not get albums, Exception:" << e.code() << "-> " << e.what());
        if(stmt_albums != NULL) db->release_statement(stmt_albums);
        if(stmt_artist != NULL) db->release_statement(stmt_artist);
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_albums.");
        throw;
    }

    /* ----------- Songs ----------- */
  } else if( request->contains( OBJECT_ID) && commons::string::starts_with( request->getValue( OBJECT_ID ), "music.albums:" ) ) {
    unsigned long id = commons::string::parse_string<unsigned long>( request->getValue( OBJECT_ID ).substr( request->getValue( OBJECT_ID ).find(":")+1, request->getValue( OBJECT_ID ).length()));

    squawk::db::Sqlite3Statement * stmt_songs = NULL;
    squawk::db::Sqlite3Statement * stmt_album = NULL;
    squawk::db::Sqlite3Statement * stmt_artists = NULL;
    try {

        stmt_album = db->prepare_statement( QUERY_ALBUM );
        stmt_album->bind_int( 1, id );
        stmt_songs = db->prepare_statement( QUERY_SONGS_BY_ALBUM );
        stmt_songs->bind_int( 1, id );
        stmt_artists = db->prepare_statement( QUERY_ARTIST_BY_ALBUM );
        stmt_artists->bind_int( 1, id );

        std::string creator;
        while( stmt_artists->step() ) {
            creator += "dc:creator&gt;" + commons::string::escape_xml(commons::string::escape_xml( stmt_artists->get_string( 1 ) ) ) + "&lt;/dc:creator&gt;";
        }


        if( stmt_album->step() ) {
            while( stmt_songs->step() ) {

                commons::xml::Node item_element = xmlWriter->element( didl_element, "", "item", "" );
                xmlWriter->attribute(item_element, "id", "/music/albums/songs/" + id );
                xmlWriter->attribute(item_element, "parentID", request->getValue( OBJECT_ID ) );
                xmlWriter->attribute(item_element, "restricted", "1");
                xmlWriter->attribute(item_element, "childCount", std::to_string( 1 ) ); //TODO count songs in album

                xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "title", stmt_songs->get_string( 1 ) );
                xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "album", stmt_songs->get_string( 0 ) );
                xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "class", "object.item.audioItem.musicTrack");
                //creator

//                "&lt;upnp:artist role=\"artist\"&gt;" << "album.artist" << "&lt;/upnp:artist&gt;&lt;dc:contributor&gt;" << "album.artist" << "&lt;/dc:contributor&gt;" <<
//                "&lt;upnp:originalTrackNumber&gt;" << stmt_songs->get_int( 3 ) << "&lt;/upnp:originalTrackNumber&gt;&lt;dc:date&gt;" << stmt_album->get_int( 3 ) << "-01-01&lt;/dc:date&gt;" <<
//                "&lt;upnp:genre&gt;" << stmt_album->get_string( 2 ) << "&lt;/upnp:genre&gt;" <<

//                "&lt;upnp:albumArtURI dlna:profileID=\"JPEG_TN\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\"&gt;" <<
//                "http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/" << id << "/cover.jpg&lt;/upnp:albumArtURI&gt;" <<
//                "&lt;upnp:albumArtURI&gt;http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/image/" << dao->getFrontImage(id).id << ".jpg&lt;/upnp:albumArtURI&gt;" <<

                //TODO DLNA FLAG
//                "&lt;pv:modificationTime&gt;" << stmt_songs->get_string( 12 ) << "&lt;/pv:modificationTime&gt;&lt;pv:addedTime&gt;" << stmt_songs->get_string( 12 ) << "&lt;/pv:addedTime&gt;" <<
//                "&lt;pv:lastUpdated&gt;" << stmt_songs->get_string( 12 ) << "&lt;/pv:lastUpdated&gt;&lt;res protocolInfo=\"http-get:*:" << stmt_songs->get_string( 10 )  << ":DLNA.ORG_OP=11;DLNA.ORG_FLAGS=" <<
//                "01700000000000000000000000000000\" size=\"" << stmt_songs->get_int( 13 ) << "\" duration=\"" << commons::string::time_to_string( stmt_songs->get_int( 5 ) ) << "\" bitrate=\"" << stmt_songs->get_int( 6 ) << "\" bitsPerSample=\"" << stmt_songs->get_int( 8 ) << "\" " <<
//                "sampleFrequency=\"" << stmt_songs->get_int( 7 ) << "\"&gt;http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/"  << "&lt;/res&gt;" <<

                "&lt;/item&gt;";
            }
        }

        stmt_songs->reset();
        db->release_statement(stmt_songs);

    } catch( squawk::db::DaoException & e ) {
        LOG4CXX_FATAL(logger, "Can not get songs, Exception:" << e.code() << "-> " << e.what());
        if(stmt_songs != NULL) db->release_statement( stmt_songs );
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_songs.");
        throw;
    }
/*TODO    squawk::model::Album album = dao->get_album(id);
    std::list< squawk::model::Song > songs = dao->getSongsByAlbum(id);

    for(std::list< squawk::model::Song >::iterator list_iter = songs.begin(); list_iter != songs.end(); list_iter++) {

      result << "&lt;item id=\"/music/albums/songs/" << (*list_iter).id << "\" parentID=\"" << request[OBJECT_ID] << "\" restricted=\"1\"&gt;" <<
      "&lt;upnp:class&gt;object.item.audioItem.musicTrack&lt;/upnp:class&gt;" <<
      "&lt;dc:title&gt;" << (*list_iter).title << "&lt;/dc:title&gt;&lt;dc:creator&gt;" << "album.artist" << "&lt;/dc:creator&gt;&lt;upnp:album&gt;" << album.name << "&lt;/upnp:album&gt;" <<

      "&lt;upnp:artist role=\"artist\"&gt;" << "album.artist" << "&lt;/upnp:artist&gt;&lt;dc:contributor&gt;" << "album.artist" << "&lt;/dc:contributor&gt;" <<
      "&lt;upnp:originalTrackNumber&gt;" << (*list_iter).track << "&lt;/upnp:originalTrackNumber&gt;&lt;dc:date&gt;" << album.year << "-01-01&lt;/dc:date&gt;" <<
      "&lt;upnp:genre&gt;" << album.genre << "&lt;/upnp:genre&gt;" <<

      "&lt;upnp:albumArtURI dlna:profileID=\"JPEG_TN\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\"&gt;" <<
      "http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/" << id << "/cover.jpg&lt;/upnp:albumArtURI&gt;" <<
      "&lt;upnp:albumArtURI&gt;http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/image/" << dao->getFrontImage(id).id << ".jpg&lt;/upnp:albumArtURI&gt;" <<

      //TODO DLNA FLAG
      "&lt;pv:modificationTime&gt;" << (*list_iter).mtime << "&lt;/pv:modificationTime&gt;&lt;pv:addedTime&gt;" << (*list_iter).mtime << "&lt;/pv:addedTime&gt;" <<
      "&lt;pv:lastUpdated&gt;" << (*list_iter).mtime << "&lt;/pv:lastUpdated&gt;&lt;res protocolInfo=\"http-get:*:" << (*list_iter).mime_type  << ":DLNA.ORG_OP=11;DLNA.ORG_FLAGS=" <<
      "01700000000000000000000000000000\" size=\"" << (*list_iter).size << "\" duration=\"" << commons::string::time_to_string((*list_iter).playLength) << "\" bitrate=\"" << (*list_iter).bitrate << "\" bitsPerSample=\"" << (*list_iter).samplerate << "\" " <<
      "sampleFrequency=\"" << (*list_iter).sampleFrequency << "\"&gt;http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/"  << "&lt;/res&gt;" <<

      "&lt;/item&gt;";
    } */

  } else if( request->contains( OBJECT_ID) && commons::string::starts_with( request->getValue( OBJECT_ID ), "music." ) ) {
      std::cout << "requested music: = " << request->getValue( OBJECT_ID ) << std::endl; //TODO logger
  }
}
}}
