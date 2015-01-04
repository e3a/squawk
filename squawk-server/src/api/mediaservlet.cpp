/*
    media servlet implementation.
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

#include "mediaservlet.h"
#include "../db/database.h"

#include <sstream>

#include "commons.h"

#define QUERY_SONG "select songs.filename from tbl_cds_audiofiles songs where songs.ROWID = ?"
#define QUERY_VIDEO "select video.filename from tbl_cds_videos video where video.ROWID = ?"

namespace squawk {
namespace api {

log4cxx::LoggerPtr MediaServlet::logger(log4cxx::Logger::getLogger("squawk.api.MediaServlet"));

void MediaServlet::do_get( ::http::HttpRequest & request, ::http::HttpResponse & response ) {

    squawk::db::Sqlite3Statement * stmt_song = NULL;

    std::string type;
    int item_id = 0;
    bool result = match(request.uri, &type, &item_id);
    if(result && item_id > 0) {
        if( type == "song" || type == "video" || type == "image" ) {

            try {

                LOG4CXX_TRACE( logger, type << "::" << item_id )
                if( type == "song" ) {
                    stmt_song = db->prepare_statement( QUERY_SONG );
                } else if( type == "video" ) {
                    stmt_song = db->prepare_statement( QUERY_VIDEO );
                } else if( type == "image" ) {
                    //TODO
                }

                stmt_song->bind_int( 1, item_id );
                if( stmt_song->step() ) {
                    std::string filename = stmt_song->get_string(0);
                    if( ! commons::string::starts_with( filename, mediadirectory )) {
                        throw ::http::http_status::BAD_REQUEST;
                    }

                    LOG4CXX_TRACE( logger, "song_file: " << filename )
                    request.uri = filename.substr( mediadirectory.length(), filename.length() );

                } else {
                    LOG4CXX_TRACE( logger, "file not found: " << type << "::" << item_id )
                }
                db->release_statement( stmt_song );
                FileServlet::do_get(request, response);

            } catch( ::db::DbException * e ) {
                if( stmt_song != NULL ) {
                    db->release_statement( stmt_song );
                }
                LOG4CXX_FATAL( logger, "can not get item path: " << e->code() << ":" << e->what() )
                throw ::http::http_status::INTERNAL_SERVER_ERROR;
            }

        }
    } else {
        throw ::http::http_status::BAD_REQUEST;
    }
}
}}
