/*
    SQLite3 Database implementation.

    Copyright (C) <year>  <name of author>

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

#include "sqlite3database.h"
#include "sqlite3statement.h"
#include "database.h"

namespace squawk {
namespace db {

int Sqlite3Database::exec( std::string query ) {
    return sqlite3_exec(db, query.c_str(), NULL, NULL, NULL);
}

void Sqlite3Database::open( std::string path ) {
    int res = sqlite3_open(path.c_str(), &db);
    if(res != SQLITE_OK) {
        throw new ::db::DbException(res, sqlite3_errmsg(db));
    }
}
int Sqlite3Database::close() {
    sqlite3_close(db);
}
Sqlite3Statement * Sqlite3Database::prepare_statement(std::string statement) {
    sqlite3_stmt * sqlite3_statement;
    int res = sqlite3_prepare(db, statement.c_str(), -1, &sqlite3_statement, 0);
    if(SQLITE_OK != res) {
        throw new ::db::DbException(res, sqlite3_errmsg(db));
    }
    Sqlite3Statement * stmt = new Sqlite3Statement(db, sqlite3_statement);
    return stmt;
}
void Sqlite3Database::release_statement(Sqlite3Statement * statement) {
    statement->reset();
    delete statement;
}
unsigned long Sqlite3Database::last_insert_rowid() {
    return sqlite3_last_insert_rowid(db);
}
}}
