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

#include "sqlite3database.h"

namespace db {

db_connection_ptr Sqlite3Database::connection ( const std::string & path ) {
	std::lock_guard<std::mutex> lck ( mtx_ );

	if ( connections_.find ( path ) == connections_.end() ) {
        connections_[ path ] = std::shared_ptr< Sqlite3Connection > ( new Sqlite3Connection ( path ) );
	}

	return connections_[ path ];
}
} //namespace db
