/*
    API Albums Servlet
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

#ifndef APIALBUMSHANDLER_H
#define APIALBUMSHANDLER_H

#include <string>

#include "http.h"
#include "../db/database.h"
/* #include "../db/sqlite3database.h"
#include "../db/sqlite3statement.h" */

#include "log4cxx/logger.h"

namespace squawk {
namespace api {

class ApiAlbumsServlet : public ::http::HttpServlet {
public:
  ApiAlbumsServlet( const std::string path, squawk::db::Sqlite3Database * db ) : HttpServlet(path), db(db) {}
  virtual void do_get(::http::HttpRequest & request, ::http::HttpResponse & response);
private:
  static log4cxx::LoggerPtr logger;
  squawk::db::Sqlite3Database * db;
};
}}
#endif // APIALBUMSHANDLER_H
