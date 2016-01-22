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

#include "apialbumlistservlet.h"

#include "squawk.h"

#include <boost/algorithm/string/trim.hpp>

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiAlbumListServlet::logger ( log4cxx::Logger::getLogger ( "squawk.api.ApiAlbumListServlet" ) );

void ApiAlbumListServlet::do_get ( http::HttpRequest & request, http::HttpResponse & response ) {
	if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "get albums: " << request ); }

	try {
		response << "{";
		std::string qAlbums = "from tbl_cds_albums albums ";
		bool first_query_token = true;

		if ( request.containsAttribute ( "artist" ) &&
				commons::string::is_number ( boost::algorithm::trim_copy ( request.attribute ( "artist" ) ) ) ) { //TODO remove commons
			first_query_token = false;
			qAlbums = "from tbl_cds_albums albums " \
					  "JOIN tbl_cds_artists_albums m ON albums.ROWID = m.album_id " \
					  "where m.artist_id=" + request.attribute ( "artist" );
		}

		if ( request.containsAttribute ( "name" ) ) {
			if ( first_query_token ) {
				first_query_token = false;
				qAlbums += " where ";

			} else { qAlbums += " and "; }

			qAlbums += " clean_name like \"%" + request.attribute ( "name" ) + "%\""; //todo clean input
		}

		if ( request.containsAttribute ( "genre" ) ) {
			if ( first_query_token ) {
				first_query_token = false;
				qAlbums += " where ";

			} else { qAlbums += " and "; }

			qAlbums += " genre = \"" + request.attribute ( "genre" ) + "\"";
		}

		if ( request.containsAttribute ( "year" ) &&
				commons::string::is_number ( request.attribute ( "year" ) ) ) {
			if ( first_query_token ) {
				// last condition first_query_token = false;
				qAlbums += " where ";

			} else { qAlbums += " and "; }

			qAlbums += " year = \"" + request.attribute ( "year" ) + "\"";
		}

		if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "select count(*) " + qAlbums ); }

         db::db_statement_ptr stmt_count = db->prepareStatement ( "select count(*) " + qAlbums );

		while ( stmt_count->step() ) {
			response << "\"count\":" << std::to_string ( stmt_count->get_int ( 0 ) );
		}

		//get order mode
		std::string order_mode = "clean_name asc";

		if ( request.containsAttribute ( "order" ) ) {
			if ( request.attribute ( "order" ) == "insert" ) {
				order_mode = "timstastamp desc"; //TODO no timestamp on album

			} else if ( request.attribute ( "order" ) == "year" ) {
				order_mode = "timstastamp asc";
			}
		}

		//set the pager
         db::db_statement_ptr stmt = NULL;

		if ( request.containsAttribute ( "index" ) && commons::string::is_number ( request.attribute ( "index" ) ) &&
				request.containsAttribute ( "limit" ) &&  commons::string::is_number ( request.attribute ( "limit" ) ) ) {
			if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "select albums.name, albums.genre, albums.year, albums.ROWID " + qAlbums + " ORDER BY " + order_mode + " LIMIT ?, ?" ); }

			stmt = db->prepareStatement ( "select albums.name, albums.genre, albums.year, albums.ROWID " + qAlbums + " ORDER BY " + order_mode + " LIMIT ?, ?" );
			stmt->bind_int ( 1, commons::string::parse_string<int> ( request.attribute ( "index" ) ) );
			stmt->bind_int ( 2, commons::string::parse_string<int> ( request.attribute ( "limit" ) ) );

		} else {
			if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "select albums.name, albums.genre, albums.year, albums.ROWID " + qAlbums + " ORDER BY " + order_mode ); }

			stmt = db->prepareStatement ( "select albums.name, albums.genre, albums.year, albums.ROWID " + qAlbums + " ORDER BY " + order_mode );
		}

         db::db_statement_ptr stmt_artist = db->prepareStatement ( squawk::sql::QUERY_ARTIST_BY_ALBUM );

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
					request.attribute ( "attributes" ).find ( "genre" ) != std::string::npos ) {
				if ( first_attribute ) { first_attribute = false; }

				else { response << ","; }

				response << "\"genre\":\"" << commons::string::escape_json ( stmt->get_string ( 1 ) ) << "\"";
			}

			if ( ! request.containsAttribute ( "attributes" ) ||
					request.attribute ( "attributes" ).find ( "year" ) != std::string::npos ) {
				if ( first_attribute ) { first_attribute = false; }

				else { response << ","; }

				response << "\"year\":\"" << commons::string::escape_json ( stmt->get_string ( 2 ) ) << "\"";
			}

			if ( ! request.containsAttribute ( "attributes" ) ||
					request.attribute ( "attributes" ).find ( "id" ) != std::string::npos ) {
				if ( first_attribute ) { first_attribute = false; }

				else { response << ","; }

				response << "\"id\":" << std::to_string ( stmt->get_int ( 3 ) );
			}

			if ( ! request.containsAttribute ( "attributes" ) ||
					request.attribute ( "attributes" ).find ( "artist" ) != std::string::npos ) {
				response << ", \"artists\":[";
				stmt_artist->bind_int ( 1, stmt->get_int ( 3 ) );
				bool first_artist = true;

				while ( stmt_artist->step() ) {
					if ( first_artist ) {
						first_artist = false;

					} else { response << ", "; }

					response << "{\"id\":" << std::to_string ( stmt_artist->get_int ( 0 ) ) <<
							 ",\"name\":\"" << commons::string::escape_json ( stmt_artist->get_string ( 1 ) ) << "\"}";
				}

				response << "]";
				stmt_artist->reset();
			}

			response << "}";
		}

		response << "]}";

    } catch (  db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not get albums, Exception:" << e.code() << "-> " << e.what() );
		throw http::http_status::INTERNAL_SERVER_ERROR;

	} catch ( ... ) {
		LOG4CXX_FATAL ( logger, "Other Excpeption in get_albums." );
		throw http::http_status::INTERNAL_SERVER_ERROR;
	}

	response.set_mime_type ( http::mime::JSON );
	response.status ( http::http_status::OK );
}
} // api
} // squawk
