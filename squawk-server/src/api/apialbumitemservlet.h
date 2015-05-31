/*
    API Album Item Servlet
    Copyright (C) 2013  <copyright holder> <e.knecht@netwings.ch>

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

#ifndef APIALBUMITEMSERVLET_H
#define APIALBUMITEMSERVLET_H

#include <string>

#include "squawk.h"
#include "http.h"

#include "../db/sqlite3database.h"
#include "../db/sqlite3connection.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace api {
/**
 * @brief The ApiAlbumItemServlet class
 */
class ApiAlbumItemServlet : public http::HttpServlet {
public:
	ApiAlbumItemServlet ( const std::string & path, http::HttpServletContext context ) : HttpServlet ( path ),
		db ( squawk::db::Sqlite3Database::instance().connection ( context.parameter ( squawk::CONFIG_DATABASE_FILE ) ) ) {}
	~ApiAlbumItemServlet() {}
	virtual void do_get ( http::HttpRequest & request, http::HttpResponse & response ) override;
private:
	static log4cxx::LoggerPtr logger;
	squawk::db::db_connection_ptr db;
};
} // api
} // squawk
#endif // APIALBUMITEMSERVLET_H
