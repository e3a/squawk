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

#include "dumpdata.h"

namespace squawk {
namespace servlet {
namespace api {

log4cxx::LoggerPtr ApiAlbumsServlet::logger(log4cxx::Logger::getLogger("squawk.servlet.api.DumpData"));

void ApiAlbumsServlet::do_get( http::HttpRequest & request, ::http::HttpResponse & response ) {
    std::cout << "get albums:\n" << request << std::endl;

    squawk::db::Sqlite3Statement * stmt_albums = NULL;

    try {

        stmt_count = db->prepare_statement( "select xxx from " );


    } catch( squawk::db::DaoException & e ) {
        LOG4CXX_FATAL(logger, "Can not dump all data, Exception:" << e.code() << "-> " << e.what());
        if(stmt_albums != NULL) db->release_statement(stmt_albums);
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in dump data.");
        throw;
    }
    response.set_mime_type( ::http::mime::JSON );
    response.set_status( ::http::http_status::OK );
}

}}}
