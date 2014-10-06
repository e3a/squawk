/*
    cover servlet implementation.
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

#include "coverservlet.h"

namespace squawk {
namespace servlet {

void CoverServlet::do_get(::http::HttpRequest & request, ::http::HttpResponse & response) {

    int cover_id = 0;
    bool result = match(request.uri, &cover_id);
    if(result && cover_id > 0) {

        std::stringstream ss;
        ss << std::string("/") << cover_id  << std::string(".jpg");
        request.uri = ss.str();

        FileServlet::do_get(request, response);

    } else {
        throw ::http::http_status::BAD_REQUEST;
    }
}
}}
