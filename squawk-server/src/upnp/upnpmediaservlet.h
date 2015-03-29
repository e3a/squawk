/*
    Media servlet header file.
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

#ifndef UPNPMEDIASERVLET_H
#define UPNPMEDIASERVLET_H

#include "http.h"
#include "fileservlet.h"

#include "../db/sqlite3database.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace upnp {

class UpnpMediaServlet : public http::HttpServlet {
public:
    explicit UpnpMediaServlet( const std::string path, squawk::db::Sqlite3Database * db ) :
        HttpServlet(path), db( db ) {}
    virtual void do_get( ::http::HttpRequest & request, ::http::HttpResponse & response );
    virtual void do_head( ::http::HttpRequest & request, ::http::HttpResponse & response );
private:
    static log4cxx::LoggerPtr logger;
    std::string path;
    void getFile( ::http::HttpRequest & request, ::http::HttpResponse & response );
    squawk::db::Sqlite3Database * db;
};
}}
#endif // UPNPMEDIASERVLET_H
