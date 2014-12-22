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

#include "../db/database.h"

#include <sstream>

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr LetterServlet::logger(log4cxx::Logger::getLogger("squawk.servlet.LetterServlet"));

void LetterServlet::do_get(::http::HttpRequest & request, ::http::HttpResponse & response) {

    squawk::db::Sqlite3Statement * stmt_letter = NULL;
    squawk::db::Sqlite3Statement * stmt_letter_count = NULL;
    try {
        stmt_letter = db->prepare_statement( "select distinct letter from tbl_cds_albums order by letter asc"  );
        stmt_letter_count = db->prepare_statement( "select count(*) from tbl_cds_albums where letter=?"  );

        response << "[";
        bool first = true;
        while( stmt_letter->step() ) {
            if( first ) first = false;
            else response << ",";
            std::string letter = stmt_letter->get_string(0);
            stmt_letter_count->bind_text( 1, letter );
            if( stmt_letter_count->step() ) {
                response << "{\"letter\":\"" << commons::string::escape_json( letter ) <<
                            "\", \"count\":" << commons::string::to_string( stmt_letter_count->get_int(0) ) << "}";
            } else {
                throw(new ::db::DbException(-1, "Can not step count.") );
            }
            stmt_letter_count->reset();
        }
        response << "]";

        db->release_statement(stmt_letter);
        db->release_statement(stmt_letter_count);

    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not get album letters, Exception:" << e->code() << "-> " << e->what());
        if(stmt_letter != NULL) db->release_statement(stmt_letter);
        if(stmt_letter_count != NULL) db->release_statement(stmt_letter_count);
        throw;
    }
    response.set_mime_type( ::http::mime::JSON );
    response.set_status( ::http::http_status::OK );
}
}}
