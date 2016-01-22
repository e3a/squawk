/*
    api artist list servlet implementation.
    Copyright (C) 2014  <e.knecht@netwings.ch>

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

#include "apiartistlistservlet.h"

#include "boost/algorithm/string/case_conv.hpp"

#include "squawk.h"


namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiArtistListServlet::logger ( log4cxx::Logger::getLogger ( "squawk.api.ApiArtistListServlet" ) );

std::string ApiArtistListServlet::QUERY_ARTISTS = "select ROWID, name, letter from tbl_cds_artists order by letter, clean_name";
std::string ApiArtistListServlet::QUERY_ARTISTS_FILTER = "select ROWID, name, letter from tbl_cds_artists where clean_name LIKE ? order by letter, clean_name";

void ApiArtistListServlet::do_get ( http::HttpRequest & request, ::http::HttpResponse & response ) {
	if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "get api artist list." ); }

	try {
        db::db_statement_ptr stmt = nullptr;

        if ( request.containsAttribute ( "name" ) ) {

            stmt = db->prepareStatement ( QUERY_ARTISTS_FILTER );
            stmt->bind_text( 1, "%" + squawk::media::clean_name( request.attribute ( "name" ) ) + "%" ); //TODO clean string

        } else {
            stmt = db->prepareStatement ( QUERY_ARTISTS );
        }

		bool first_artist = true;
		std::string last_letter;

		while ( stmt->step() ) {
			std::string letter = ( stmt->get_string ( 2 ) == "" ? "_" : stmt->get_string ( 2 ) );

			if ( first_artist ) {
				first_artist = false;
				response << "{\"" << letter << "\":[";
				last_letter = letter;

			} else {
				if ( last_letter != stmt->get_string ( 2 ) ) {
					response << "], \"" << letter << "\":[";
					last_letter = letter;

				} else {
					response << ", ";
				}
			}

			response << "{\"id\":" << std::to_string ( stmt->get_int ( 0 ) ) <<
					 ", \"name\":\"" << commons::string::escape_json ( stmt->get_string ( 1 ) ) << "\"}";
		}

		response << "]}";

	} catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not get artists, Exception:" << e.code() << "-> " << e.what() );
		throw;

	} catch ( ... ) {
		LOG4CXX_FATAL ( logger, "Other Excpeption in get_artists." );
		throw;
	}

	response.set_mime_type ( ::http::mime::JSON );
	response.status ( ::http::http_status::OK );
}
}
}
