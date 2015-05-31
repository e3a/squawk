/*
    cover servlet header file.
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

#ifndef COVERSERVLET_H
#define COVERSERVLET_H

#include "squawk.h"
#include "http.h"
#include "fileservlet.h"

namespace squawk {
namespace api {
/**
 * @brief The CoverServlet class
 */
class CoverServlet : public http::servlet::FileServlet {
public:
    CoverServlet ( const std::string & path, http::HttpServletContext context ) :
        FileServlet ( path, context.parameter ( squawk::CONFIG_TMP_DIRECTORY ) ) {}
    ~CoverServlet() {}
    virtual void do_get ( http::HttpRequest & request, http::HttpResponse & response ) override;
};
} // api
} // squawk
#endif // COVERSERVLET_H
