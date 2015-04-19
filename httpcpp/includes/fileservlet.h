/*
    file servlet implementation header
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

#ifndef FILESERVLET_H
#define FILESERVLET_H

#include "http.h"

namespace http {
namespace servlet {

/**
 * @brief The FileServlet class
 * The FileServlet returns static files from the disk.
 */
class FileServlet : public http::HttpServlet {
public:
	explicit FileServlet ( const std::string path, const std::string docroot ) : HttpServlet ( path ), docroot ( docroot ) {}
	virtual void do_get ( HttpRequest & request, HttpResponse & response );
	virtual void do_head ( HttpRequest & request, HttpResponse & response );
private:
	std::string docroot;
};
}
}
#endif // FILESERVLET_H
