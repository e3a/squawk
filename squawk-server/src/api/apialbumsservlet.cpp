/*
    API Albums Servlet
    Copyright (C) 2013  <copyright holder> <e.knecht@netwings.ch>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "apialbumsservlet.h"
#include "../db/database.h"

#include "commons.h"

#define QUERY_ALBUMS_COUNT "select count(*) from tbl_cds_albums"
#define QUERY_ALBUMS "select name, genre, year, ROWID from tbl_cds_albums ORDER BY name"
#define QUERY_ALBUMS_PAGE "select name, genre, year, ROWID from tbl_cds_albums ORDER BY name LIMIT ?, ?"
#define QUERY_ARTIST_BY_ALBUM "select artist.ROWID, artist.name from tbl_cds_artists artist " \
                              "JOIN tbl_cds_artists_albums m ON artist.ROWID = m.artist_id " \
                              "where m.album_id=?"

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr ApiAlbumsServlet::logger(log4cxx::Logger::getLogger("squawk.servlet.ApiAlbumsServlet"));

void ApiAlbumsServlet::do_get( http::HttpRequest & request, ::http::HttpResponse & response ) {
    std::cout << "get albums:\n" << request << std::endl;
    squawk::db::Sqlite3Statement * stmt_count = NULL;
    squawk::db::Sqlite3Statement * stmt = NULL;
    squawk::db::Sqlite3Statement * stmt_artist = NULL;
    try {
        response << "{";

        if( request.parameters.find("attributes")==request.parameters.end() ||
            request.parameters["attributes"].find("count") != std::string::npos ) {

            stmt_count = db->prepare_statement( QUERY_ALBUMS_COUNT );
            while( stmt_count->step() ) {
                response << "\"count\":" << std::to_string( stmt_count->get_int(0) );
            }
        }

        if(request.parameters.find("index")!=request.parameters.end() &&
           request.parameters.find("limit")!=request.parameters.end() ) {
            stmt = db->prepare_statement( QUERY_ALBUMS_PAGE );
            stmt->bind_int(1, commons::string::parse_string<int>(request.parameters["index"]));
            stmt->bind_int(2, commons::string::parse_string<int>(request.parameters["limit"]));
        } else {
            stmt = db->prepare_statement( QUERY_ALBUMS );
        }
        stmt_artist = db->prepare_statement( QUERY_ARTIST_BY_ALBUM );

        if( request.parameters.find("attributes")==request.parameters.end() ||
            ( request.parameters["attributes"].find("name") != std::string::npos ||
              request.parameters["attributes"].find("genre") != std::string::npos ||
              request.parameters["attributes"].find("year") != std::string::npos ||
              request.parameters["attributes"].find("id") != std::string::npos ||
              request.parameters["attributes"].find("artist") != std::string::npos ) ) {

            response << ",\"albums\":[";
            bool first_album = true;
            while( stmt->step() ) {
                if( first_album ){
                    first_album = false;
                } else response << ", ";
                response << "{\"name\":\"" << commons::string::escape_json(stmt->get_string(0)) <<
                            "\", \"genre\":\"" << commons::string::escape_json(stmt->get_string(1)) <<
                            "\", \"year\":\"" << commons::string::escape_json(stmt->get_string(2)) <<
                            "\", \"id\":" << std::to_string(stmt->get_int(3)) << ", \"artists\":[";

                stmt_artist->bind_int(1, stmt->get_int(3));
                bool first_artist = true;
                while( stmt_artist->step() ) {
                    if( first_artist ){
                        first_artist = false;
                    } else response << ", ";

                    response << "{\"id\":" << std::to_string(stmt_artist->get_int(0)) <<
                                ",  \"name\":\"" << commons::string::escape_json(stmt_artist->get_string(1)) << "\"}";
                }
                stmt_artist->reset();
                response << "]}";
            }
            response << "]";

            stmt->reset();
            db->release_statement(stmt_artist);
            db->release_statement(stmt);
        }
        response << "}";

    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not get albums, Exception:" << e->code() << "-> " << e->what());
        if(stmt != NULL) db->release_statement(stmt);
        if(stmt_artist != NULL) db->release_statement(stmt_artist);
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_albums.");
        throw;
    }
    response.set_mime_type( ::http::mime::JSON );
    response.set_status( ::http::http_status::OK );
}
}}
