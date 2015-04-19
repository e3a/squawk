/*
    http request implementation
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

namespace http  {
HttpRequest::HttpRequest ( const std::string & path ) :
	_method ( std::string ( "GET" ) ), _uri ( path ), _protocol ( "HTTP" ), body_size ( 0 ),
	_remote_ip ( std::string ( "" ) ), _http_version_major ( 1 ), _http_version_minor ( 1 ),
	request_lines ( std::map< std::string, std::string >() ),
	parameters ( std::map< std::string, std::string >() ) {

	out_body = std::shared_ptr< std::istream > ( new std::stringstream() );
}

HttpRequest::HttpRequest() :
	_method ( std::string ( "GET" ) ), _uri ( "" ), _protocol ( "HTTP" ), body_size ( 0 ),
	_remote_ip ( std::string ( "" ) ), _http_version_major ( 1 ), _http_version_minor ( 1 ),
	request_lines ( std::map< std::string, std::string >() ),
	parameters ( std::map< std::string, std::string >() ) {

	out_body = std::shared_ptr< std::istream > ( new std::stringstream() );
}

void HttpRequest::method ( const std::string & method ) {
	_method = method;
}
std::string HttpRequest::method() const {
	return _method;
}
void HttpRequest::protocol ( const std::string & protocol ) {
	_protocol = protocol;
}
std::string HttpRequest::protocol() const {
	return _protocol;
}
std::string HttpRequest::uri() const {
	return _uri;
}
void HttpRequest::uri ( const std::string & uri ) {
	_uri = uri;
}
void HttpRequest::httpVersionMajor ( const int & http_version_major ) {
	_http_version_major = http_version_major;
}
int HttpRequest::httpVersionMajor() const {
	return _http_version_major;
}
void HttpRequest::httpVersionMinor ( const int & http_version_minor ) {
	_http_version_minor = http_version_minor;
}
int HttpRequest::httpVersionMinor() const {
	return _http_version_minor;
}
std::string HttpRequest::remoteIp() const {
	return _remote_ip;
}
void HttpRequest::remoteIp ( const std::string & remote_ip ) {
	_remote_ip = remote_ip;
}
void HttpRequest::requestBody ( const size_t & size, std::shared_ptr< std::istream > request_body ) {
	body_size = size;
	out_body = request_body;
}
const std::string HttpRequest::requestBody() const {
	return ( *std::dynamic_pointer_cast<std::stringstream> ( out_body ) ).str();
}
bool HttpRequest::isPersistent() { //TODO check standard
	return _http_version_major == 1 && _http_version_minor == 1 &&
		   request_lines.find ( HTTP_HEADER_CONNECTION ) != request_lines.end() &&
		   request_lines[ HTTP_HEADER_CONNECTION ] == "keep-alive";
}
void HttpRequest::setPersistend ( bool persistent ) {
	if ( persistent )
	{ request_lines[HTTP_HEADER_CONNECTION] = "keep-alive"; }

	else
	{ request_lines[HTTP_HEADER_CONNECTION] = "close"; }
}
void HttpRequest::operator<< ( const std::string & in ) {
	( *std::dynamic_pointer_cast<std::stringstream> ( out_body ) ) << in;
	body_size += in.size();
}
std::ostream& operator<< ( std::ostream& out, const http::HttpRequest & request ) {
	out << request._method << " " << request._uri << " " << request._http_version_major << "." << request._http_version_minor << "\n";
	out << "RequestLines:\n";

	for ( auto request_line : request.request_lines ) {
		out << "\t" << request_line.first << ": " << request_line.second << "\n";
	}

	if ( request.parameters.size() > 0 ) {
		out << "Parameters:\n";

		for ( auto parameter : request.parameters ) {
			out << "\t" << parameter.first << ": " << parameter.second << "\n";
		}
	}

	return out;
}

} //http
