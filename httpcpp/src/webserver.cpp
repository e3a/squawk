/*
    web server implementation.
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

#include "httpserverfactory.h"

namespace http {

void WebServer::register_servlet ( HttpServlet * servlet ) {
	servlets.push_back ( servlet );
}

void WebServer::handle_request ( HttpRequest & request, HttpResponse & response, std::function< void() > fptr ) {

	for ( auto * servlet : servlets ) {
		if ( servlet->match ( request.uri() ) ) {
			try {

				if ( request.method() == method::GET ) {
					servlet->do_get ( request, response );

				} else if ( request.method() == method::POST ) {
					servlet->do_post ( request, response );

				} else if ( request.method() == method::HEAD ) {
					servlet->do_head ( request, response );

				} else if ( request.method() == method::PUT ) {
					servlet->do_put ( request, response );

				} else if ( request.method() == method::DELETE ) {
					servlet->do_delete ( request, response );

				} else if ( request.method() == method::TRACE ) {
					servlet->do_trace ( request, response );

				} else if ( request.method() == method::OPTIONS ) {
					servlet->do_options ( request, response );

				} else if ( request.method() == method::CONNECT ) {
					servlet->do_connect ( request, response );

				} else {
					servlet->do_default ( request.method(), request, response );
				}

			} catch ( http_status & status ) {
				servlet->create_stock_reply ( status, response );

			} catch ( ... ) {
				servlet->create_stock_reply ( http_status::INTERNAL_SERVER_ERROR, response );
			}

			//log request
			std::cout << request.remoteIp() << " user-identifier anonymous [" << "] \"" << request.method() << " " <<
					  request.uri() << " HTTP/" << request.httpVersionMajor() << "." << request.httpVersionMinor() << " " <<
					  http::parse_status ( response.status() ) << " " << response.size() << std::endl;
			fptr();
			return;
		}
	}
}

void WebServer::start() {
	HttpServerFactory::start ( local_ip, port/* , threads */, dynamic_cast< http::HttpRequestHandler * > ( this ) );
}
void WebServer::stop() { /* TODO */}

} //http
