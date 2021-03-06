/*
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

#include "sqlite3connection.h"
#include "dbexception.h"

namespace db {

Sqlite3Connection::Sqlite3Connection ( const std::string & path ) {
	sqlite3 * db;
	int res = sqlite3_open ( path.c_str(), &db );

	if ( res != SQLITE_OK ) {
		throw DbException ( res, sqlite3_errmsg ( db ) );
	}

    _db = std::unique_ptr< sqlite3, std::function<void ( sqlite3* ) > > ( db, [] ( sqlite3 * db ) {
		sqlite3_close ( db );
	} );
}
Sqlite3Connection::~Sqlite3Connection() {
    for ( auto stmt_list : _stmt_pool ) {
		for ( auto stmt : stmt_list.second ) {
			delete stmt;
		}
	}
    _stmt_pool.clear();
}
int Sqlite3Connection::exec ( const std::string & query ) {
    return sqlite3_exec ( _db.get(), query.c_str(), NULL, NULL, NULL );
}
db_statement_ptr Sqlite3Connection::prepareStatement ( const std::string & statement ) {
    std::lock_guard<std::mutex> lck ( _db_mtx );
	Sqlite3Statement * stmt = nullptr;

	// search an existing statement
    if ( _stmt_pool.find ( statement ) != _stmt_pool.end() &&
            _stmt_pool[statement].size() != 0 ) {

        stmt = _stmt_pool[statement].back();
        _stmt_pool[statement].pop_back();

    // create new statement
	}  else {
		sqlite3_stmt * sqlite3_statement;
        int res = sqlite3_prepare ( _db.get(), statement.c_str(), -1, &sqlite3_statement, 0 );

		if ( SQLITE_OK != res ) {
            throw DbException ( res, sqlite3_errmsg ( _db.get() ) );
		}

		sqlite3_stmt_ptr sqlite3_statement_ ( sqlite3_statement, [] ( sqlite3_stmt * sqlite3_statement ) {
            sqlite3_finalize ( sqlite3_statement );
		} );
        stmt = new Sqlite3Statement ( _db, std::move ( sqlite3_statement_ ), statement );
	}

    return std::shared_ptr< Sqlite3Statement > ( stmt, std::bind ( &Sqlite3Connection::_release_statement, this, std::placeholders::_1 ) );
}
void Sqlite3Connection::_release_statement ( Sqlite3Statement * statement ) {
    std::lock_guard<std::mutex> lck ( _db_mtx );
	statement->reset();
    _stmt_pool[statement->statement()].push_back ( statement );
}
unsigned long Sqlite3Connection::last_insert_rowid() {
    return sqlite3_last_insert_rowid ( _db.get() );
}
int Sqlite3Connection::last_changes_count() {
    return sqlite3_changes ( _db.get() );
}
} //namespace db
