/*
    image servlet implementation.
    Copyright (C) 2013  e.knecht@netwings.ch

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sstream>

#include "imageservlet.h"

#define QUERY_IMAGE ""

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr ImageServlet::logger(log4cxx::Logger::getLogger("squawk.servlet.ImageServlet"));

void ImageServlet::do_get( ::http::HttpRequest & request, ::http::HttpResponse & response ) {

    int image_id = 0;
    bool result = match(request.uri, &image_id);
    if(result && image_id > 0) {

        std::stringstream ss;
        ss << "/image-" << image_id << ".jpg";
        request.uri = ss.str();

        FileServlet::do_get(request, response);

    } else {
        throw ::http::http_status::BAD_REQUEST;
    }
}
}}
