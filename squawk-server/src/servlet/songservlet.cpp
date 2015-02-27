/*
    song servlet implementation.
    Copyright (C) 2013  e.knecht@netwings.ch

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

#include "songservlet.h"
#include "../db/database.h"

#include <sstream>

#include "commons.h"

#define QUERY_SONG "select songs.filename from tbl_cds_audiofiles songs where songs.ROWID = ?"

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr SongServlet::logger(log4cxx::Logger::getLogger("squawk.servlet.SongServlet"));

void SongServlet::do_get( ::http::HttpRequest & request, ::http::HttpResponse & response ) {

    squawk::db::Sqlite3Statement * stmt_song = NULL;

    int song_id = 0;
    bool result = match(request.uri, &song_id);
    if(result && song_id > 0) {

        try {

            LOG4CXX_TRACE( logger, "song_id: " << song_id )

            stmt_song = db->prepare_statement( QUERY_SONG );
            stmt_song->bind_int( 1, song_id );
            if( stmt_song->step() ) {
                request.uri = stmt_song->get_string(0);

            } else {
                LOG4CXX_TRACE( logger, "file not found: " << song_id )
            }
            db->release_statement( stmt_song );
            FileServlet::do_get(request, response);

        } catch( ::db::DbException * e ) {
            if( stmt_song != NULL ) {
                db->release_statement( stmt_song );
            }
            LOG4CXX_FATAL( logger, "can not get song path: " << e->code() << ":" << e->what() )
            throw ::http::http_status::INTERNAL_SERVER_ERROR;
        }

    } else {
        throw ::http::http_status::BAD_REQUEST;
    }
}
}}
