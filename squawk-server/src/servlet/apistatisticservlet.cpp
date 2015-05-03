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

#include "apistatisticservlet.h"

#define QUERY_ALBUMS_COUNT "select count(*) from tbl_cds_albums"
#define QUERY_ARTISTS_COUNT "select count(*) from tbl_cds_artists"
#define QUERY_AUDIOFILES_COUNT "select mime_type, count(mime_type) from tbl_cds_audiofiles GROUP BY mime_type"

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr ApiStatisticServlet::logger(log4cxx::Logger::getLogger("squawk.servlet.ApiStatisticServlet"));

void ApiStatisticServlet::do_get( http::HttpRequest&, ::http::HttpResponse & response ) {
    response << "{";
    squawk::db::Sqlite3Statement * stmt_albums = NULL;
    squawk::db::Sqlite3Statement * stmt_artists = NULL;
    squawk::db::Sqlite3Statement * stmt_audiofiles = NULL;
    try {
        //Get the Albums Count
        stmt_albums = db->prepare_statement( QUERY_ALBUMS_COUNT );
        while( stmt_albums->step() ) {
            response << "\"albums\":" << std::to_string( stmt_albums->get_int(0) );
        }
        stmt_albums->reset();
        db->release_statement( stmt_albums );

        //Get the Artist Count
        stmt_artists = db->prepare_statement( QUERY_ARTISTS_COUNT );
        while( stmt_artists->step() ) {
            response << ", \"artists\":" << std::to_string(stmt_artists->get_int(0));
        }
        stmt_artists->reset();
        db->release_statement( stmt_artists );

        //Get the Files Count
        response << ", \"audiofiles\":{";
        bool first_audiofile = true;
        stmt_audiofiles = db->prepare_statement( QUERY_AUDIOFILES_COUNT );
        while( stmt_audiofiles->step() ) {
            if( first_audiofile ){
                first_audiofile = false;
            } else response << ", ";
            response << "\"" << stmt_audiofiles->get_string(0) << "\":" << std::to_string(stmt_audiofiles->get_int(1));
        }
        stmt_audiofiles->reset();
        db->release_statement( stmt_audiofiles );

        response << "}}";
    } catch( squawk::db::DbException & e) {
        LOG4CXX_FATAL(logger, "Can not get statistic, Exception:" << e.code() << "-> " << e.what());
        if(stmt_albums != NULL) db->release_statement( stmt_albums );
        if(stmt_artists != NULL) db->release_statement( stmt_artists );
        if(stmt_audiofiles != NULL) db->release_statement( stmt_audiofiles );
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_statistic.");
        throw;
    }
    response.set_mime_type( ::http::mime::JSON );
    response.status( ::http::http_status::OK );
}
}}
