/*
    web server definition.
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

#ifndef WEBSERVER
#define WEBSERVER

#include <vector>

namespace http {

/**
 * @brief The main Web Server Class.
 */
class WebServer : public HttpRequestHandler {
public:

        WebServer ( std::string local_ip, int port /*, int threads */ );
        virtual ~WebServer();

	/**
	 * @brief Start the server.
	 */
        virtual void start();
	/**
	 * @brief Stop the server.
	 */
        virtual void stop();

        /* implement the HttpRequestHandler */

	/**
	 * @brief Register servlets.
	 * @param servlet The Servlet class.
	 */
	virtual void register_servlet ( HttpServlet * servlet );
	/**
	 * @brief The handle request callback method.
	 * @brief handle_request
	 * @param request
	 * @param response
	 */
	virtual void handle_request ( HttpRequest & request, HttpResponse & response, std::function<void() > fptr );
private:
	std::vector<HttpServlet *> servlets;
	std::string local_ip;
	int port; //, threads;
        std::unique_ptr< IHttpServer > httpServer_;
};
} //http
#endif // WEBSERVER

