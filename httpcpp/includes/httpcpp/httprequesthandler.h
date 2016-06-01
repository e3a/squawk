/*
    request handler interface.
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

#ifndef HTTPREQUESTHANDLER
#define HTTPREQUESTHANDLER

#include <string>

namespace http {

typedef std::unique_ptr< HttpServlet > ptr_servlet_t;
using http_callback_t = void( const HttpRequest&, HttpResponse& );

/**
 * @brief The HttpRequestHandler class
 */
class HttpRequestHandler {
public:
	/**
	 * @brief Register the servlets.
	 * @param servlet A servlet instance.
	 */
        virtual void register_servlet ( ptr_servlet_t servlet ) = 0;

        virtual void callback ( std::string & method, std::string & uri, std::function< http_callback_t > callback ) = 0;

	/**
	 * @brief Callback handler for requests.
	 * @param request The HttpRequest object.
	 * @param response The HttpResponse object.
	 */
	virtual void handle_request ( HttpRequest & request, HttpResponse & response, std::function<void() > fptr ) = 0;
};
} //http
#endif // HTTPREQUESTHANDLER
