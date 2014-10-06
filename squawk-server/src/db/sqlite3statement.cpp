/*
    <one line to give the library's name and an idea of what it does.>
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

#include "sqlite3statement.h"
#include "squawk.h"

#include <string>

#include "sqlite3.h"

namespace squawk {
namespace db {
  
void Sqlite3Statement::bind_int(int index, int value) {
    int res = sqlite3_bind_int(stmt, index, value);
    if (res != SQLITE_OK) {
      throw new DaoException(res, std::string(sqlite3_errmsg(db)));
    }
}
void Sqlite3Statement::bind_text(int index, std::string text) {
    int res = sqlite3_bind_text(stmt, index, text.c_str(), text.length(), SQLITE_STATIC);
    if (res != SQLITE_OK) {
      throw new DaoException(res, std::string(sqlite3_errmsg(db)));
    }
}
int Sqlite3Statement::get_int(int position) {
  sqlite3_value* sql3_msgid = sqlite3_column_value(stmt,position);
  return sqlite3_value_int(sql3_msgid);
}
std::string Sqlite3Statement::get_string(int position) {
  return std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,position)));
}
bool Sqlite3Statement::step() {
  int step = sqlite3_step( stmt );
  if( step == SQLITE_DONE ) {
      return false;
  } else if( step == SQLITE_ROW) {
    return true;
  }
  throw new DaoException(step, std::string(sqlite3_errmsg(db)));
}
int Sqlite3Statement::update() {
  int step = sqlite3_step(stmt);
  if(step == SQLITE_DONE) {
    return step;
  }
  throw new DaoException(step, std::string(sqlite3_errmsg(db)));
}
int Sqlite3Statement::insert() {
  int step = sqlite3_step(stmt);
  if(step == SQLITE_DONE) {
    return step;
  }
  throw new DaoException(step, std::string(sqlite3_errmsg(db)));
}
void Sqlite3Statement::reset() {
  sqlite3_reset(stmt);
}
}}
