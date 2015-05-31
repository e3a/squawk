/*
    http request parser implementation.
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

#ifndef HTTPREQUESTPARSER_H
#define HTTPREQUESTPARSER_H

#include "http.h"

namespace http {

/**
 * @brief The HTTP request parser class.
 */
class HttpRequestParser {
public:
	HttpRequestParser() : type ( parser_type::METHOD ), request_key ( "" ), body_length ( 0 ) {}
	~HttpRequestParser() {}

	/**
	 * @brief parse http request from buffer.
	 * @param request The Request object.
	 * @param input the input array.
	 * @param size valid size of the array.
	 * @return the position of the body or 0 if header is incomplete.
	 */
        size_t parse_http_request ( http::HttpRequest * request, const std::array<char, 8192> input, size_t size );
	/**
	 * @brief reset the parser.
	 */
	void reset();
private:
	enum class parser_type {
		METHOD, REQUEST_URI, REQUEST_PROTOCOL, REQUEST_VERSION_MAJOR,
		REQUEST_VERSION_MINOR, REQUEST_KEY, REQUEST_VALUE, REQUEST_BODY,
		RESPONSE_STATUS, RESPONSE_STATUS_TEXT
	};
	parser_type type;
	std::string request_key;
	size_t body_length;
	std::stringstream ss_buffer;
	enum line_break { NONE, CR, LF } break_type = NONE;
	parser_type next ( parser_type t );
};
} // http
#endif // HTTPREQUESTPARSER_H
