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

#include "httpcpp/httpresponseparser.h"

namespace http {

HttpResponseParser::parser_type HttpResponseParser::nextResponseType ( HttpResponseParser::parser_type t ) {
	switch ( t ) {
	case parser_type::REQUEST_PROTOCOL:
		return parser_type::REQUEST_VERSION_MAJOR;

	case parser_type::REQUEST_VERSION_MAJOR:
		return parser_type::REQUEST_VERSION_MINOR;

	case parser_type::REQUEST_VERSION_MINOR:
		return parser_type::RESPONSE_STATUS;

	case parser_type::RESPONSE_STATUS:
		return parser_type::RESPONSE_STATUS_TEXT;

	case parser_type::RESPONSE_STATUS_TEXT:
		return parser_type::REQUEST_KEY;

	default:
		return parser_type::REQUEST_BODY;
	}
}

size_t HttpResponseParser::parse_http_response ( http::HttpResponse & response, const std::array<char, BUFFER_SIZE> input, size_t size ) {
	parser_type type = parser_type::REQUEST_PROTOCOL;
	std::stringstream ss_buffer;
	std::string request_key;

	for ( size_t i = 0; i < size; i++ ) { //search line break and configure the parser

		if ( type != parser_type::REQUEST_BODY && input[i] == '\r' ) {
			break_type = CR;

		} else if ( type != parser_type::REQUEST_BODY && input[i] == '\n' ) { //found a real new line
			if ( break_type == CR ) {
				if ( type == parser_type::RESPONSE_STATUS_TEXT ) {
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = nextResponseType ( type );

				} else if ( type == parser_type::REQUEST_VALUE ) {
                    response.parameter ( boost::trim_copy ( request_key ), boost::trim_copy ( ss_buffer.str() ) );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = parser_type::REQUEST_KEY;

				} else if ( type == parser_type::REQUEST_KEY ) {
					type = parser_type::REQUEST_BODY;
					return i + 1;

				} else if ( DEBUG ) {
					std::cerr << "unknown type at end of line:" /* << type */ << std::endl;
				}

			} else if ( DEBUG ) {
				std::cerr << "LF without CR" << std::endl;
			}

		} else { //parse the characters

			switch ( type ) {

			case parser_type::REQUEST_PROTOCOL: {
				if ( input[i] == '/' ) {
					response.protocol ( ss_buffer.str() );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = nextResponseType ( type );

				} else {
					ss_buffer << input[i];
				}

				break;
			}

			case parser_type::REQUEST_VERSION_MAJOR: {
				if ( input[i] == '.' ) {
					ss_buffer >> response.http_version_major;
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = nextResponseType ( type );

				} else {
					ss_buffer << input[i];
				}

				break;
			}

			case parser_type::REQUEST_VERSION_MINOR: {
				if ( input[i] == ' ' ) {
					ss_buffer >> response.http_version_minor;
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = nextResponseType ( type );

				} else {
					ss_buffer << input[i];
				}

				break;
			}

			case parser_type::RESPONSE_STATUS: {
				if ( input[i] == ' ' ) {
                    response.status ( parse_status ( std::stoi( ss_buffer.str() ) ) );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = nextResponseType ( type );

				} else {
					ss_buffer << input[i];
				}

				break;
			}

			case parser_type::REQUEST_KEY: {
				if ( input[i] == ':' ) {
					type = parser_type::REQUEST_VALUE;
					request_key = utils::normalize_key ( ss_buffer.str() );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();

				} else {
					ss_buffer << input[i];
				}

				break;
			}

			default:
				ss_buffer << input[i];
			}
		}
	}

	return -1;
}
} //http

