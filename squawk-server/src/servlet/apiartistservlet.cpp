/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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

#include "apiartistservlet.h"
#include "../db/database.h"

#include "commons.h"

#define QUERY_ARTISTS "select ROWID, name, letter from tbl_cds_artists order by letter, clean_name"
#define QUERY_ARTISTS_FILTER "select ROWID, name, letter from tbl_cds_artists where name LIKE ? order by letter, clean_name"

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr ApiArtistServlet::logger(log4cxx::Logger::getLogger("squawk.servlet.ApiArtistServlet"));

void ApiArtistServlet::do_get( http::HttpRequest & request, ::http::HttpResponse & response ) {
    std::cout << "get artists:\n" << request << std::endl;
    squawk::db::Sqlite3Statement * stmt = NULL;
    try {
        /* TODO if(  ) {
        } else */ stmt = db->prepare_statement( QUERY_ARTISTS );

        response << "[";
        bool first_artist = true;
        while( stmt->step() ) {
            if( first_artist ){
                first_artist = false;
            } else response << ", ";
            response << "{\"id\":" << std::to_string(stmt->get_int(0)) <<
                        ", \"name\":\"" << commons::string::escape_json(stmt->get_string(1)) <<
                        "\", \"letter\":\"" << stmt->get_string(2) << "\"}";
        }
        stmt->reset();
        db->release_statement(stmt);
        response << "]";

    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not get artists, Exception:" << e->code() << "-> " << e->what());
        if(stmt != NULL) db->release_statement(stmt);
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_artists.");
        throw;
    }
    response.set_mime_type( ::http::mime::JSON );
    response.set_status( ::http::http_status::OK );
}
}}
