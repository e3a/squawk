/*
    http client definition.
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
#ifndef HTTPCLIENT
#define HTTPCLIENT

namespace http {

/**
 * @brief The HttpClient class
 */
class HttpClient {
public:
	HttpClient ( const std::string & url );
	HttpClient ( const std::string & ip, const int & port, const std::string & uri );
	~HttpClient();

	void invoke ( HttpRequest & request, std::function<void ( HttpResponse& ) > callback );

private:
	std::string _ip;
	int _port;
	std::string _uri;
	std::unique_ptr< IHttpClientConnection > connection;
	HttpResponse response;

	FRIEND_TEST ( HttpClientTest, ParseIp );
	FRIEND_TEST ( HttpClientTest, ParsePort );
	FRIEND_TEST ( HttpClientTest, ParsePath );
	std::string parseIp ( const std::string & url );
	int parsePort ( const std::string & url );
	std::string parsePath ( const std::string & url );
};
} //http
#endif // HTTPCLIENT

