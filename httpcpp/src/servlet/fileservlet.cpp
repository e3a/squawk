/*
    file servlet implementation
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

#include "fileservlet.h"

#include <fcntl.h>
#include <sys/sendfile.h>
#include <fstream>
#include <iostream>
#include <time.h>

#include <sys/stat.h>

#include "http.h"
#include "mimetypes.h"

namespace http {
namespace servlet {

//TODO handle relative path
void FileServlet::do_get(HttpRequest & request, HttpResponse & response) {

    if (request.uri.empty() || request.uri[0] != '/' || request.uri.find("..") != std::string::npos) {
      throw http_status::BAD_REQUEST;
    }

    std::string full_path = docroot + request.uri;

    struct stat filestatus;
    stat( full_path.c_str(), &filestatus );
    if(S_ISDIR(filestatus.st_mode)) {
        full_path += std::string("/index.html");
        stat( full_path.c_str(), &filestatus );
    }
    std::cout << "FileServlet Full Path:" << full_path << std::endl;

    /*/test if the file has changed
    struct tm * timeinfo = gmtime ( &filestatus.st_mtime );
    char * _time = asctime(timeinfo);
    _time[(strlen(_time)-1)] = '\0';
    if( request.request_lines.find(HTTP_HEADER_IF_MODIFIED_SINCE) != request.request_lines.end() &&
        request.request_lines[HTTP_HEADER_IF_MODIFIED_SINCE] == std::string(_time) ) {
        throw http_status::NOT_MODIFIED;
    } */

    // Determine the filename and extension.
    std::size_t last_slash_pos = full_path.find_last_of("/");
    std::size_t last_dot_pos = full_path.find_last_of(".");
    std::string extension, filename;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
      extension = full_path.substr(last_dot_pos + 1);
    }
    if (last_slash_pos != std::string::npos && last_slash_pos < full_path.size() ) {
      filename = full_path.substr(last_slash_pos + 1 );
    }

    // Open the file to send back.
    std::ifstream * is = new std::ifstream(full_path.c_str(), std::ios::in | std::ios::binary);
    if ( !is->is_open() ) {
        std::cout << "can not open file:" << full_path << std::endl;
        throw http_status::NOT_FOUND;
    }

    // Fill out the reply to be sent to the client.
    response.add_header( HTTP_HEADER_CONTENT_LENGTH, std::to_string( filestatus.st_size ) );
    response.add_header( HTTP_HEADER_CONTENT_DISPOSITION, "inline; filename= \"" + filename + "\"" );
    response.set_status( http_status::OK );
    response.set_mime_type( ::http::mime::mime_type(extension) );
    response.set_last_modified( filestatus.st_mtime );
    response.set_expires( 3600 * 24 );
    response.set_istream( is );
}
void FileServlet::do_head(HttpRequest & request, HttpResponse & response) {

    if (request.uri.empty() || request.uri[0] != '/' || request.uri.find("..") != std::string::npos) {
      throw http_status::BAD_REQUEST;
    }

    std::string full_path = docroot + request.uri;

    struct stat filestatus;
    stat( full_path.c_str(), &filestatus );
    if(S_ISDIR(filestatus.st_mode)) {
        full_path += std::string("/index.html");
        stat( full_path.c_str(), &filestatus );
    }
    std::cout << "FileServlet Full Path:" << full_path << std::endl;

    // Determine the filename and extension.
    std::size_t last_slash_pos = full_path.find_last_of("/");
    std::size_t last_dot_pos = full_path.find_last_of(".");
    std::string extension, filename;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
      extension = full_path.substr(last_dot_pos + 1);
    }
    if (last_slash_pos != std::string::npos && last_slash_pos < full_path.size() ) {
      filename = full_path.substr(last_slash_pos + 1 );
    }

    // Open the file to send back.
    std::ifstream * is = new std::ifstream(full_path.c_str(), std::ios::in | std::ios::binary);
    if ( !is->is_open() ) {
        std::cout << "can not open file:" << full_path << std::endl;
        throw http_status::NOT_FOUND;
    }
    delete is;

    // Fill out the reply to be sent to the client.
    response.add_header( HTTP_HEADER_CONTENT_LENGTH, std::to_string( filestatus.st_size ) );
    response.add_header( HTTP_HEADER_CONTENT_DISPOSITION, "inline; filename= \"" + filename + "\"" );
    response.set_status( http_status::OK );
    response.set_mime_type( ::http::mime::mime_type(extension) );
    response.set_last_modified( filestatus.st_mtime );
    response.set_expires( 3600 * 24 );
}
}}
