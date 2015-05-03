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
	method_ ( std::string ( method::GET ) ), uri_ ( path ), protocol_ ( "HTTP" ), remote_ip_ ( std::string ( "" ) ),
	body_size_ ( 0 ), http_version_major_ ( 1 ), http_version_minor_ ( 1 ),
	parameters_ ( std::map< std::string, std::string >() ),
	attributes_ ( std::map< std::string, std::string >() ) {

	out_body_ = std::shared_ptr< std::istream > ( new std::stringstream() );
}

HttpRequest::HttpRequest() :
	method_ ( std::string ( method::GET ) ), uri_ ( "" ), protocol_ ( "HTTP" ), remote_ip_ ( std::string ( "" ) ),
	body_size_ ( 0 ), http_version_major_ ( 1 ), http_version_minor_ ( 1 ),
	parameters_ ( std::map< std::string, std::string >() ),
	attributes_ ( std::map< std::string, std::string >() ) {

	out_body_ = std::shared_ptr< std::istream > ( new std::stringstream() );
}

void HttpRequest::method ( const std::string & method ) {
	method_ = method;
}
std::string HttpRequest::method() const {
	return method_;
}
void HttpRequest::protocol ( const std::string & protocol ) {
	protocol_ = protocol;
}
std::string HttpRequest::protocol() const {
	return protocol_;
}
std::string HttpRequest::uri() const {
	return uri_;
}
void HttpRequest::uri ( const std::string & uri ) {
	uri_ = uri;
}
void HttpRequest::httpVersionMajor ( const int & http_version_major ) {
	http_version_major_ = http_version_major;
}
int HttpRequest::httpVersionMajor() const {
	return http_version_major_;
}
void HttpRequest::httpVersionMinor ( const int & http_version_minor ) {
	http_version_minor_ = http_version_minor;
}
int HttpRequest::httpVersionMinor() const {
	return http_version_minor_;
}
std::string HttpRequest::remoteIp() const {
	return remote_ip_;
}
void HttpRequest::remoteIp ( const std::string & remote_ip ) {
	remote_ip_ = remote_ip;
}
void HttpRequest::requestBody ( const size_t & size, std::shared_ptr< std::istream > request_body ) {
	body_size_ = size;
	out_body_ = request_body;
}
const std::string HttpRequest::requestBody() const {
	return ( *std::dynamic_pointer_cast<std::stringstream> ( out_body_ ) ).str();
}
bool HttpRequest::isPersistent() { //TODO check standard
	return http_version_major_ == 1 && http_version_minor_ == 1 &&
		   parameters_.find ( header::CONNECTION ) != parameters_.end() &&
		   parameters_[ header::CONNECTION ] == "keep-alive";
}
void HttpRequest::setPersistend ( bool persistent ) {
	if ( persistent )
	{ parameters_[header::CONNECTION] = "keep-alive"; }

	else
	{ parameters_[header::CONNECTION] = "close"; }
}
void HttpRequest::parameter ( const std::string & name, const std::string & value ) {
	parameters_[name] = value;
}
std::string HttpRequest::parameter ( const std::string & name ) {
	return parameters_[name];
}
bool HttpRequest::containsParameter ( const std::string & name ) {
	return parameters_.find ( name ) != parameters_.end();
}
std::vector<std::string> HttpRequest::parameterNames() {
	std::vector< std::string > result;

	for ( auto r : parameters_ )
	{ result.push_back ( r.first ); }

	return result;
}
std::map< std::string, std::string > HttpRequest::parameterMap() {
	return parameters_;
}
void HttpRequest::attribute ( const std::string & name, const std::string & value ) {
	attributes_[name] = value;
}
std::string HttpRequest::attribute ( const std::string & name ) {
	return attributes_[name];
}
bool HttpRequest::containsAttribute ( const std::string & name ) {
	return attributes_.find ( name ) != attributes_.end();
}
std::vector<std::string> HttpRequest::attributeNames() {
	std::vector< std::string > result;

	for ( auto r : attributes_ )
	{ result.push_back ( r.first ); }

	return result;
}
std::map< std::string, std::string > HttpRequest::attributeMap() {
	return attributes_;
}
void HttpRequest::content ( std::array< char, BUFFER_SIZE > & body, const size_t & index, const size_t & count ) {
	for ( size_t i = index; i < ( index + count ); i++ ) {
		( *std::dynamic_pointer_cast<std::stringstream> ( out_body_ ) ) << body[i];
	}
}
void HttpRequest::operator<< ( const std::string & in ) {
	( *std::dynamic_pointer_cast<std::stringstream> ( out_body_ ) ) << in;
	body_size_ += in.size();
}
std::ostream& operator<< ( std::ostream& out, const http::HttpRequest & request ) {
	out << request.method_ << " " << request.uri_ << " " << request.http_version_major_ << "." << request.http_version_minor_ << "\n";
	out << "RequestLines:\n";

	for ( auto request_line : request.parameters_ ) {
		out << "\t" << request_line.first << ": " << request_line.second << "\n";
	}

	if ( request.attributes_.size() > 0 ) {
		out << "Parameters:\n";

		for ( auto parameter : request.attributes_ ) {
			out << "\t" << parameter.first << ": " << parameter.second << "\n";
		}
	}

	return out;
}

} //http
