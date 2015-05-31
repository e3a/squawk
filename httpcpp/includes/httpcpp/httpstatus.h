/*
    http status definition.
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

#ifndef HTTPSTATUS
#define HTTPSTATUS

namespace http {

/**
 * The http status types.
 * @brief http status types.
 */
enum class http_status {
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NO_CONTENT = 204,
	PARTIAL_CONTENT = 206,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	MOVED_TEMPORARILY = 302,
	NOT_MODIFIED = 304,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503
};
inline http_status parse_status ( const int & status ) {
	switch ( status ) {
	case 200:
		return http_status::OK;

	case 201:
		return http_status::CREATED;

	case 202:
		return http_status::ACCEPTED;

	case 204:
		return http_status::NO_CONTENT;

	case 206:
		return http_status::PARTIAL_CONTENT;

	case 300:
		return http_status::MULTIPLE_CHOICES;

	case 301:
		return http_status::MOVED_PERMANENTLY;

	case 302:
		return http_status::MOVED_TEMPORARILY;

	case 304:
		return http_status::NOT_MODIFIED;

	case 400:
		return http_status::BAD_REQUEST;

	case 401:
		return http_status::UNAUTHORIZED;

	case 403:
		return http_status::FORBIDDEN;

	case 404:
		return http_status::NOT_FOUND;

	case 500:
		return http_status::INTERNAL_SERVER_ERROR;

	case 501:
		return http_status::NOT_IMPLEMENTED;

	case 502:
		return http_status::BAD_GATEWAY;

	case 503:
		return http_status::SERVICE_UNAVAILABLE;

	default:
		return http_status::BAD_REQUEST;
	}
}
inline int parse_status ( const http_status & status ) {
	switch ( status ) {
	case http_status::OK:
		return 200;

	case http_status::CREATED:
		return 201;

	case http_status::ACCEPTED:
		return 202;

	case http_status::NO_CONTENT:
		return 204;

	case http_status::PARTIAL_CONTENT:
		return 206;

	case http_status::MULTIPLE_CHOICES:
		return 300;

	case http_status::MOVED_PERMANENTLY:
		return 301;

	case http_status::MOVED_TEMPORARILY:
		return 302;

	case http_status::NOT_MODIFIED:
		return 304;

	case http_status::BAD_REQUEST:
		return 400;

	case http_status::UNAUTHORIZED:
		return 401;

	case http_status::FORBIDDEN:
		return 403;

	case http_status::NOT_FOUND:
		return 404;

	case http_status::INTERNAL_SERVER_ERROR:
		return 500;

	case http_status::NOT_IMPLEMENTED:
		return 501;

	case http_status::BAD_GATEWAY:
		return 502;

	case http_status::SERVICE_UNAVAILABLE:
		return 503;

	default:
		return 400;
	}
}

} //http
#endif // HTTPSTATUS

