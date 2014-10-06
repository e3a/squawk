/*
    SQLite3 Prepared Statement
    
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

#ifndef SQLITE3STATEMENT_H
#define SQLITE3STATEMENT_H

#include "squawk.h"
#include "sqlite3.h"

namespace squawk {
namespace db {
/**
 * \brief SQLlite3 prepared statement.
 */
class Sqlite3Statement {
  public:
    Sqlite3Statement(sqlite3 * db, sqlite3_stmt * stmt) : db(db), stmt(stmt) {};
    ~Sqlite3Statement() {
      sqlite3_finalize(stmt);
    };
    /**
     * \brief Bind an int for a prepared statement.
     * \param index the index of the value
     * \param value the int parameter
     * \throws DAOException throws a DAOException
     */
    virtual void bind_int(int index, int value);
    /**
     * \brief Bind text for a prepared statement.
     * \param index the index of the value
     * \param text the textt parameter
     * \throws DAOException throws a DAOException
     */
    virtual void bind_text(int index, std::string text);
    /**
     * \brief  Get an int form a result set
     * \param index the index of the value
     * \return the int value
     * \throws DAOException throws a DAOException
     */
    virtual int get_int(int index);
    /**
     * \brief Get a text from a result set.
     * \param index the index of the value
     * \param value the int parameter
     * \throws DAOException throws a DAOException
     */
    virtual std::string get_string(int index);
    /**
     * \brief Step to the next result in the result set.
     * \return true when there is a next row, false otherwise.
     * \throws DAOException throws a DAOException
     */
    virtual bool step();
    /**
     * \brief Update row.
     * \throws DAOException throws a DAOException
     */
    virtual int update();
    /**
     * \brief Insert row.
     * \throws DAOException throws a DAOException
     */
    virtual int insert();
    /**
     * \brief Reset the statement.
     */
    virtual void reset();
  private: 
    sqlite3 * db;
    sqlite3_stmt * stmt;
};
}}
#endif // SQLITE3STATEMENT_H
