/*
    http response implementation
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

#define RESPONSE_LINE_OK                    std::string("HTTP/1.1 200 OK\r\n")
#define RESPONSE_LINE_PARTIAL_CONTENT       std::string("HTTP/1.1 206 OK\r\n")
#define RESPONSE_LINE_CREATED               std::string("HTTP/1.1 201 Created\r\n")
#define RESPONSE_LINE_ACCEPTED              std::string("HTTP/1.1 202 Accepted\r\n")
#define RESPONSE_LINE_NO_CONTENT            std::string("HTTP/1.1 204 No Content\r\n")
#define RESPONSE_LINE_MULTIPLE_CHOICES      std::string("HTTP/1.1 300 Multiple Choices\r\n")
#define RESPONSE_LINE_MOVED_PERMANENTLY     std::string("HTTP/1.1 301 Moved Permanently\r\n")
#define RESPONSE_LINE_MOVED_TEMPORARILY     std::string("HTTP/1.1 302 Moved Temporarily\r\n")
#define RESPONSE_LINE_NOT_MODIFIED          std::string("HTTP/1.1 304 Not Modified\r\n")
#define RESPONSE_LINE_BAD_REQUEST           std::string("HTTP/1.1 400 Bad Request\r\n")
#define RESPONSE_LINE_UNAUTHORIZED          std::string("HTTP/1.1 401 Unauthorized\r\n")
#define RESPONSE_LINE_FORBIDDEN             std::string("HTTP/1.1 403 Forbidden\r\n")
#define RESPONSE_LINE_NOT_FOUND             std::string("HTTP/1.1 404 Not Found\r\n")
#define RESPONSE_LINE_INTERNAL_SERVER_ERROR std::string("HTTP/1.1 500 Internal Server Error\r\n")
#define RESPONSE_LINE_NOT_IMPLEMENTED       std::string("HTTP/1.1 501 Not Implemented\r\n")
#define RESPONSE_LINE_BAD_GATEWAY           std::string("HTTP/1.1 502 Bad Gateway\r\n")
#define RESPONSE_LINE_SERVICE_UNAVAILABLE   std::string("HTTP/1.1 503 Service Unavailable\r\n")

#define LINE_BREAK std::string("\r\n")

namespace http {

HttpResponse::HttpResponse() : protocol_ ( "" ) {}
HttpResponse::~HttpResponse() {
    if ( body_istream ) {
        delete body_istream; //TODO unique pointer
    }
};

HttpResponse & HttpResponse::operator<< ( const std::string & str ) {
	body_stream << str;
	size_ += str.length();
	return *this;
}

void HttpResponse::set_istream ( std::istream * is ) {
    std::cout << "set stream im response" << std::endl;

	if ( body_istream ) {
		std::cerr << "body input stream can only be set once." << std::endl;

	} else {
		body_istream = is;
	}

	/*    char buffer[512];
	     size_t total_size = 0;
	     std::streamsize read_size = 0;
	     do {
	         read_size = is.readsome( buffer, 512 );
	         total_size+= read_size;
	         body_stream.write(buffer, read_size);
	     } while(read_size != 0);

	     size += total_size; */
}

inline std::string time_to_string ( struct tm * time ) {
	char * _time = asctime ( time );
	_time[ ( strlen ( _time ) - 1 )] = '\0';
	return std::string ( _time );
}

size_t HttpResponse::fill_buffer ( char * buffer, size_t buffer_size ) {
	if ( body_istream ) {
		//read body from input stream
		return body_istream->readsome ( buffer, buffer_size - 1 );

	} else {
		// read body from string stream
		return body_stream.readsome ( buffer, buffer_size - 1 );
	}
}

/* std::string HttpResponse::get_message_body() {
    return body_stream.str();
} */

std::string HttpResponse::get_message_header() {
	std::stringstream ss;

	switch ( status_ ) {
	case http_status::OK:
		ss << RESPONSE_LINE_OK;
		break;

	case http_status::PARTIAL_CONTENT:
		ss << RESPONSE_LINE_PARTIAL_CONTENT;
		break;

	case http_status::CREATED:
		ss << RESPONSE_LINE_CREATED;
		break;

	case http_status::ACCEPTED:
		ss << RESPONSE_LINE_ACCEPTED;
		break;

	case http_status::NO_CONTENT:
		ss << RESPONSE_LINE_NO_CONTENT;
		break;

	case http_status::MULTIPLE_CHOICES:
		ss << RESPONSE_LINE_MULTIPLE_CHOICES;
		break;

	case http_status::MOVED_PERMANENTLY:
		ss << RESPONSE_LINE_MOVED_PERMANENTLY;
		break;

	case http_status::MOVED_TEMPORARILY:
		ss << RESPONSE_LINE_MOVED_TEMPORARILY;
		break;

	case http_status::NOT_MODIFIED:
		ss << RESPONSE_LINE_NOT_MODIFIED;
		break;

	case http_status::BAD_REQUEST:
		ss << RESPONSE_LINE_BAD_REQUEST;
		break;

	case http_status::UNAUTHORIZED:
		ss << RESPONSE_LINE_UNAUTHORIZED;
		break;

	case http_status::FORBIDDEN:
		ss << RESPONSE_LINE_FORBIDDEN;
		break;

	case http_status::NOT_FOUND:
		ss << RESPONSE_LINE_NOT_FOUND;
		break;

	case http_status::INTERNAL_SERVER_ERROR:
		ss << RESPONSE_LINE_INTERNAL_SERVER_ERROR;
		break;

	case http_status::NOT_IMPLEMENTED:
		ss << RESPONSE_LINE_NOT_IMPLEMENTED;
		break;

	case http_status::BAD_GATEWAY:
		ss << RESPONSE_LINE_BAD_GATEWAY;
		break;

	case http_status::SERVICE_UNAVAILABLE:
		ss << RESPONSE_LINE_SERVICE_UNAVAILABLE;
		break;

	default:
		ss << RESPONSE_LINE_INTERNAL_SERVER_ERROR;
	}

	if ( size_ > 0 ) {
		std::cout << "set size:" << size_ << std::endl;
		ss << header::CONTENT_LENGTH << std::string ( ": " ) << std::to_string ( size_ ) << LINE_BREAK;
	}

	for ( auto & header : parameters_ ) {
		ss << header.first << std::string ( ": " ) << header.second << LINE_BREAK;
	}

	//add expiration date
	if ( seconds ) {
		time_t now = time ( nullptr );
		struct tm then_tm = *gmtime ( &now );
		then_tm.tm_sec += seconds;
		mktime ( &then_tm );
		ss << header::EXPIRES << std::string ( ": " ) << time_to_string ( &then_tm ) << LINE_BREAK;
	}

	//add last Modified Date
	if ( last_modified ) {
		//TODO ss << HTTP_HEADER_LAST_MODIFIED << std::string(": ") << time_to_string( gmtime( &last_modified ) ) << LINE_BREAK;
	}

	//add now
	time_t now = time ( nullptr );
	struct tm now_tm = *gmtime ( &now );
	mktime ( &now_tm );
	ss << header::DATE << std::string ( ": " ) << time_to_string ( &now_tm ) << LINE_BREAK;

	//add mime-type
	ss << header::CONTENT_TYPE << std::string ( ": " ) << ::http::mime::mime_type ( type ) << LINE_BREAK;

	ss << LINE_BREAK;
	ss.flush();
	return ss.str();
}

void HttpResponse::set_expires ( int seconds ) {
	this->seconds = seconds;
}

void HttpResponse::set_last_modified ( time_t last_modified ) {
	this->last_modified = last_modified;
}

void HttpResponse::set_mime_type ( mime::MIME_TYPE type ) {
	this->type = type;
}

void HttpResponse::reset() {
	if ( body_istream ) {
		delete body_istream; //TODO unique pointer
	}

	body_istream = nullptr;
	body_stream.str ( string ( "" ) );
	parameters_.clear();
	size_ = 0;
}


void HttpResponse::protocol ( const std::string & protocol ) {
	this->protocol_ = protocol;
}

std::string HttpResponse::protocol() {
	return protocol_;
}
void HttpResponse::parameter ( const std::string & key, const std::string & value ) {
	parameters_[key] = value;
}
std::string HttpResponse::parameter ( const std::string & key ) {
	return parameters_[key];
}
bool HttpResponse::containsParameter ( const std::string & key ) {
	return ( parameters_.find ( key ) != parameters_.end() );
}
std::string HttpResponse::body() {
	return body_stream.str();
}

} //http
