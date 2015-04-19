/*
    asio web server implementation
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

#include "http.h"

#include <iostream>
#include <thread>

#include "httpserverfactory.h"

namespace http {

void WebServer::register_servlet ( HttpServlet * servlet ) {
	servlets.push_back ( servlet );
}

void WebServer::handle_request ( HttpRequest & request, HttpResponse & response, std::function< void() > fptr ) {

	for ( auto * servlet : servlets ) {
		if ( servlet->match ( request.uri() ) ) {
			try {
				std::cout << "execute servlet: " << ( servlet->getPath() ) << std::endl;

				if ( request.method() == "GET" ) {
					servlet->do_get ( request, response );

				} else if ( request.method() == "POST" ) {
					servlet->do_post ( request, response );

				} else if ( request.method() == "HEAD" ) {
					servlet->do_head ( request, response );

				} else if ( request.method() == "SUBSCRIBE" ) {
					servlet->do_subscribe ( request, response );

				} else {
					std::cerr << "unknow request method: " << request.method() << std::endl;
					throw http_status::NOT_IMPLEMENTED;
				}

				/* TODO handle all http methods */


			} catch ( http_status & status ) {
				std::cerr << "create error code with status: " /* << status */ << std::endl;
				servlet->create_stock_reply ( status, response );

			} catch ( ... ) {
				std::cerr << "create error code without status." << std::endl;
				servlet->create_stock_reply ( http_status::INTERNAL_SERVER_ERROR, response );
			}

			//log request
			std::cout << request.remoteIp() << " user-identifier anonymous [" << "] \"" << request.method() << " " <<
					  request.uri() << " HTTP/" << request.httpVersionMajor() << "." << request.httpVersionMinor() << " " <<
					  int ( response.status ) << " " << response.get_size() << std::endl;
			fptr();
			return;
		}
	}
}

void WebServer::start() {
	HttpServerFactory::start ( local_ip, port/* , threads */, dynamic_cast< http::HttpRequestHandler * > ( this ) );
}
void WebServer::stop() { /* TODO */}

}
