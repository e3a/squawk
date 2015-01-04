/*
    media servlet header file.
    Copyright (C) 2013  e.knecht@netwings.ch

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

#ifndef MEDIASERVLET_H
#define MEDIASERVLET_H

#include "http.h"
#include "fileservlet.h"

#include "../db/sqlite3database.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace api {

class MediaServlet : public ::http::servlet::FileServlet {
public:
    explicit MediaServlet( const std::string path, const std::string docroot, squawk::db::Sqlite3Database * db ) :
        FileServlet( path, docroot ), db( db ), mediadirectory( docroot ) {}
    virtual void do_get( ::http::HttpRequest & request, ::http::HttpResponse & response );
private:
    static log4cxx::LoggerPtr logger;
    squawk::db::Sqlite3Database * db;
    const std::string mediadirectory;
};
}}
#endif // MEDIASERVLET_H
