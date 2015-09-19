/*
    http utils header
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

#ifndef HTTPUTILS
#define HTTPUTILS

namespace http {
namespace method {
static const std::string    GET     =   "GET";
static const std::string    POST    =   "POST";
static const std::string    HEAD    =   "HEAD";
static const std::string    PUT     =   "PUT";
static const std::string    DELETE  =   "DELETE";
static const std::string    TRACE   =   "TRACE";
static const std::string    OPTIONS =   "OPTIONS";
static const std::string    CONNECT =   "CONNECT";
} //method
} //http

namespace http {
namespace header {
/** Content-Types that are acceptable for the response */
static const std::string    ACCEPT              =   "Accept";
/** The length of the response body in octets (8-bit bytes) */
static const std::string    CONTENT_LENGTH      =   "Content-Length";
/** The content type of the content body */
static const std::string    CONTENT_TYPE        =   "Content-Type";
/** The date and time that the message was sent (in "HTTP-date" format as defined by RFC 2616) */
static const std::string    DATE                =   "Date";
/** The last modified date for the requested object (in "HTTP-date" format as defined by RFC 2616) */
static const std::string    LAST_MODIFIED       =   "Last-Modified";
/** Gives the date/time after which the response is considered stale */
static const std::string    EXPIRES             =   "Expires";
/** The domain name of the server (for virtual hosting), and the TCP port number on which the server is listening */
static const std::string    HOST                =   "Host";
/** Allows a 304 Not Modified to be returned if content is unchanged */
static const std::string    IF_MODIFIED_SINCE   =   "If-Modified-Since";
/** Used to specify directives that MUST be obeyed by all caching mechanisms along the request/response chain */
static const std::string    CACHE_CONTROL       =   "Cache-Control";
/** An opportunity to raise a "File Download" dialogue box for a known MIME type with binary format or suggest a filename for dynamic content. Quotes are necessary with special characters. */
static const std::string    CONTENT_DISPOSITION =   "Content-Disposition";
/** What type of connection the user-agent would prefer */
static const std::string    CONNECTION          =   "Connection";
/** Request only part of an entity. Bytes are numbered from 0. */
static const std::string    RANGE               =   "Range";
} //header
} //http

namespace http {
namespace response {
static const std::string    OK                      = "";
static const std::string    CREATED                 = "<html><head><title>Created</title></head><body><h1>201 Created</h1></body></html>";
static const std::string    ACCEPTED                = "<html><head><title>Accepted</title></head><body><h1>202 Accepted</h1></body></html>";
static const std::string    NO_CONTENT              = "<html><head><title>No Content</title></head><body><h1>204 Content</h1></body></html>";
static const std::string    MULTIPLE_CHOICES        = "<html><head><title>Multiple Choices</title></head><body><h1>300 Multiple Choices</h1></body></html>";
static const std::string    MOVED_PERMANENTLY       = "<html><head><title>Moved Permanently</title></head><body><h1>301 Moved Permanently</h1></body></html>";
static const std::string    MOVED_TEMPORARILY       = "<html><head><title>Moved Temporarily</title></head><body><h1>302 Moved Temporarily</h1></body></html>";
static const std::string    NOT_MODIFIED            = "<html><head><title>Not Modified</title></head><body><h1>304 Not Modified</h1></body></html>";
static const std::string    BAD_REQUEST             = "<html><head><title>Bad Request</title></head><body><h1>400 Bad Request</h1></body></html>";
static const std::string    UNAUTHORIZED            = "<html><head><title>Unauthorized</title></head><body><h1>401 Unauthorized</h1></body></html>";
static const std::string    FORBIDDEN               = "<html><head><title>Forbidden</title></head><body><h1>403 Forbidden</h1></body></html>";
static const std::string    NOT_FOUND               = "<html><head><title>Not Found</title></head><body><h1>404 Not Found</h1></body></html>";
static const std::string    INTERNAL_SERVER_ERROR   = "<html><head><title>Internal Server Error</title></head><body><h1>500 Internal Server Error</h1></body></html>";
static const std::string    NOT_IMPLEMENTED         = "<html><head><title>Not Implemented</title></head><body><h1>501 Not Implemented</h1></body></html>";
static const std::string    BAD_GATEWAY             = "<html><head><title>Bad Gateway</title></head><body><h1>502 Bad Gateway</h1></body></html>";
static const std::string    SERVICE_UNAVAILABLE     = "<html><head><title>Service Unavailable</title></head><body><h1>503 Service Unavailable</h1></body></html>";
} //response
} //http

namespace http {
namespace utils {
/**
 * @brief parse range
 * @param range
 * @return from to int values.
 */
inline std::tuple<int, int> parseRange ( const std::string & range ) {
        std::string clean_range = boost::trim_copy( range );

        if ( boost::starts_with ( clean_range, "bytes=" ) ) {
                clean_range = boost::trim_copy ( clean_range.substr ( 6 ) );
		int start = 0;
		int end = -1;

		if ( clean_range.find ( "-" ) != std::string::npos ) {
			std::string from = clean_range.substr ( 0, clean_range.find ( "-" ) );
                        start = std::stoi ( from );

			if ( clean_range.find ( "-" ) + 1 != clean_range.size() ) {
				std::string to = clean_range.substr ( clean_range.find ( "-" ) + 1, clean_range.size() );
                                end = std::stoi ( to );
			}

			return std::tuple<int, int> ( start, end );
		}
	}

	return std::tuple<int, int> ( 0, -1 );
}
/**
 * @brief Normalize the http parameter key to a camel case style.
 * @param key the key.
 * @return the camel case key.
 */
inline std::string normalize_key ( std::string key ) {
	std::locale loc;
	std::stringstream ss_buffer;
	char lastChar;

	for ( size_t i = 0; i < key.length(); i++ ) {
		if ( i == 0 ) {
			ss_buffer << std::toupper ( key[i], loc );

		} else if ( lastChar == '-' ) {
			ss_buffer << std::toupper ( key[i], loc );

		} else {
			ss_buffer << std::tolower ( key[i], loc );
		}

		lastChar = key[i];
	}

	return ss_buffer.str();
}
/**
 * @brief Get parameters from request line and write them to the request.
 * @param parameters the request line string.
 * @param request the request where to store the parameters.
 */
inline void get_parameters ( std::string parameters, HttpRequest * request ) {
	enum parse_mode { KEY, VALUE } mode = KEY;
	std::stringstream ss_buffer_key;
	std::stringstream ss_buffer_value;

	for ( size_t i = 0; i < parameters.length(); i++ ) {
		if ( parameters[i] == '=' ) {
			mode = VALUE;

		} else if ( parameters[i] == '&' ) {
                    std::string value;
                    url_decode( ss_buffer_value.str() , value);
                        request->attribute ( ss_buffer_key.str(), value );
			ss_buffer_key.str ( std::string() );
			ss_buffer_key.clear();
			ss_buffer_value.str ( std::string() );
			ss_buffer_value.clear();
			mode = KEY;

		} else if ( mode == KEY ) {
			ss_buffer_key << parameters[i];

		} else {
			ss_buffer_value << parameters[i];
		}
	}

	if ( mode == VALUE ) {
            std::string value;
            url_decode( ss_buffer_value.str() , value);
            request->attribute ( ss_buffer_key.str(), value );
	}
}
/**
 * @brief get stock body for status
 * @param status
 * @return
 */
inline const std::string get_stock_body ( const http_status & status ) {
	switch ( status ) {
	case http_status::OK:
		return http::response::OK;

	case http_status::CREATED:
		return http::response::CREATED;

	case http_status::ACCEPTED:
		return http::response::ACCEPTED;

	case http_status::NO_CONTENT:
		return http::response::NO_CONTENT;

	case http_status::MULTIPLE_CHOICES:
		return http::response::MULTIPLE_CHOICES;

	case http_status::MOVED_PERMANENTLY:
		return http::response::MOVED_PERMANENTLY;

	case http_status::MOVED_TEMPORARILY:
		return http::response::MOVED_TEMPORARILY;

	case http_status::NOT_MODIFIED:
		return http::response::NOT_MODIFIED;

	case http_status::BAD_REQUEST:
		return http::response::BAD_REQUEST;

	case http_status::UNAUTHORIZED:
		return http::response::UNAUTHORIZED;

	case http_status::FORBIDDEN:
		return http::response::FORBIDDEN;

	case http_status::NOT_FOUND:
		return http::response::NOT_FOUND;

	case http_status::INTERNAL_SERVER_ERROR:
		return http::response::INTERNAL_SERVER_ERROR;

	case http_status::NOT_IMPLEMENTED:
		return http::response::NOT_IMPLEMENTED;

	case http_status::BAD_GATEWAY:
		return http::response::BAD_GATEWAY;

	case http_status::SERVICE_UNAVAILABLE:
		return http::response::SERVICE_UNAVAILABLE;

	default:
		return http::response::INTERNAL_SERVER_ERROR;
	}
}
} //utils
} //http
#endif // HTTPUTILS

