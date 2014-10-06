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

        stmt_song = db->prepare_statement( QUERY_SONG );
        if( stmt_song->step() ) {
            std::string filename = stmt_song->get_string(0);
            if( ! commons::string::starts_with( filename, mediadirectory )) {
                throw ::http::http_status::BAD_REQUEST;
            }
            request.uri = filename.substr( mediadirectory.length(), filename.length() );
        }
        FileServlet::do_get(request, response);

    } else {
        throw ::http::http_status::BAD_REQUEST;
    }
}
}}
