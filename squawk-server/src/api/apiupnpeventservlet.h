/*
    API UpnpEvent Servlet
    Copyright (C) 2013  <copyright holder> <e.knecht@netwings.ch>

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

#ifndef APIUPNPEVENT_H
#define APIUPNPEVENT_H

#include <string>

#include "http.h"
#include "ssdp.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace api {
/**
 * @brief The ApiUpnpEvent class
 */
class ApiUpnpEventServlet : public http::HttpServlet {
public:
    ApiUpnpEventServlet ( const std::string & path, didl::SSDPServerImpl * ssdp_server ) :
        HttpServlet ( path ), ssdp_server( ssdp_server ) {}
        ~ApiUpnpEventServlet() {}
        virtual void do_get ( http::HttpRequest & request, http::HttpResponse & response ) override;
private:
        static log4cxx::LoggerPtr logger;
        didl::SSDPServerImpl * ssdp_server;
};
} // api
} // squawk
#endif // APIUPNPEVENT_H
