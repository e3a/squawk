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

#ifndef APIALBUMSLETTERSERVLET_H
#define APIALBUMSLETTERSERVLET_H

#include <string>

#include "squawk.h"
#include "http.h"
#include "../db/sqlite3connectionmanager.h"
#include "../db/sqlite3database.h"
#include "../db/sqlite3statement.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace api {

class ApiAlbumsLetterServlet : public ::http::HttpServlet {
public:
	ApiAlbumsLetterServlet ( const std::string & path, http::HttpServletContext context ) : HttpServlet ( path ),
		db ( squawk::db::Sqlite3ConnectionManager::instance().connection ( context.parameter ( squawk::CONFIG_DATABASE_FILE ) ) ) {}
	virtual void do_get ( ::http::HttpRequest & request, ::http::HttpResponse & response );
private:
	static log4cxx::LoggerPtr logger;
	squawk::db::db_connection_ptr db;
};
} // squawk
} // api
#endif // APIALBUMSLETTERSERVLET_H
