/*
    API Browse Items Servlet
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

#include "apibrowseservlet.h"

#include "commons.h"
#include "squawk.h"

#define QUERY_ALBUMS_COUNT "select count(*) from tbl_cds_albums"
#define QUERY_ITEM "select ROWID, type, name, mime_type from tbl_cds_files where parent = ? and type in (0, ?)"

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiBrowseServlet::logger ( log4cxx::Logger::getLogger ( "squawk.api.ApiBrowseServlet" ) );

void ApiBrowseServlet::do_get ( http::HttpRequest & request, ::http::HttpResponse & response ) {

	std::cout << "get albums:\n" << request << std::endl;

	std::string strType, strId;
	int type = 0;
	int id = 0;
	bool result = match ( request.uri(), &strType, &strId );

	if ( result ) {
		id = ( strId.length() == 0 ? 0 : commons::string::parse_string<int> ( strId ) );

		if ( strType == "image" ) {
			type = IMAGE;

		} else if ( strType == "video" ) {
			type = VIDEO;
		}

		try {
            db::db_statement_ptr stmt_items = db->prepareStatement ( QUERY_ITEM );
			stmt_items->bind_int ( 1, id );
			stmt_items->bind_int ( 2, type );

			response << "[";
			bool first_item = true;

			while ( stmt_items->step() ) {
				if ( first_item ) { first_item = false; }

				else { response << ","; }

				response << "{\"id\":" << commons::string::to_string ( stmt_items->get_int ( 0 ) ) <<
						 ",\"type\":\"" << commons::string::to_string ( stmt_items->get_int ( 1 ) ) <<
						 "\",\"name\":\"" << commons::string::escape_json ( stmt_items->get_string ( 2 ) ) << "\"";

				if ( stmt_items->get_int ( 1 ) > 0 )
					response <<  ",\"url\":\"" << "/file/" << strType << "/" << commons::string::to_string ( stmt_items->get_int ( 0 ) ) << "." <<
							 http::mime::extension ( stmt_items->get_string ( 3 ) ) << "\"";

				response << "}";
			}

			response << "]";

			stmt_items->reset();

        } catch (  db::DbException & e ) {
			LOG4CXX_FATAL ( logger, "Can not get albums, Exception:" << e.code() << "-> " << e.what() );
			throw http::http_status::INTERNAL_SERVER_ERROR;

		} catch ( ... ) {
			LOG4CXX_FATAL ( logger, "Other Excpeption in get_albums." );
			throw http::http_status::INTERNAL_SERVER_ERROR;
		}

		response.set_mime_type ( ::http::mime::JSON );
		response.status ( ::http::http_status::OK );

	} else {
		response.status ( ::http::http_status::NOT_FOUND );
	}
}
}
}
