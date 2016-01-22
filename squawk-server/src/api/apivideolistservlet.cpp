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

#include "apivideolistservlet.h"

#include "squawk.h"
#include "commons.h"

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiVideoListServlet::logger ( log4cxx::Logger::getLogger ( "squawk.api.ApiVideoListServlet" ) );

void ApiVideoListServlet::do_get ( http::HttpRequest&, http::HttpResponse & response ) {

	response << "[";

	try {
        db::db_statement_ptr stmt_video = db->prepareStatement ( squawk::sql::QUERY_VIDEOS );
		bool first_video = true;

		while ( stmt_video->step() ) {
			if ( first_video ) { first_video = false; }

			else { response << ","; }
            // ROWID, name, filename, mime_type, duration, filesize, sampleFrequency, width, height, bitrate, channels
            response << "{\"name\":\"" << commons::string::escape_json ( stmt_video->get_string ( 1 ) ) <<
                     "\", \"mime-type\":\"" << commons::string::escape_json ( stmt_video->get_string ( 3 ) ) <<
                     "\", \"ext\":\"" << http::mime::extension ( stmt_video->get_string ( 3 ) ) <<
                     "\", \"id\":" << commons::string::to_string<int> ( stmt_video->get_int ( 0 ) ) << "}";
		}

		response << "]";

	} catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not get videos, Exception:" << e.code() << "-> " << e.what() );
		throw http::http_status::INTERNAL_SERVER_ERROR;

	} catch ( ... ) {
		LOG4CXX_FATAL ( logger, "Can not get videos" );
		throw http::http_status::INTERNAL_SERVER_ERROR;
	}

	response.set_mime_type ( http::mime::JSON );
	response.status ( http::http_status::OK );
}
} // api
} // squawk
