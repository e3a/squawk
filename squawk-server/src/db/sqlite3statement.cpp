/*
    SQLite3 Prepared Statement
    
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

#include "sqlite3statement.h"

namespace squawk {
namespace db {
  
void Sqlite3Statement::bind_int(const int & index, const int & value) {
    int res = sqlite3_bind_int(stmt, index, value);
    if (res != SQLITE_OK) {
      throw DbException(res, std::string(sqlite3_errmsg(db)));
    }
}
void Sqlite3Statement::bind_text(const int & index, const std::string & text) {
    int res = sqlite3_bind_text(stmt, index, text.c_str(), text.length(), SQLITE_STATIC);
    if (res != SQLITE_OK) {
      throw DbException(res, std::string(sqlite3_errmsg(db)));
    }
}
int Sqlite3Statement::get_int(const int & position) {
  sqlite3_value* sql3_msgid = sqlite3_column_value(stmt,position);
  return sqlite3_value_int(sql3_msgid);
}
std::string Sqlite3Statement::get_string(const int & position) {
  return std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,position)));
}
bool Sqlite3Statement::step() {
  int step = sqlite3_step( stmt );
  if( step == SQLITE_DONE ) {
      return false;
  } else if( step == SQLITE_ROW) {
    return true;
  }
  throw DbException(step, std::string(sqlite3_errmsg(db)));
}
int Sqlite3Statement::update() {
  int step = sqlite3_step(stmt);
  if(step == SQLITE_DONE) {
    return step;
  }
  throw DbException(step, std::string(sqlite3_errmsg(db)));
}
int Sqlite3Statement::insert() {
  int step = sqlite3_step(stmt);
  if(step == SQLITE_DONE) {
    return step;
  }
  throw DbException(step, std::string(sqlite3_errmsg(db)));
}
void Sqlite3Statement::reset() {
  sqlite3_reset(stmt);
}
}}
