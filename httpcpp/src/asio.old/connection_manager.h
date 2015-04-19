/*
    asio connection manager implementation header
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

#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>
#include "asioconnection.h"

namespace http {
namespace asio_impl {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class connection_manager
{
public:
	connection_manager ( const connection_manager& ) = delete;
	connection_manager& operator= ( const connection_manager& ) = delete;

	/// Construct a connection manager.
	connection_manager();

	/// Add the specified connection to the manager and start it.
	void start ( connection_ptr c );

	/// Stop the specified connection.
	void stop ( connection_ptr c );

	/// Stop all connections.
	void stop_all();

private:
	/// The managed connections.
	std::set<connection_ptr> connections_;
};

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_MANAGER_HPP
