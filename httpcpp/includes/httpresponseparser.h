/*
    http request parser definition.
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

#ifndef HTTPRESPONSEPARSER_H
#define HTTPRESPONSEPARSER_H

#include "http.h"

namespace http {

class HttpResponseParser {
public:
	HttpResponseParser() {}
	~HttpResponseParser() {}

	/**
	 * @brief Static function to parse a HTTP response.
	 */
	size_t parse_http_response ( http::HttpResponse & response, const std::array<char, 8192> input, size_t size );

private:
	enum line_break { NONE, CR, LF } break_type = NONE;
	enum class parser_type {
		METHOD, REQUEST_URI, REQUEST_PROTOCOL, REQUEST_VERSION_MAJOR,
		REQUEST_VERSION_MINOR, REQUEST_KEY, REQUEST_VALUE, REQUEST_BODY,
		RESPONSE_STATUS, RESPONSE_STATUS_TEXT
	};
	parser_type nextResponseType ( parser_type type );

};
} //http

#endif // HTTPRESPONSEPARSER_H
