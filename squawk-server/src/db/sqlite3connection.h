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

#ifndef SQLITE3CONNECTION_H
#define SQLITE3CONNECTION_H

#include <iostream> //TODO

#include <map>
#include <mutex>

#include "sqlite3statement.h"
#include "dbexception.h"

#include <sqlite3.h>

#include "log4cxx/logger.h"

namespace db {

/** \brief Connection implementation for SQLite. */
class Sqlite3Connection {
public:

    Sqlite3Connection ( const std::string & path );
    Sqlite3Connection ( const Sqlite3Connection& other ) = delete;
    Sqlite3Connection& operator= ( const Sqlite3Connection& ) = delete;
    ~Sqlite3Connection();

    /** \brief execute a sql command on the database */
	int exec ( const std::string & query );

	/**
	 * \brief Prepare the sql statement.
	 * \throws DAOException throws DAOExecption
	 */
	db_statement_ptr prepareStatement ( const std::string & statement );
	/**
	 * \brief Get last insert row id.
	 */
	unsigned long last_insert_rowid();

    /**
     * \brief Count the number of rows modified.
     * <p>Get returns the number of rows modified, inserted or deleted by the most
     * recently completed INSERT, UPDATE or DELETE statement on the database connection.</p>
     */
    int last_changes_count();

private:
    static log4cxx::LoggerPtr logger;

    sqlite3_ptr _db;
    std::mutex _mtx;
    std::map< std::string, std::vector< Sqlite3Statement * > > _stmt_pool;

    /** \brief Release a statement. */
    void _release_statement ( Sqlite3Statement * statement );
};
typedef std::shared_ptr< Sqlite3Connection > ptr_connection;
} //namespace db
#endif // SQLITE3CONNECTION_H
