/*
    Upnp music directory module.
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
#ifndef UPNPMUSICDIRECTORYMODULE_H
#define UPNPMUSICDIRECTORYMODULE_H

#include "squawk.h"
#include "../squawkconfig.h"
#include "../db/sqlite3database.h"
#include <upnp.h>

#include <functional>
#include <iostream>

#include "log4cxx/logger.h"

namespace squawk {
namespace upnp {

class UpnpMusicDirectoryModule : public commons::upnp::ContentDirectoryModule {
public:

    UpnpMusicDirectoryModule(squawk::SquawkConfig * squawk_config, squawk::db::Sqlite3Database * db ) : db(db), squawk_config(squawk_config) {}
    virtual void getRootNode( commons::xml::XMLWriter * xmlWriter, commons::xml::Node * element, commons::upnp::CdsResult * cds_result );
    virtual bool match( commons::upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual void parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result, commons::upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    squawk::db::Sqlite3Database * db;
    squawk::SquawkConfig * squawk_config;
    static log4cxx::LoggerPtr logger;

    /* artist count  */
    static constexpr const char * SQL_COUNT_ARTISTS = "select count(*) from tbl_cds_artists";
    int artistCount() {
        int artist_count = 0;
        squawk::db::Sqlite3Statement * stmt_artists_count = NULL;
        try {
            stmt_artists_count = db->prepare_statement( SQL_COUNT_ARTISTS );
            if( stmt_artists_count->step() ) {
                artist_count = stmt_artists_count->get_int( 0 );
            }
            db->release_statement( stmt_artists_count );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get artist_count, Exception:" << e.code() << "-> " << e.what());
            if( stmt_artists_count != NULL ) db->release_statement( stmt_artists_count );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in get_artist_count.");
            if( stmt_artists_count != NULL ) db->release_statement( stmt_artists_count );
            throw;
        }
        return artist_count;
    }

    /* album count  */
    static constexpr const char * SQL_COUNT_ALBUMS = "select count(*) from tbl_cds_albums";
    int albumCount() {
        int album_count = 0;
        squawk::db::Sqlite3Statement * stmt_albums_count = NULL;
        try {
            stmt_albums_count = db->prepare_statement( SQL_COUNT_ALBUMS );
            if( stmt_albums_count->step() ) {
                album_count = stmt_albums_count->get_int( 0 );
            }
            db->release_statement( stmt_albums_count );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get albums_count, Exception:" << e.code() << "-> " << e.what());
            if( stmt_albums_count != NULL ) db->release_statement( stmt_albums_count );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in get_album_count.");
            if( stmt_albums_count != NULL ) db->release_statement( stmt_albums_count );
            throw;
        }
        return album_count;
    }

    /* album by artist count  */
    static constexpr const char * SQL_COUNT_ALBUMS_ARTIST =
        "select count(*) from tbl_cds_albums album, tbl_cds_artists_albums m, tbl_cds_artists artist " \
            "where artist.ROWID = ? and album.ROWID = m.album_id and artist.ROWID = m.artist_id";
    int albumByArtistCount( int artist_id ) {
        int album_count = 0;
        squawk::db::Sqlite3Statement * stmt_albums_artist_count = NULL;
        try {
            stmt_albums_artist_count = db->prepare_statement( SQL_COUNT_ALBUMS_ARTIST );
            stmt_albums_artist_count->bind_int( 1, artist_id );
            if( stmt_albums_artist_count->step() ) {
                album_count = stmt_albums_artist_count->get_int( 0 );
            }
            db->release_statement( stmt_albums_artist_count );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get albums_artist_count, Exception:" << e.code() << "-> " << e.what());
            if( stmt_albums_artist_count != NULL ) db->release_statement( stmt_albums_artist_count );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in get_album_artist_count.");
            if( stmt_albums_artist_count != NULL ) db->release_statement( stmt_albums_artist_count );
            throw;
        }
        return album_count;
    }

    /* song by album count  */
    static constexpr const char * SQL_COUNT_SONGS_ALBUM =
        "select count(*) from tbl_cds_audiofiles songs, tbl_cds_albums album where album.ROWID = ? and album.ROWID = songs.album_id";
    int songByAlbumCount( const int album_id ) {
        squawk::db::Sqlite3Statement * stmt_song_album_count = NULL;
        int song_count = 0;
        try {
            stmt_song_album_count = db->prepare_statement( SQL_COUNT_SONGS_ALBUM );
            stmt_song_album_count->bind_int( 1, album_id );
            if( stmt_song_album_count->step() ) {
                song_count = stmt_song_album_count->get_int( 0 );
            }
            db->release_statement( stmt_song_album_count );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get song_album_count, Exception:" << e.code() << "-> " << e.what());
            if( stmt_song_album_count != NULL ) db->release_statement( stmt_song_album_count );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in song_album_artist_count.");
            if( stmt_song_album_count != NULL ) db->release_statement( stmt_song_album_count );
            throw;
        }
        std::cout << "song by album count: " << album_id << " = " << song_count << std::endl;
        return song_count;
    }

    /* get artists */
    static constexpr const char * SQL_ARTIST = "select ROWID, name from tbl_cds_artists order by name LIMIT ?, ?";
    void artists( const int & start_index, const int & request_count, std::function<void(const int, const std::string)> callback ) {
        squawk::db::Sqlite3Statement * stmt_artists = NULL;
        try {
            stmt_artists = db->prepare_statement( SQL_ARTIST );
            stmt_artists->bind_int( 1, start_index );
            stmt_artists->bind_int( 2, request_count );
            while( stmt_artists->step() ) {
                callback( stmt_artists->get_int( 0 ), stmt_artists->get_string(1) );
            }
            db->release_statement( stmt_artists );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get artist, Exception:" << e.code() << "-> " << e.what());
            if( stmt_artists != NULL ) db->release_statement( stmt_artists );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in artist.");
            if( stmt_artists != NULL ) db->release_statement( stmt_artists );
            throw;
        }
    }

    /* get albums */
    static constexpr const char * SQL_ALBUM = "select name, genre, year, ROWID from tbl_cds_albums ORDER BY name LIMIT ?, ?";
    void albums( const int & index, const int result_count,
                 std::function<void(const int, const std::string, const std::string, const std::string)> callback ) {
        squawk::db::Sqlite3Statement * stmt_albums = NULL;
        try {
            stmt_albums = db->prepare_statement( SQL_ALBUM );
            stmt_albums->bind_int( 1, index );
            stmt_albums->bind_int( 2, result_count );
            std::cout << "start stmt:albums" << std::endl;
            while( stmt_albums->step() ) {
                callback( stmt_albums->get_int( 3 ), stmt_albums->get_string(0),
                          stmt_albums->get_string(1), stmt_albums->get_string(2) );
            }
            std::cout << "endt stmt:albums" << std::endl;
            db->release_statement( stmt_albums );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get albums, Exception:" << e.code() << "-> " << e.what());
            if( stmt_albums != NULL ) db->release_statement( stmt_albums );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in albums.");
            if( stmt_albums != NULL ) db->release_statement( stmt_albums );
            throw;
        }
    }

    /* get albums by artists */
    static constexpr const char * SQL_ARTIST_ALBUM = "select album.name, album.genre, album.year, album.ROWID " \
            "from tbl_cds_albums album, tbl_cds_artists_albums m, tbl_cds_artists artist " \
            "where artist.ROWID = ? and album.ROWID = m.album_id and artist.ROWID = m.artist_id LIMIT ?,?";
    void albums( const int & artist_id, const int & start_index, const int & request_count,
                 std::function<void(const int, const std::string, const std::string, const std::string)> callback ) {
        squawk::db::Sqlite3Statement * stmt_artist_albums = NULL;
        try {
            stmt_artist_albums = db->prepare_statement( SQL_ARTIST_ALBUM );
            stmt_artist_albums->bind_int( 1, artist_id );
            stmt_artist_albums->bind_int( 2, start_index );
            stmt_artist_albums->bind_int( 3, request_count );
            while( stmt_artist_albums->step() ) {
                callback( stmt_artist_albums->get_int( 3 ), stmt_artist_albums->get_string(0),
                          stmt_artist_albums->get_string(1), stmt_artist_albums->get_string(2) );
            }
            db->release_statement( stmt_artist_albums );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get albums by artist, Exception:" << e.code() << "-> " << e.what());
            if( stmt_artist_albums != NULL ) db->release_statement( stmt_artist_albums );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in albums by artist.");
            if( stmt_artist_albums != NULL ) db->release_statement( stmt_artist_albums );
            throw;
        }
    }

    /* get album */
    static constexpr const char * SQL_ALBUM_ID = "select album.name, album.genre, album.year, album.ROWID " \
            "from tbl_cds_albums album where album.ROWID = ?";
    void album( const int album_id, std::function<void(const int, const std::string, const std::string, const std::string)> callback ) {
        squawk::db::Sqlite3Statement * stmt_album = NULL;
        try {
            stmt_album = db->prepare_statement( SQL_ALBUM_ID );
            stmt_album->bind_int( 1, album_id );
            while( stmt_album->step() ) {
                callback( stmt_album->get_int( 3 ), stmt_album->get_string(0),
                          stmt_album->get_string(1), stmt_album->get_string(2) );
            }
            db->release_statement( stmt_album );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get album, Exception:" << e.code() << "-> " << e.what());
            if( stmt_album != NULL ) db->release_statement( stmt_album );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in album.");
            if( stmt_album != NULL ) db->release_statement( stmt_album );
            throw;
        }
    }

    /* get artists for album */
    static constexpr const char * SQL_ALBUM_ARTIST = "select artist.ROWID, artist.name from tbl_cds_artists artist " \
            "JOIN tbl_cds_artists_albums m ON artist.ROWID = m.artist_id where m.album_id=?";
    void artist( const int album_id, std::function<void(const int, const std::string)> callback ) {
        squawk::db::Sqlite3Statement * stmt_album_artist = NULL;
        try {
            stmt_album_artist = db->prepare_statement( SQL_ALBUM_ARTIST );
            stmt_album_artist->bind_int( 1, album_id );
            while( stmt_album_artist->step() ) {
                callback( stmt_album_artist->get_int(0), stmt_album_artist->get_string(1) );
            }
            db->release_statement( stmt_album_artist );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get artists for album, Exception:" << e.code() << "-> " << e.what());
            if( stmt_album_artist != NULL ) db->release_statement( stmt_album_artist );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in artists for album.");
            if( stmt_album_artist != NULL ) db->release_statement( stmt_album_artist );
            throw;
        }
    }

    /* get songs for album */
    static constexpr const char * SQL_ALBUM_SONG = "select songs.ROWID, songs.title, songs.track, songs.filename, " \
            "songs.length, songs.bitrate, songs.sample_rate, songs.bits_per_sample, songs.channels, " \
            "songs.mime_type, songs.disc, songs.mtime, songs.filesize " \
            "from tbl_cds_audiofiles songs, tbl_cds_albums album where album.ROWID = ? and " \
            "album.ROWID = songs.album_id order by songs.track, songs.disc";
    void songs( const int album_id, std::function<void(const int, const int, const std::string, const std::string, const std::string,
                                                       const std::string, const int, const int, const int, const int, const int)> callback ) {

        squawk::db::Sqlite3Statement * stmt_album_song = NULL;
        try {
            stmt_album_song = db->prepare_statement( SQL_ALBUM_SONG );
            stmt_album_song->bind_int( 1, album_id );
            while( stmt_album_song->step() ) {
                callback( stmt_album_song->get_int(0), stmt_album_song->get_int(2), stmt_album_song->get_string(1),
                          stmt_album_song->get_string(1), stmt_album_song->get_string(11), stmt_album_song->get_string(9),
                          stmt_album_song->get_int(12), stmt_album_song->get_int(4), stmt_album_song->get_int(5),
                          stmt_album_song->get_int(6), stmt_album_song->get_int(7) );
            }
            db->release_statement( stmt_album_song );

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get songs for album, Exception:" << e.code() << "-> " << e.what());
            if( stmt_album_song != NULL ) db->release_statement( stmt_album_song );
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Exception in songs for album.");
            if( stmt_album_song != NULL ) db->release_statement( stmt_album_song );
            throw;
        }
    }
};
}}
#endif // UPNPMUSICDIRECTORYMODULE_H
