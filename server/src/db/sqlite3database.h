/*
    Copyright (C) 2014  <e.knecht@netwings.ch>

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

#ifndef SQLITE3DATABASE_H
#define SQLITE3DATABASE_H

#include "sqlite3connection.h"

namespace db {

typedef std::shared_ptr< Sqlite3Connection > db_connection_ptr;

/** @brief The Sqlite3 database manager class. */
class Sqlite3Database {
public:
    Sqlite3Database ( const Sqlite3Database& ) = delete;
    Sqlite3Database & operator= ( const Sqlite3Database& ) = delete;

    /** \brief Factory method */
    static Sqlite3Database & instance() {
        static Sqlite3Database instance;
		return instance;
	}

    /** \brief Create a connection to the database */
	db_connection_ptr connection ( const std::string & path );

private:
    Sqlite3Database() = default;

	std::map< std::string, db_connection_ptr > connections_;
	std::mutex mtx_;
};
} //db
#endif // SQLITE3DATABASE_H
