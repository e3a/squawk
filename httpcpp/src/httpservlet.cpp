/*
    HttpServlet implementation.
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

namespace http {

HttpServlet::HttpServlet ( std::string _path ) {
	re = new pcrecpp::RE ( _path.c_str() );
	path = _path;
}
HttpServlet::~HttpServlet() {
	delete re;
}
bool HttpServlet::match ( const std::string & request_path ) {
	return re->FullMatch ( request_path.c_str() );
}
void HttpServlet::do_get ( HttpRequest&, HttpResponse & response ) {
	create_stock_reply ( http_status::NOT_IMPLEMENTED, response );
}
void HttpServlet::do_post ( HttpRequest&, HttpResponse & response ) {
	create_stock_reply ( http_status::NOT_IMPLEMENTED, response );
}
void HttpServlet::do_delete ( HttpRequest&, HttpResponse & response ) {
	create_stock_reply ( http_status::NOT_IMPLEMENTED, response );
}
void HttpServlet::do_put ( HttpRequest&, HttpResponse & response ) {
	create_stock_reply ( http_status::NOT_IMPLEMENTED, response );
}
void HttpServlet::do_head ( HttpRequest&, HttpResponse & response ) {
	create_stock_reply ( http_status::NOT_IMPLEMENTED, response );
}
void HttpServlet::do_trace ( HttpRequest&, HttpResponse & response ) {
	create_stock_reply ( http_status::NOT_IMPLEMENTED, response );
}
void HttpServlet::do_options ( HttpRequest&, HttpResponse & response ) {
	create_stock_reply ( http_status::NOT_IMPLEMENTED, response );
}
void HttpServlet::do_connect ( HttpRequest&, HttpResponse & response ) {
	create_stock_reply ( http_status::NOT_IMPLEMENTED, response );
}
void HttpServlet::do_default ( const std::string&, HttpRequest&, HttpResponse & response ) {
	create_stock_reply ( http_status::NOT_IMPLEMENTED, response );
}

void HttpServlet::create_stock_reply ( http_status status, HttpResponse & response ) {
	response.status ( status );
	response << http::utils::get_stock_body ( status );
	response.set_mime_type ( mime::MIME_TYPE::HTML );
}
void HttpServlet::create_response ( HttpResponse & response, ::http::http_status http_status, std::istream * ss, int content_length, mime::MIME_TYPE mime_type ) {
	// Fill out the reply to be sent to the client.
	response.status ( http_status );
	response.set_istream ( ss );
	response.parameter ( header::CONTENT_LENGTH, std::to_string ( content_length ) );
	response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime_type ) );
}
} //http
