/*
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

#ifndef UPNPMEDIASERVLET_H
#define UPNPMEDIASERVLET_H

#include "squawk.h"

namespace squawk {

class UpnpMediaServlet : public http::HttpServlet {
public:
    UpnpMediaServlet ( const std::string & path ) : HttpServlet ( path ) {}
    virtual void do_get ( http::HttpRequest & request, http::HttpResponse & response );
    virtual void do_head ( http::HttpRequest & request, http::HttpResponse & response );
private:
    void _process_file ( http::HttpRequest & request, http::HttpResponse & response );
    void _dlna_headers ( http::HttpRequest & request, http::HttpResponse & response );
};
}//namespace squawk
#endif // UPNPMEDIASERVLET_H
