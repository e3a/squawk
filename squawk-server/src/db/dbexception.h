/*
    DB Exception implementation.
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

#ifndef DATABASE_H
#define DATABASE_H

#include <exception>

namespace squawk {
namespace db {
/**
 * @brief The DatabaseException class
 */
class DbException : public std::exception {
public:
    explicit DbException ( int _code, std::string _what ) throw() : _code ( _code ), _what ( _what ) {}
    virtual ~DbException() throw() {}
    virtual const char* what() const throw() {
        return _what.c_str();
    }
    int code() throw() {
        return _code;
    }
private:
    int _code;
    std::string _what;
};
} // db
} // squawk
#endif // DATABASE_H
