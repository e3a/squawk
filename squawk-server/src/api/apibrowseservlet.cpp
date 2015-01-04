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
#include "../db/database.h"

#include "commons.h"

#define QUERY_ALBUMS_COUNT "select count(*) from tbl_cds_albums"
#define QUERY_ITEM "select name, type, filename, mime_type, width, height, color from tbl_cds_files where parent = ? and type in (0, ?)"

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiBrowseServlet::logger(log4cxx::Logger::getLogger("squawk.api.ApiBrowseServlet"));

void ApiBrowseServlet::do_get( http::HttpRequest & request, ::http::HttpResponse & response ) {

    std::cout << "get albums:\n" << request << std::endl;

    std::string strType, strId;
    int type = 0;
    int id = 0;
    bool result = match(request.uri, &strType, &strId);
    id = (strId.length()==0 ? 0 : commons::string::parse_string<int>(strId));
    if( strType == "image" ){
        type = 2;
    } else if( strType == "video") {
        type = 3;
    }

    std::cout << "get " << strType << " = " << id << std::endl;

    squawk::db::Sqlite3Statement * stmt_items = NULL;

    try {
        stmt_items = db->prepare_statement( QUERY_ITEM );
        stmt_items->bind_int(1, type);
        stmt_items->bind_int(2, id);

        response << "{";

        while( stmt_items->step() ) {

            std::cout << "item:" << stmt_items->get_string(0) << std::endl;
        }
        response << "]";

        stmt_items->reset();
        db->release_statement(stmt_items);
        response << "}";

    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not get albums, Exception:" << e->code() << "-> " << e->what());
        if(stmt_items != NULL) db->release_statement(stmt_items);
        throw http::http_status::INTERNAL_SERVER_ERROR;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_albums.");
        throw http::http_status::INTERNAL_SERVER_ERROR;
    }
    response.set_mime_type( ::http::mime::JSON );
    response.set_status( ::http::http_status::OK );
}
}}
