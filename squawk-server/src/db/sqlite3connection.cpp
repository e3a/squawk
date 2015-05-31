/*
    DB connection implementation.
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

#include <sqlite3.h>

#include "squawk.h"

#include "sqlite3connection.h"
#include "sqlite3statement.h"
#include "dbexception.h"

namespace squawk {
namespace db {

log4cxx::LoggerPtr Sqlite3Connection::logger ( log4cxx::Logger::getLogger ( "squawk.db.Sqlite3Connection" ) );

Sqlite3Connection::Sqlite3Connection ( const std::string & path ) {
	sqlite3 * db;
	int res = sqlite3_open ( path.c_str(), &db );

	if ( res != SQLITE_OK ) {
		throw DbException ( res, sqlite3_errmsg ( db ) );
	}

	db_ = std::unique_ptr< sqlite3, std::function<void ( sqlite3* ) > > ( db, [] ( sqlite3 * db ) {
		std::cout << "delete database" << std::endl;
		sqlite3_close ( db );
	} );
}
Sqlite3Connection::~Sqlite3Connection() {
	std::cout << "DTOR START" << std::endl;

	for ( auto stmt_list : stmt_pool ) {
		for ( auto stmt : stmt_list.second ) {
			delete stmt;
		}
	}

	stmt_pool.clear();
	std::cout << "DTOR END" << std::endl;
}
int Sqlite3Connection::exec ( const std::string & query ) {
	return sqlite3_exec ( db_.get(), query.c_str(), NULL, NULL, NULL );
}
db_statement_ptr Sqlite3Connection::prepareStatement ( const std::string & statement ) {
	std::lock_guard<std::mutex> lck ( mtx_ );
	Sqlite3Statement * stmt = nullptr;

	// search an existing statement
	if ( stmt_pool.find ( statement ) != stmt_pool.end() &&
			stmt_pool[statement].size() != 0 ) {

		stmt = stmt_pool[statement].back();
		stmt_pool[statement].pop_back();

    // create new statement
	}  else {
		sqlite3_stmt * sqlite3_statement;
		int res = sqlite3_prepare ( db_.get(), statement.c_str(), -1, &sqlite3_statement, 0 );

		if ( SQLITE_OK != res ) {
			throw DbException ( res, sqlite3_errmsg ( db_.get() ) );
		}

		sqlite3_stmt_ptr sqlite3_statement_ ( sqlite3_statement, [] ( sqlite3_stmt * sqlite3_statement ) {
            std::cout << "delete sqlite3_stmt" << std::endl;
            sqlite3_finalize ( sqlite3_statement );
		} );
		stmt = new Sqlite3Statement ( db_, std::move ( sqlite3_statement_ ), statement );
	}

    return std::shared_ptr< Sqlite3Statement > ( stmt, std::bind ( &Sqlite3Connection::release_statement, this, std::placeholders::_1 ) );
}
void Sqlite3Connection::release_statement ( Sqlite3Statement * statement ) {
	std::lock_guard<std::mutex> lck ( mtx_ );
	statement->reset();
	stmt_pool[statement->statement()].push_back ( statement );
    if( squawk::DEBUG ) LOG4CXX_TRACE ( logger, "size of statement pool: " << stmt_pool.size() )
}
unsigned long Sqlite3Connection::last_insert_rowid() {
	return sqlite3_last_insert_rowid ( db_.get() );
}
} // db
} // squawk
