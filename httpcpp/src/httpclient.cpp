/*
    asio http client implementation
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

#include "asio/httpclientconnection.h"

namespace http {

HttpClient::HttpClient ( const std::string & ip, const int & port, const std::string & uri ) :
	_ip ( ip ), _port ( port ), _uri ( uri ), connection ( std::unique_ptr<IHttpClientConnection> ( new HttpClientConnection ( ip ) ) ) {}

HttpClient::~HttpClient() {}

void HttpClient::invoke ( HttpRequest & request, std::function<void ( HttpResponse& ) > callback ) {
	std::cout << "HTTPclient  invoke" << std::endl;

    //TODO add all neccesary headers
	if ( request.isPersistent() ) {
		request.parameters_[header::CONNECTION] = "keep-alive";

	} else {
		request.parameters_[header::CONNECTION] = "close";
	}

	connection->start ( &request, &response, callback );
}
std::string HttpClient::parseIp ( const std::string & url ) {
	if ( url.find ( "http://" ) == 0 ) {
		size_t dot_pos = url.find_first_of ( ":", 7 );
		size_t slash_pos = url.find_first_of ( "/", 7 );

		if ( dot_pos == std::string::npos || dot_pos > slash_pos ) {
			return url.substr ( 7, slash_pos - 7 );

		} else {
			return url.substr ( 7, dot_pos - 7 );
		}

	} else {
		std::cout << "url does not start with http" << std::endl;
		return "";
	}
}
int HttpClient::parsePort ( const std::string & url ) {
	size_t dot_pos = url.find_first_of ( ":", 7 );

	if ( dot_pos != std::string::npos ) {
		size_t slash_pos = url.find_first_of ( "/", 7 );

		if ( slash_pos != std::string::npos ) {
            return std::stoi ( url.substr ( dot_pos + 1, slash_pos - dot_pos ) );

		} else {
            return std::stoi ( url.substr ( dot_pos + 1, url.size() - dot_pos ) );
		}

	} else { return 0; }
}
std::string HttpClient::parsePath ( const std::string & url ) {
	size_t slash_pos = url.find_first_of ( "/", 7 );

	if ( slash_pos != std::string::npos ) {
		return url.substr ( slash_pos, url.size() - slash_pos );

	} else { return "/"; }
}
} //namespace http
