/*
    get the available letters
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

#include "letterservlet.h"

#include <sstream>

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr LetterServlet::logger(log4cxx::Logger::getLogger("squawk.servlet.LetterServlet"));

void LetterServlet::do_get(::http::HttpRequest & request, ::http::HttpResponse & response) {

    squawk::db::Sqlite3Statement * stmt_images = NULL;
    response << "{";

    try {
        stmt_album = db->prepare_statement( "select distinct letter from tcl_cds_albums"  );
        stmt_album->bind_int(1, album_id);

        while( stmt_album->step() ) {
            response << "\"name\":\"" << commons::string::escape_json(stmt_album->get_string(0)) <<
                        "\", \"genre\":\"" << commons::string::escape_json(stmt_album->get_string(1)) <<
                        "\", \"year\":\"" << stmt_album->get_string(2) << "\", \"id\":" << std::to_string(stmt_album->get_int(3));
        }

        //  get artists
        stmt_artists = db->prepare_statement( QUERY_ARTIST_BY_ALBUM );
        stmt_artists->bind_int(1, album_id);
        response << ", \"artists\":[";
        bool first_artist = true;
        while( stmt_artists->step() ) {
            if( first_artist ){
                first_artist = false;
            } else response << ", ";
            response << "{\"id\":" << std::to_string(stmt_artists->get_int(0)) <<
                        ",  \"name\":\"" << commons::string::escape_json(stmt_artists->get_string(1)) << "\"}";
        }
        //  get songs
        stmt_songs = db->prepare_statement( QUERY_SONGS );
        stmt_songs->bind_int(1, album_id);
        response << "], \"songs\":[";
        bool first_song = true;
        while( stmt_songs->step() ) {
            if( first_song ){
                first_song = false;
            } else response << ", ";
            response << "{\"id\":" << std::to_string(stmt_songs->get_int(0)) <<
                        ", \"title\":\"" << commons::string::escape_json(stmt_songs->get_string(1)) <<
                        "\", \"track\":" << std::to_string(stmt_songs->get_int(2)) <<
                        ", \"filename\":\"" << commons::string::escape_json(stmt_songs->get_string(3)) <<
                        "\", \"length\":" << commons::string::escape_json(stmt_songs->get_string(4)) <<
                        ", \"bitrate\":" << std::to_string(stmt_songs->get_int(5)) <<
                        ", \"samplerate\":" << std::to_string(stmt_songs->get_int(6)) <<
                        ", \"bps\":" << std::to_string(stmt_songs->get_int(7)) <<
                        ", \"channels\":" << std::to_string(stmt_songs->get_int(8)) <<
                        ", \"mimetype\":\"" << commons::string::escape_json(stmt_songs->get_string(9)) <<
                        "\", \"disc\":" << std::to_string(stmt_songs->get_int(10)) << "}";
        }

        //  get images
        stmt_images = db->prepare_statement( QUERY_IMAGES );
        stmt_images->bind_int(1, album_id);
        response << "], \"images\":[";
        bool first_image = true;
        while( stmt_images->step() ) {
            if( first_image ){
                first_image = false;
            } else response << ", ";
            response << std::to_string(stmt_images->get_int(0));
        }

        response << "]}";

        db->release_statement(stmt_album);
        db->release_statement(stmt_artists);
        db->release_statement(stmt_songs);
        db->release_statement(stmt_images);

    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not get album by id, Exception:" << e->code() << "-> " << e->what());
        if(stmt_album != NULL) db->release_statement(stmt_album);
        throw;
    }
    response.set_mime_type( ::http::mime::JSON );
    response.set_status( ::http::http_status::OK );
}
}}
