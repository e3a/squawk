/*
    HttpServlet implementation.
    Copyright (C) 2013  <copyright holder> <email>

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

#define RESPONSE_BODY_OK std::string("")
#define RESPONSE_BODY_CREATED std::string("<html><head><title>Created</title></head><body><h1>201 Created</h1></body></html>")
#define RESPONSE_BODY_ACCEPTED std::string("<html><head><title>Accepted</title></head><body><h1>202 Accepted</h1></body></html>")
#define RESPONSE_BODY_NO_CONTENT std::string("<html><head><title>No Content</title></head><body><h1>204 Content</h1></body></html>")
#define RESPONSE_BODY_MULTIPLE_CHOICES std::string("<html><head><title>Multiple Choices</title></head><body><h1>300 Multiple Choices</h1></body></html>")
#define RESPONSE_BODY_MOVED_PERMANENTLY std::string("<html><head><title>Moved Permanently</title></head><body><h1>301 Moved Permanently</h1></body></html>")
#define RESPONSE_BODY_MOVED_TEMPORARILY std::string("<html><head><title>Moved Temporarily</title></head><body><h1>302 Moved Temporarily</h1></body></html>")
#define RESPONSE_BODY_NOT_MODIFIED std::string("<html><head><title>Not Modified</title></head><body><h1>304 Not Modified</h1></body></html>")
#define RESPONSE_BODY_BAD_REQUEST std::string("<html><head><title>Bad Request</title></head><body><h1>400 Bad Request</h1></body></html>")
#define RESPONSE_BODY_UNAUTHORIZED std::string("<html><head><title>Unauthorized</title></head><body><h1>401 Unauthorized</h1></body></html>")
#define RESPONSE_BODY_FORBIDDEN std::string("<html><head><title>Forbidden</title></head><body><h1>403 Forbidden</h1></body></html>")
#define RESPONSE_BODY_NOT_FOUND std::string("<html><head><title>Not Found</title></head><body><h1>404 Not Found</h1></body></html>")
#define RESPONSE_BODY_INTERNAL_SERVER_ERROR std::string("<html><head><title>Internal Server Error</title></head><body><h1>500 Internal Server Error</h1></body></html>")
#define RESPONSE_BODY_NOT_IMPLEMENTED std::string("<html><head><title>Not Implemented</title></head><body><h1>501 Not Implemented</h1></body></html>")
#define RESPONSE_BODY_BAD_GATEWAY std::string("<html><head><title>Bad Gateway</title></head><body><h1>502 Bad Gateway</h1></body></html>")
#define RESPONSE_BODY_SERVICE_UNAVAILABLE std::string("<html><head><title>Service Unavailable</title></head><body><h1>503 Service Unavailable</h1></body></html>")

namespace http {

inline std::string get_stock_body(http_status status) {
    switch (status) {
    case http_status::OK:
      return RESPONSE_BODY_OK;
    case http_status::CREATED:
      return RESPONSE_BODY_CREATED;
    case http_status::ACCEPTED:
      return RESPONSE_BODY_ACCEPTED;
    case http_status::NO_CONTENT:
      return RESPONSE_BODY_NO_CONTENT;
    case http_status::MULTIPLE_CHOICES:
      return RESPONSE_BODY_MULTIPLE_CHOICES;
    case http_status::MOVED_PERMANENTLY:
      return RESPONSE_BODY_MOVED_PERMANENTLY;
    case http_status::MOVED_TEMPORARILY:
      return RESPONSE_BODY_MOVED_TEMPORARILY;
    case http_status::NOT_MODIFIED:
      return RESPONSE_BODY_NOT_MODIFIED;
    case http_status::BAD_REQUEST:
      return RESPONSE_BODY_BAD_REQUEST;
    case http_status::UNAUTHORIZED:
      return RESPONSE_BODY_UNAUTHORIZED;
    case http_status::FORBIDDEN:
      return RESPONSE_BODY_FORBIDDEN;
    case http_status::NOT_FOUND:
      return RESPONSE_BODY_NOT_FOUND;
    case http_status::INTERNAL_SERVER_ERROR:
      return RESPONSE_BODY_INTERNAL_SERVER_ERROR;
    case http_status::NOT_IMPLEMENTED:
      return RESPONSE_BODY_NOT_IMPLEMENTED;
    case http_status::BAD_GATEWAY:
      return RESPONSE_BODY_BAD_GATEWAY;
    case http_status::SERVICE_UNAVAILABLE:
      return RESPONSE_BODY_SERVICE_UNAVAILABLE;
    default:
      return RESPONSE_BODY_INTERNAL_SERVER_ERROR;
    }
  }

HttpServlet::HttpServlet(std::string _path) {
    re = new pcrecpp::RE( _path.c_str() );
    path = _path;
}
HttpServlet::~HttpServlet() {
    delete re;
}
bool HttpServlet::match(const std::string & request_path) {
    return re->FullMatch(request_path.c_str());
}
void HttpServlet::do_get(HttpRequest & request, HttpResponse & response) {
    create_stock_reply(http_status::NOT_IMPLEMENTED, response);
}
void HttpServlet::do_post(HttpRequest & request, HttpResponse & response) {
    create_stock_reply(http_status::NOT_IMPLEMENTED, response);
}
void HttpServlet::do_delete(HttpRequest & request, HttpResponse & response) {
    create_stock_reply(http_status::NOT_IMPLEMENTED, response);
}
void HttpServlet::do_put(HttpRequest & request, HttpResponse & response) {
    create_stock_reply(http_status::NOT_IMPLEMENTED, response);
}
void HttpServlet::do_head(HttpRequest & request, HttpResponse & response) {
    create_stock_reply(http_status::NOT_IMPLEMENTED, response);
}
void HttpServlet::create_stock_reply(http_status status, HttpResponse & response) {
    response.set_status(status);
    response << get_stock_body(status);
    response.set_mime_type(mime::MIME_TYPE::HTML);
}
void HttpServlet::create_response(HttpResponse & response, ::http::http_status http_status, std::istream * ss, int content_length, mime::MIME_TYPE mime_type ) {
    // Fill out the reply to be sent to the client.
    response.set_status( http_status );
    response.set_istream( ss );
    response.add_header(HTTP_HEADER_CONTENT_LENGTH, std::to_string( content_length ) );
    response.add_header(HTTP_HEADER_CONTENT_TYPE, mime::mime_type( mime_type ) );
};
}
