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

#include "apistatisticsservlet.h"

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiStatisticsServlet::logger ( log4cxx::Logger::getLogger ( "squawk.api.ApiStatisticsServlet" ) );

void ApiStatisticsServlet::do_get ( http::HttpRequest &, http::HttpResponse & response ) {
    response << "{";
    try {
        //Get the Albums Count
        db::db_statement_ptr stmt_albums = db->prepareStatement( squawk::sql::QUERY_ALBUMS_COUNT );
        while( stmt_albums->step() ) {
            response << "\"albums\":" << std::to_string( stmt_albums->get_int(0) );
        }

        //Get the Artist Count
        db::db_statement_ptr stmt_artists = db->prepareStatement( squawk::sql::QUERY_ARTISTS_COUNT );
        while( stmt_artists->step() ) {
            response << ", \"artists\":" << std::to_string(stmt_artists->get_int(0));
        }

        //Get the Files Count
        response << ", \"audiofiles\":{";
        bool first_audiofile = true;
        db::db_statement_ptr stmt_audiofiles = db->prepareStatement( squawk::sql::QUERY_AUDIOFILES_COUNT );
        while( stmt_audiofiles->step() ) {
            if( first_audiofile ){
                first_audiofile = false;
            } else response << ", ";
            response << "\"" << stmt_audiofiles->get_string(0) << "\":" << std::to_string(stmt_audiofiles->get_int(1));
        }

        //Get the Files Count
        response << "}, \"types\":{";
        bool first_mimetype = true;
        db::db_statement_ptr stmt_type = db->prepareStatement( squawk::sql::QUERY_TYPES_COUNT );
        while( stmt_type->step() ) {
            if( first_mimetype ){
                first_mimetype = false;
            } else response << ", ";
            if( stmt_type->get_int(0) == IMAGE ) response << "\"IMAGE\":";
            if( stmt_type->get_int(0) == AUDIO ) response << "\"AUDIO\":";
            if( stmt_type->get_int(0) == VIDEO ) response << "\"VIDEO\":";
            if( stmt_type->get_int(0) == BOOK ) response << "\"BOOK\":";
            if( stmt_type->get_int(0) == AUDIO_COVER ) response << "\"AUDIO_COVER\":";
            if( stmt_type->get_int(0) == AUDIO_IMAGE ) response << "\"AUDIO_IMAGE\":";
            response << std::to_string(stmt_type->get_int(1));
        }
        response << "}}";
    } catch( db::DbException & e) {
        LOG4CXX_FATAL(logger, "Can not get statistic, Exception:" << e.code() << "-> " << e.what());
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_statistic.");
        throw;
    }
    response.set_mime_type( ::http::mime::JSON );
    response.status( ::http::http_status::OK );
}
} // api
} // squawk
