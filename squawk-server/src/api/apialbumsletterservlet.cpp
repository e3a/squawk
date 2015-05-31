/*
    API Albums Letter Servlet
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

#include "apialbumsletterservlet.h"

#include "squawk.h"

#include <sstream>

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiAlbumsLetterServlet::logger ( log4cxx::Logger::getLogger ( "squawk.api.ApiAlbumsLetterServlet" ) );

void ApiAlbumsLetterServlet::do_get ( http::HttpRequest & /* request */, http::HttpResponse & response ) {
	if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "get api albums letter." ); }

	try {

		squawk::db::db_statement_ptr stmt_letter = db->prepareStatement ( "select distinct letter from tbl_cds_albums order by letter asc" );
		squawk::db::db_statement_ptr stmt_letter_count = db->prepareStatement ( "select count(*) from tbl_cds_albums where letter=?" );

		response << "[";
		bool first = true;

		while ( stmt_letter->step() ) {
			if ( first ) { first = false; }

			else { response << ","; }

			std::string letter = stmt_letter->get_string ( 0 );
			stmt_letter_count->bind_text ( 1, letter );

			if ( stmt_letter_count->step() ) {
				response << "{\"letter\":\"" << commons::string::escape_json ( letter ) <<
						 "\", \"count\":" << commons::string::to_string ( stmt_letter_count->get_int ( 0 ) ) << "}";

			} else {
				throw ( squawk::db::DbException ( -1, "Can not step count." ) );
			}

			stmt_letter_count->reset();
		}

		response << "]";

	} catch ( squawk::db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not get album letters, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}

	response.set_mime_type ( ::http::mime::JSON );
	response.status ( ::http::http_status::OK );
}
}
}
