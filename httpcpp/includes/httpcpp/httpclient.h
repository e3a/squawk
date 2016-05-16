/*
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
#ifndef HTTPCLIENT
#define HTTPCLIENT

#include "http.h"

namespace http {

/**
 * @brief The HttpClient class
 */
class HttpClient {
public:
	HttpClient ( const std::string & url );
	HttpClient ( const std::string & ip, const int & port, const std::string & uri );
        ~HttpClient();

        /**
         * @brief invoke
         * @param request
         * @param callback
         */
	void invoke ( HttpRequest & request, std::function<void ( HttpResponse& ) > callback );

        /**
         * @brief parseIp
         * @param url
         * @return
         */
        static std::string parseIp ( const std::string & url );
        /**
         * @brief parsePort
         * @param url
         * @return
         */
        static int parsePort ( const std::string & url );
        /**
         * @brief parsePath
         * @param url
         * @return
         */
        static std::string parsePath ( const std::string & url );

private:
	std::string _ip;
	int _port;
	std::string _uri;
	std::unique_ptr< IHttpClientConnection > connection;
	HttpResponse response;
};
} //http
#endif // HTTPCLIENT

