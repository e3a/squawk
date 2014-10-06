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

#define XML_CDS_ROOT_NODE "&lt;container id=\"music.albums\" parentID=\"music\" restricted=\"1\" childCount=\"2\"&gt;" \
      "&lt;dc:title&gt;Albums&lt;/dc:title&gt;" \
      "&lt;upnp:class&gt;object.container.storageFolder&lt;/upnp:class&gt;" \
      "&lt;upnp:storageUsed&gt;-1&lt;/upnp:storageUsed&gt;" \
      "&lt;/container&gt;" \
      "&lt;container id=\"music.artists\" parentID=\"music\" restricted=\"1\" childCount=\"1\"&gt;" \
      "&lt;dc:title&gt;Artists&lt;/dc:title&gt;" \
      "&lt;upnp:class&gt;object.container.storageFolder&lt;/upnp:class&gt;" \
      "&lt;upnp:storageUsed&gt;-1&lt;/upnp:storageUsed&gt;" \
      "&lt;/container&gt;"
#define ROOT_NODE "&lt;container id=\"music\" parentID=\"0\" restricted=\"1\" childCount=\"2\"&gt;" \
      "&lt;dc:title&gt;Music&lt;/dc:title&gt;" \
      "&lt;upnp:class&gt;object.container.storageFolder&lt;/upnp:class&gt;" \
      "&lt;upnp:storageUsed&gt;-1&lt;/upnp:storageUsed&gt;" \
      "&lt;/container&gt;"
#define OBJECT_ID "ObjectID"

#define QUERY_ARTISTS "select ROWID, name from tbl_cds_artists order by name"
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

log4cxx::LoggerPtr UpnpMusicDirectoryModule::logger(log4cxx::Logger::getLogger("squawk.servlet.UpnpMusicDirectoryModule"));

bool UpnpMusicDirectoryModule::match(std::map<std::string, std::string> request) {

    // log the request
    std::string logBuffer;
    for( auto iterator = request.begin(); iterator != request.end(); ++iterator ) {
        logBuffer += "\t" + iterator->first + ": " + iterator->second + "\n";
    }
    logBuffer += "\n";
    LOG4CXX_FATAL(logger, "UPNP Request:\n" << logBuffer );


  if(request.find(OBJECT_ID) != request.end() && commons::string::starts_with(request[OBJECT_ID], CDS_MEDIA_ROOT)) {      
    return true;
  } else return false;
}
std::string UpnpMusicDirectoryModule::getRootNode() {
    return std::string(ROOT_NODE);
}
std::string UpnpMusicDirectoryModule::parseNode(std::map<std::string, std::string> request) {
  if(request.find(OBJECT_ID) != request.end() && request[OBJECT_ID] == CDS_MEDIA_ROOT) {
      return std::string(XML_CDS_ROOT_NODE);

  } else if(request.find(OBJECT_ID) != request.end() && request[OBJECT_ID] == CDS_MEDIA_ARTIST) {
    std::stringstream result;

    squawk::db::Sqlite3Statement * stmt_artists = NULL;
    try {
        stmt_artists = db->prepare_statement( QUERY_ARTISTS );


        while( stmt_artists->step() ) {

            result << "&lt;container id=\"music.artists:" << std::to_string(stmt_artists->get_int(0)) << "\" parentID=\"music\" restricted=\"1\" childCount=\"1\"&gt;" <<
                      "&lt;dc:title&gt;" << commons::string::escape_xml( commons::string::escape_xml( stmt_artists->get_string(1) ) ) << "&lt;/dc:title&gt;" <<
                      "&lt;upnp:class&gt;object.container.storageFolder&lt;/upnp:class&gt;" <<
                      "&lt;upnp:storageUsed&gt;-1&lt;/upnp:storageUsed&gt;" <<
                      "&lt;/container&gt;";
        }
        stmt_artists->reset();
        db->release_statement(stmt_artists);

    } catch(squawk::db::DaoException & e) {
        LOG4CXX_FATAL(logger, "Can not get artists, Exception:" << e.code() << "-> " << e.what());
        if(stmt_artists != NULL) db->release_statement( stmt_artists );
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_artists.");
        throw;
    }

    return result.str();

  } else if(request.find(OBJECT_ID) != request.end() && commons::string::starts_with(request[OBJECT_ID], "music.artists:")) {
    unsigned long id = commons::string::parse_string<unsigned long>(request[OBJECT_ID].substr(request[OBJECT_ID].find(":")+1, request[OBJECT_ID].length()));
    std::stringstream result;

    squawk::db::Sqlite3Statement * stmt_albums = NULL;
    squawk::db::Sqlite3Statement * stmt_artist = NULL;
    try {
        stmt_albums = db->prepare_statement( QUERY_ALBUMS_BY_ARTISTS );
        stmt_artist = db->prepare_statement( QUERY_ARTIST_BY_ALBUM );

        stmt_albums->bind_int( 1, id );
        while( stmt_albums->step() ) {

            result << "&lt;container id=\"music.albums:" << std::to_string(stmt_albums->get_int(3)) << "\" parentID=\"music:artists:\" restricted=\"1\" childCount=\"1\"&gt;" <<
                "&lt;dc:title&gt;" << commons::string::escape_xml(commons::string::escape_xml( stmt_albums->get_string(0) ) ) << "&lt;/dc:title&gt;" <<
                "&lt;upnp:class&gt;object.container.storageFolder&lt;/upnp:class&gt;" <<
                "&lt;upnp:storageUsed&gt;-1&lt;/upnp:storageUsed&gt;";

            stmt_artist->bind_int(1, stmt_albums->get_int(3) );
            while( stmt_artist->step() ) {
                result << "&lt;upnp:artist&gt;" << commons::string::escape_xml(commons::string::escape_xml( stmt_artist->get_string(1) ) ) << "&lt;/upnp:artist&gt;";
            }
            stmt_artist->reset();

            result << "&lt;dc:date&gt;" << commons::string::escape_xml(commons::string::escape_xml( stmt_albums->get_string(2) ) ) << "-01-01&lt;/dc:date&gt;" <<
                "&lt;upnp:albumArtURI dlna:profileID=\"JPEG_TN\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\"&gt;" <<
                "http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/" << std::to_string(stmt_albums->get_int(3)) << "/cover.jpg&lt;/upnp:albumArtURI&gt;" <<
                "&lt;upnp:albumArtURI&gt;http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/image/" << std::to_string(stmt_albums->get_int(3)) << ".jpg&lt;/upnp:albumArtURI&gt;" <<

                "&lt;/container&gt;";
        }

        stmt_albums->reset();
        db->release_statement(stmt_artist);
        db->release_statement(stmt_albums);

    } catch(squawk::db::DaoException & e) {
        LOG4CXX_FATAL(logger, "Can not get albims by artists, Exception:" << e.code() << "-> " << e.what());
        if(stmt_albums != NULL) db->release_statement( stmt_albums );
        if(stmt_artist != NULL) db->release_statement( stmt_artist );
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get albums by artists.");
        throw;
    }

    return result.str();

  } else if(request.find(OBJECT_ID) != request.end() && request[OBJECT_ID] == "music.albums") {
    std::stringstream result;

    squawk::db::Sqlite3Statement * stmt_albums = NULL;
    squawk::db::Sqlite3Statement * stmt_artist = NULL;
    try {

        stmt_albums = db->prepare_statement( QUERY_ALBUMS );        
        stmt_artist = db->prepare_statement( QUERY_ARTIST_BY_ALBUM );
        while( stmt_albums->step() ) {

            result << "&lt;container id=\"music.albums:" << std::to_string(stmt_albums->get_int(3)) << "\" parentID=\"music:artists:\" restricted=\"1\" childCount=\"1\"&gt;" <<
                "&lt;dc:title&gt;" << commons::string::escape_xml(commons::string::escape_xml( stmt_albums->get_string(0) ) ) << "&lt;/dc:title&gt;" <<
                "&lt;upnp:class&gt;object.container.storageFolder&lt;/upnp:class&gt;" <<
                "&lt;upnp:storageUsed&gt;-1&lt;/upnp:storageUsed&gt;";

            stmt_artist->bind_int(1, stmt_albums->get_int(3));
            while( stmt_artist->step() ) {
                result << "&lt;upnp:artist&gt;" << commons::string::escape_xml(commons::string::escape_xml( stmt_artist->get_string(1) ) ) << "&lt;/upnp:artist&gt;";
            }
            stmt_artist->reset();

            result << "&lt;dc:date&gt;" << commons::string::escape_xml(commons::string::escape_xml( stmt_albums->get_string(2) ) ) << "-01-01&lt;/dc:date&gt;" <<
                "&lt;upnp:albumArtURI dlna:profileID=\"JPEG_TN\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\"&gt;" <<
                "http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/" << std::to_string(stmt_albums->get_int(3)) << "/cover.jpg&lt;/upnp:albumArtURI&gt;" <<
                "&lt;upnp:albumArtURI&gt;http://" << squawk_config->string_value(CONFIG_HTTP_IP) << ":" << squawk_config->string_value(CONFIG_HTTP_PORT) << "/album/image/" << std::to_string(stmt_albums->get_int(3)) << ".jpg&lt;/upnp:albumArtURI&gt;" <<

                "&lt;/container&gt;";
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
    return result.str();

  } else if(request.find(OBJECT_ID) != request.end() && commons::string::starts_with(request[OBJECT_ID], "music.albums:")) {
    unsigned long id = commons::string::parse_string<unsigned long>(request[OBJECT_ID].substr(request[OBJECT_ID].find(":")+1, request[OBJECT_ID].length()));
    std::stringstream result;

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

                result << "&lt;item id=\"/music/albums/songs/" << id << "\" parentID=\"" << request[OBJECT_ID] << "\" restricted=\"1\"&gt;" <<
                "&lt;upnp:class&gt;object.item.audioItem.musicTrack&lt;/upnp:class&gt;" <<
                "&lt;dc:title&gt;" << commons::string::escape_xml(commons::string::escape_xml( stmt_songs->get_string( 1 ) ) ) << "&lt;/dc:title&gt;&lt;" <<
                creator <<
                "&lt;upnp:album&gt;" << commons::string::escape_xml(commons::string::escape_xml( stmt_album->get_string( 0 ) ) )  << "&lt;/upnp:album&gt;" <<

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
    return result.str();

  } else if(request.find(OBJECT_ID) != request.end() && commons::string::starts_with(request[OBJECT_ID], "music.")) {
      return std::string("");
  }
  return std::string("");
}
}}
