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

void WebServer::register_servlet(HttpServlet * servlet) {
    servlets.push_back(servlet);
}

void WebServer::handle_request(HttpRequest & request, HttpResponse & response, std::function<void()> fptr ) {

    for(auto * servlet : servlets) {
        if(servlet->match(request.uri)) {
            try {

                if( request.request_method == "GET" ) {
                    servlet->do_get(request, response);
                } else if( request.request_method == "POST" ) {
                    servlet->do_post(request, response);
                } else if( request.request_method == "HEAD" ) {
                    servlet->do_head(request, response);
                 }
                /* TODO handle all http methods */


            } catch(http_status & status) {
                servlet->create_stock_reply(status, response);
            } catch(...) {
                servlet->create_stock_reply(http_status::INTERNAL_SERVER_ERROR, response);
            }
            std::cout << request.client_ip << " user-identifier anonymous [" << "] \"" << request.request_method << " " <<
                         request.uri << " HTTP/" << request.http_version_major << "." << request.http_version_minor << " " <<
                         int(response.status) << " " << response.get_size() << std::endl;
            fptr();
            return;
        }
    }
}

void WebServer::start() {
    HttpServerFactory::start( local_ip, port, threads, dynamic_cast< http::HttpRequestHandler * > ( this ) );
}
void WebServer::stop() { /* TODO */}

}
