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

#include "apivideoitemservlet.h"

#include "squawk.h"
#include "commons.h"

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiVideoItemServlet::logger ( log4cxx::Logger::getLogger ( "squawk.api.ApiVideoItemServlet" ) );

void ApiVideoItemServlet::do_get ( http::HttpRequest & request, http::HttpResponse & response ) {

    int video_id = 0;
    bool result = match ( request.uri(), &video_id );

    if ( result && video_id > 0 ) {

        try {
           //  = "select name, filename, mime_type, duration, filesize, sampleFrequency, width, height, bitrate, channels
            db::db_statement_ptr stmt_video = db->prepareStatement ( squawk::sql::QUERY_VIDEO );
            stmt_video->bind_int ( 1, video_id );

                while ( stmt_video->step() ) {
            response << "{\"name\":\"" << commons::string::escape_json ( stmt_video->get_string ( 0 ) ) <<
                     "\", \"mime_type\":\"" << commons::string::escape_json ( stmt_video->get_string ( 2 ) ) <<
                     "\", \"duration\":\"" << stmt_video->get_string ( 3 ) <<
                        "\", \"size\":" << commons::string::to_string<int> ( stmt_video->get_int ( 4 ) ) <<
                    ", \"sample_frequency\":" << std::to_string ( stmt_video->get_int ( 5 ) ) <<
                    ", \"width\":" << commons::string::to_string<int> ( stmt_video->get_int ( 6 ) ) <<
                    ", \"height\":" << commons::string::to_string<int> ( stmt_video->get_int ( 7 ) ) <<
                        ", \"bitrate\":" << commons::string::to_string<int> ( stmt_video->get_int ( 8 ) ) <<
                        ", \"channels\":" << commons::string::to_string<int> ( stmt_video->get_int ( 9 ) ) <<
                    ", \"ext\":\"" << http::mime::extension( stmt_video->get_string ( 2 ) ) << "\"}";
                }

        } catch ( db::DbException & e ) {
                LOG4CXX_FATAL ( logger, "Can not get videos, Exception:" << e.code() << "-> " << e.what() );
                throw http::http_status::INTERNAL_SERVER_ERROR;

        } catch ( ... ) {
                LOG4CXX_FATAL ( logger, "Can not get videos" );
                throw http::http_status::INTERNAL_SERVER_ERROR;
        }

    } else {
            throw http::http_status::BAD_REQUEST;
    }

    response.set_mime_type ( http::mime::JSON );
    response.status ( http::http_status::OK );
}
} // api
} // squawk
