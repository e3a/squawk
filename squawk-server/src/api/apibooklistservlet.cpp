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

#include "apibooklistservlet.h"

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiBookListServlet::logger ( log4cxx::Logger::getLogger ( "squawk.api.ApiBookListServlet" ) );

void ApiBookListServlet::do_get ( http::HttpRequest & request, http::HttpResponse & response ) {

    if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "get books: " << request ); }

    try {
            response << "{";
            std::string qBooks = "from tbl_cds_files where type=4 ";
            bool first_query_token = true;

/*            if ( request.containsAttribute ( "artist" ) &&
                            commons::string::is_number ( boost::algorithm::trim_copy ( request.attribute ( "artist" ) ) ) ) { //TODO remove commons
                    first_query_token = false;
                    qBooks = "from tbl_cds_albums albums " \
                                      "JOIN tbl_cds_artists_albums m ON albums.ROWID = m.album_id " \
                                      "where m.artist_id=" + request.attribute ( "artist" );
            }

            if ( request.containsAttribute ( "name" ) ) {
                    if ( first_query_token ) {
                            first_query_token = false;
                            qBooks += " where ";

                    } else { qBooks += " and "; }

                    qBooks += " clean_name like \"%" + request.attribute ( "name" ) + "%\""; //todo clean input
            } */


            if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "select count(*) " + qBooks ); }
            squawk::db::db_statement_ptr stmt_count = db->prepareStatement ( "select count(*) " + qBooks );
            while ( stmt_count->step() ) {
                    response << "\"count\":" << std::to_string ( stmt_count->get_int ( 0 ) );
            }

            //get order mode
            std::string order_mode = "name asc";

            if ( request.containsAttribute ( "order" ) ) {
                    if ( request.attribute ( "order" ) == "insert" ) {
                            order_mode = "timstastamp desc"; //TODO no timestamp on album

                    } else if ( request.attribute ( "order" ) == "year" ) {
                            order_mode = "timstastamp asc";
                    }
            }

            //set the pager
            squawk::db::db_statement_ptr stmt = NULL;

            if ( request.containsAttribute ( "index" ) && commons::string::is_number ( request.attribute ( "index" ) ) &&
                            request.containsAttribute ( "limit" ) &&  commons::string::is_number ( request.attribute ( "limit" ) ) ) {
                    if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "select name, mime_type, isbn, ROWID " + qBooks + " ORDER BY " + order_mode + " LIMIT ?, ?" ); }

                    stmt = db->prepareStatement ( "select name, mime_type, isbn, ROWID " + qBooks + " ORDER BY " + order_mode + " LIMIT ?, ?" );
                    stmt->bind_int ( 1, commons::string::parse_string<int> ( request.attribute ( "index" ) ) );
                    stmt->bind_int ( 2, commons::string::parse_string<int> ( request.attribute ( "limit" ) ) );

            } else {
                    if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "select name, mime_type, isbn, ROWID " + qBooks + " ORDER BY " + order_mode ); }
                    stmt = db->prepareStatement ( "select name, mime_type, isbn, ROWID " + qBooks + " ORDER BY " + order_mode );
            }

            squawk::db::db_statement_ptr stmt_artist = db->prepareStatement ( squawk::sql::QUERY_ARTIST_BY_ALBUM );

            response << ", \"albums\":[";
            bool first_album = true;

            while ( stmt->step() ) {

                    response << ( first_album ? "{" : ",{" );

                    if ( first_album ) { first_album = false; }

                    bool first_attribute = true;

                    if ( ! request.containsAttribute ( "attributes" ) ||
                                    request.attribute ( "attributes" ).find ( "name" ) != std::string::npos ) {
                            if ( first_attribute ) { first_attribute = false; }

                            else { response << ","; }

                            response << "\"name\":\"" << commons::string::escape_json ( stmt->get_string ( 0 ) ) << "\"";
                    }

                    if ( ! request.containsAttribute ( "attributes" ) ||
                                    request.attribute ( "attributes" ).find ( "mime_type" ) != std::string::npos ) {
                            if ( first_attribute ) { first_attribute = false; }

                            else { response << ","; }

                            response << "\"mime_type\":\"" << commons::string::escape_json ( stmt->get_string ( 1 ) ) << "\"";
                    }

                    if ( ! request.containsAttribute ( "attributes" ) ||
                                    request.attribute ( "attributes" ).find ( "isbn" ) != std::string::npos ) {
                            if ( first_attribute ) { first_attribute = false; }

                            else { response << ","; }

                            response << "\"isbn\":\"" << commons::string::escape_json ( stmt->get_string ( 2 ) ) << "\"";
                    }

                    if ( ! request.containsAttribute ( "attributes" ) ||
                                    request.attribute ( "attributes" ).find ( "id" ) != std::string::npos ) {
                            if ( first_attribute ) { first_attribute = false; }

                            else { response << ","; }

                            response << "\"id\":" << std::to_string ( stmt->get_int ( 3 ) );
                    }

                    response << "}";
            }

            response << "]}";


		} catch ( squawk::db::DbException & e ) {
                        LOG4CXX_FATAL ( logger, "Can not get book list, Exception:" << e.code() << "-> " << e.what() );
			throw;
		}

	response.set_mime_type ( http::mime::JSON );
	response.status ( http::http_status::OK );
}
} // api
} // squawk

