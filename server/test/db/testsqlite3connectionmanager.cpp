/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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

#include "../../src/db/sqlite3database.h"

#include <gtest/gtest.h>

TEST(TestSqlite3ConnectionManager, Singleton) {
    ASSERT_EQ( &db::Sqlite3Database::instance(), &db::Sqlite3Database::instance() );
}
TEST(TestSqlite3ConnectionManager, connection ) {
    { db::db_connection_ptr database1 = db::Sqlite3Database::instance().connection( "aaa" );
     { db::db_connection_ptr database2 = db::Sqlite3Database::instance().connection( "aaa" );
       ASSERT_EQ( 3, database1.use_count() );
     }
     ASSERT_EQ( 2, database1.use_count() );
    }
}

