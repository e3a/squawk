/*
    SQLite3 Database implementation.

    Copyright (C) 2015  <etienne> <e.knecht@netwings.ch>

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

#include "sqlite3statement.h"
#include "dbexception.h"

namespace squawk {
/**
 * \brief Database utilities.
 */
namespace db {

/**
 * \brief Database implementation for SQLite.
 */
class Sqlite3Database {
  public:
   int exec(const std::string & query);
   /**
    * \brief Open the database.
    * \param path The path of the database file.
    * \throws DAOException throws DAOExecption
    */
    void open(const std::string & path);
   /**
    * \brief Close the database.
    */
    int close();
   /**
    * \brief Prepare the sql statement.
    * \throws DAOException throws DAOExecption
    */
    Sqlite3Statement * prepare_statement(const std::string & statement);
   /**
    * \brief Release a statement.
    */
    void release_statement(Sqlite3Statement * stmt);
   /**
    * \brief Get last insert row id.
    */
    unsigned long last_insert_rowid();
    
  private:
    sqlite3 * db;
};
}}
#endif // SQLITE3DATABASE_H
