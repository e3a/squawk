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
#include "../db/sqlite3connection.h"
#include "../db/sqlite3statement.h"

#include <upnp.h>
#include <http.h>

#include <functional>
#include <iostream>

#include "log4cxx/logger.h"

namespace squawk {
namespace upnp {

class UpnpMusicDirectoryModule : public commons::upnp::ContentDirectoryModule {
public:
    UpnpMusicDirectoryModule( http::HttpServletContext context ) :
        db(squawk::db::Sqlite3Database::instance().connection( context.parameter( squawk::CONFIG_DATABASE_FILE ) ) ),
        http_address_( context.parameter( squawk::CONFIG_HTTP_IP ) ), http_port_( context.parameter( squawk::CONFIG_HTTP_PORT ) ) {}
    virtual void getRootNode( commons::xml::XMLWriter * xmlWriter, commons::xml::Node * element, commons::upnp::CdsResult * cds_result );
    virtual bool match( commons::upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual void parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result, commons::upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    static log4cxx::LoggerPtr logger;
    squawk::db::db_connection_ptr db;
    std::string http_address_;
    std::string http_port_;

//TODO    squawk::SquawkConfig * squawk_config;

    /* artist count  */
    int artistCount() {
        int artist_count = 0;
        try {
            squawk::db::db_statement_ptr stmt_artists_count = db->prepareStatement( squawk::sql::QUERY_COUNT_ARTISTS );
            if( stmt_artists_count->step() ) {
                artist_count = stmt_artists_count->get_int( 0 );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get artist_count, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in get_artist_count.");
            throw;
        }
        return artist_count;
    }

    /* album count  */
    int albumCount() {
        int album_count = 0;
        try {
            squawk::db::db_statement_ptr stmt_albums_count = db->prepareStatement( squawk::sql::QUERY_COUNT_ALBUMS );
            if( stmt_albums_count->step() ) {
                album_count = stmt_albums_count->get_int( 0 );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get albums_count, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in get_album_count.");
            throw;
        }
        return album_count;
    }

    /* album by artist count  */
    int albumByArtistCount( int artist_id ) {
        int album_count = 0;
        try {
            squawk::db::db_statement_ptr stmt_albums_artist_count = db->prepareStatement( squawk::sql::QUERY_COUNT_ALBUMS_ARTIST );
            stmt_albums_artist_count->bind_int( 1, artist_id );
            if( stmt_albums_artist_count->step() ) {
                album_count = stmt_albums_artist_count->get_int( 0 );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get albums_artist_count, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in get_album_artist_count.");
            throw;
        }
        return album_count;
    }

    /* song by album count  */
    int songByAlbumCount( const int album_id ) {
        int song_count = 0;
        try {
            squawk::db::db_statement_ptr stmt_song_album_count = db->prepareStatement( squawk::sql::QUERY_COUNT_SONGS_ALBUM );
            stmt_song_album_count->bind_int( 1, album_id );
            if( stmt_song_album_count->step() ) {
                song_count = stmt_song_album_count->get_int( 0 );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get song_album_count, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in song_album_artist_count.");
            throw;
        }
        std::cout << "song by album count: " << album_id << " = " << song_count << std::endl;
        return song_count;
    }

    /* XXX get album */
    void album( const int album_id, std::function<void(const int, const std::string, const std::string, const std::string)> callback ) {
        try {
            squawk::db::db_statement_ptr stmt_album = db->prepareStatement( squawk::sql::SQL_ALBUM_ID );
            stmt_album->bind_int( 1, album_id );
            while( stmt_album->step() ) {
                callback( stmt_album->get_int( 3 ), stmt_album->get_string(0),
                          stmt_album->get_string(1), stmt_album->get_string(2) );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get album, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in album.");
            throw;
        }
    }

    /* get artists for album */
    std::string artist( const int album_id ) const {
        std::string return_value;
        bool isFirst = true;
        try {
            squawk::db::db_statement_ptr stmt_album_artist = db->prepareStatement( squawk::sql::SQL_ALBUM_ARTIST );
            stmt_album_artist->bind_int( 1, album_id );
            while( stmt_album_artist->step() ) {
                if( ! isFirst ) {
                    return_value += ", ";
                } else isFirst = false;
                return_value += stmt_album_artist->get_string( 1 );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get artists for album, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in artists for album.");
            throw;
        }
        return return_value;
    }
    /* TODO remove
    void artist( const int album_id, std::function<void(const int, const std::string)> callback ) {
        try {
            squawk::db::db_statement_ptr stmt_album_artist = db->prepareStatement( SQL_ALBUM_ARTIST );
            stmt_album_artist->bind_int( 1, album_id );
            while( stmt_album_artist->step() ) {
                callback( stmt_album_artist->get_int(0), stmt_album_artist->get_string(1) );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get artists for album, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in artists for album.");
            throw;
        }
    }*/

    /* get songs for album */
    void songs( const int album_id, std::function<void(const int, const int, const std::string, const std::string, const std::string,
                                                       const std::string, const int, const int, const int, const int, const int)> callback ) {

        try {
            squawk::db::db_statement_ptr stmt_album_song = db->prepareStatement( squawk::sql::SQL_ALBUM_SONG );
            stmt_album_song->bind_int( 1, album_id );
            while( stmt_album_song->step() ) {
                callback( stmt_album_song->get_int(0), stmt_album_song->get_int(2), stmt_album_song->get_string(1),
                          stmt_album_song->get_string(1), stmt_album_song->get_string(11), stmt_album_song->get_string(9),
                          stmt_album_song->get_int(12), stmt_album_song->get_int(4), stmt_album_song->get_int(5),
                          stmt_album_song->get_int(6), stmt_album_song->get_int(7) );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get songs for album, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Exception in songs for album.");
            throw;
        }
    }
};
}}
#endif // UPNPMUSICDIRECTORYMODULE_H
