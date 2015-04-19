/*
    http parser implementation.
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

#include "commons.h"

namespace http {

enum line_break { NONE, CR, LF } break_type = NONE;
parser_type HttpParser::next ( parser_type t ) {
	switch ( t ) {
	case parser_type::METHOD:
		return parser_type::REQUEST_URI;

	case parser_type::REQUEST_URI:
		return parser_type::REQUEST_PROTOCOL;

	case parser_type::REQUEST_PROTOCOL:
		return parser_type::REQUEST_VERSION_MAJOR;

	case parser_type::REQUEST_VERSION_MAJOR:
		return parser_type::REQUEST_VERSION_MINOR;

	case parser_type::REQUEST_VERSION_MINOR:
		return parser_type::REQUEST_KEY;

	case parser_type::REQUEST_KEY:
		return parser_type::REQUEST_VALUE;

	case parser_type::REQUEST_VALUE:
		return parser_type::REQUEST_BODY;

	default:
		return parser_type::REQUEST_BODY;
	}
}
parser_type HttpParser::nextResponseType ( parser_type t ) {
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
std::ostream& operator<< ( std::ostream& out, const parser_type & type ) {
	switch ( type ) {
	case parser_type::METHOD:
		out << "METHOD";
		break;

	case parser_type::REQUEST_URI:
		out << "REQUEST_URI";
		break;

	case parser_type::REQUEST_PROTOCOL:
		out << "REQUEST_PROTOCOL";
		break;

	case parser_type::REQUEST_VERSION_MAJOR:
		out << "REQUEST_VERSION_MAJOR";
		break;

	case parser_type::REQUEST_VERSION_MINOR:
		out << "REQUEST_VERSION_MINOR";
		break;

	case parser_type::REQUEST_KEY:
		out << "REQUEST_KEY";
		break;

	case parser_type::REQUEST_VALUE:
		out << "REQUEST_VALUE";
		break;

	case parser_type::REQUEST_BODY:
		out << "REQUEST_BODY";
		break;

	default:
		out << "UNKNOWN";
		break;
	}

	return out;
}

/**
 * @brief static function to parse a HTTP request.
 */
PARSE_STATE HttpParser::parse_http_request ( http::HttpRequest & request, const std::array<char, 8192> input, size_t size ) {
	parser_type type = parser_type::METHOD;
	std::stringstream ss_buffer;
	std::string request_key;

	for ( size_t i = 0; i < size; i++ ) { //search line break and configure the parser

		if ( type != parser_type::REQUEST_BODY && input[i] == '\r' ) {
			break_type = CR;

		} else if ( type != parser_type::REQUEST_BODY && input[i] == '\n' ) { //found a real new line
			if ( break_type == CR ) {
				if ( type == parser_type::REQUEST_VERSION_MINOR ) {
					int version_minor = 0;
					ss_buffer >> version_minor;
					request.httpVersionMinor ( version_minor );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = http::HttpParser::next ( type ); //TODO

				} else if ( type == parser_type::REQUEST_VALUE ) {
					request.request_lines[ commons::string::trim ( request_key )] = commons::string::trim ( ss_buffer.str() );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = parser_type::REQUEST_KEY;

				} else if ( type == parser_type::REQUEST_KEY ) {
					type = parser_type::REQUEST_BODY;

				} else {
					std::cout << "unknown type at end of line:" /* << type */ << std::endl;
				}

			} else {
				std::cout << "LF without CR" << std::endl;
			}

		} else { //parse the characters

			switch ( type ) {
			case parser_type::METHOD: {
				if ( input[i] == ' ' ) {
					request.method ( ss_buffer.str() );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = http::HttpParser::next ( type ); //TODO ++

				} else {
					ss_buffer << input[i];
				}

				break;
			}

			case parser_type::REQUEST_URI: {
				if ( input[i] == ' ' ) {
					size_t qmPosition = ss_buffer.str().find ( "?" );

					if ( qmPosition != std::string::npos ) {
						request.uri ( ss_buffer.str().substr ( 0, qmPosition ) );
						utils::get_parameters ( ss_buffer.str().substr ( qmPosition + 1 ), request );

					} else {
						request.uri ( ss_buffer.str() );
					}

					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = http::HttpParser::next ( type ); //TODO

				} else {
					ss_buffer << input[i];
				}

				break;
			}

			case parser_type::REQUEST_PROTOCOL: {
				if ( input[i] == '/' ) {
					request.protocol ( ss_buffer.str() );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = http::HttpParser::next ( type );

				} else {
					ss_buffer << input[i];
				}

				break;
			}

			case parser_type::REQUEST_VERSION_MAJOR: {
				if ( input[i] == '.' ) {
					int version_major = 0;
					ss_buffer >> version_major;
					request.httpVersionMajor ( version_major );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = http::HttpParser::next ( type );

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

	if ( type == parser_type::REQUEST_BODY ) {
		request /*.body =*/ << ss_buffer.str(); //TODO pipe

	} else {
		std::cout << "end type is: " /* << type */;
		std::cout << type;
		std::cout << std::endl;
	}

	//TODO handle states
	if ( type == parser_type::REQUEST_BODY ) {
		return PARSE_STATE::TRUE;

	} else {
		return PARSE_STATE::CONTINUE;
	}
}
PARSE_STATE HttpParser::parse_http_response ( http::HttpResponse & response, const std::array<char, 8192> input, size_t size ) {
	parser_type type = parser_type::REQUEST_PROTOCOL;
	std::stringstream ss_buffer;
	std::string request_key;

	for ( size_t i = 0; i < size; i++ ) { //search line break and configure the parser

		if ( type != parser_type::REQUEST_BODY && input[i] == '\r' ) {
			break_type = CR;

		} else if ( type != parser_type::REQUEST_BODY && input[i] == '\n' ) { //found a real new line
			if ( break_type == CR ) {
				if ( type == parser_type::RESPONSE_STATUS_TEXT ) {
					std::cout << "RESPONSE TEXT" << ss_buffer.str() << std::endl;
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = http::HttpParser::nextResponseType ( type );

				} else if ( type == parser_type::REQUEST_VALUE ) {
					response.add_header ( commons::string::trim ( request_key ), commons::string::trim ( ss_buffer.str() ) );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = parser_type::REQUEST_KEY;

				} else if ( type == parser_type::REQUEST_KEY ) {
					type = parser_type::REQUEST_BODY;

				} else {
					std::cout << "unknown type at end of line:" /* << type */ << std::endl;
				}

			} else {
				std::cout << "LF without CR" << std::endl;
			}

		} else { //parse the characters

			switch ( type ) {

			case parser_type::REQUEST_PROTOCOL: {
				if ( input[i] == '/' ) {
					response.protocol = ss_buffer.str();
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = http::HttpParser::nextResponseType ( type );

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
					type = http::HttpParser::nextResponseType ( type );

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
					type = http::HttpParser::nextResponseType ( type );

				} else {
					ss_buffer << input[i];
				}

				break;
			}

			case parser_type::RESPONSE_STATUS: {
				if ( input[i] == ' ' ) {
					response.status = parse_status ( commons::string::parse_string<int> ( ss_buffer.str() ) );
					ss_buffer.str ( std::string() );
					ss_buffer.clear();
					type = http::HttpParser::nextResponseType ( type );

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

	//TODO handle states
	if ( type == parser_type::REQUEST_BODY ) {
		return PARSE_STATE::TRUE;

	} else {
		return PARSE_STATE::CONTINUE;
	}
}
}
