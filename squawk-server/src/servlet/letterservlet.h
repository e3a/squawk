/*
    get the available letters
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

#ifndef LETTERSERVLET_H
#define LETTERSERVLET_H

#include <string>

#include "http.h"
#include "../db/sqlite3database.h"
#include "../db/sqlite3statement.h"

#include "log4cxx/logger.h"

class LetterServlet : public ::http::HttpServlet {
    LetterServlet( const std::string path, squawk::db::Sqlite3Database * db ) : HttpServlet(path), db(db) {};
    virtual void do_get(::http::HttpRequest & request, ::http::HttpResponse & response);
private:
    static log4cxx::LoggerPtr logger;
    squawk::db::Sqlite3Database * db;
};

#endif // LETTERSERVLET_H
