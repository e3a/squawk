/*
    api upnp device list servlet definition.
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

#ifndef APIUPNPDEVICESERVLET_H
#define APIUPNPDEVICESERVLET_H

#include <string>

#include "squawk.h"
#include "http.h"
#include "ssdp.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace api {

/**
 * @brief The ApiUpnpDeviceServlet class
 */
class ApiUpnpDeviceServlet : public ::http::HttpServlet {
public:
        ApiUpnpDeviceServlet ( const std::string & path, didl::SSDPServerImpl * ssdp_server ) :
            HttpServlet ( path ), ssdp_server( ssdp_server ) {}
        virtual void do_get ( ::http::HttpRequest & request, ::http::HttpResponse & response );
private:
        static log4cxx::LoggerPtr logger;
        didl::SSDPServerImpl * ssdp_server;
};
} // api
} //squawk
#endif // APIUPNPDEVICESERVLET_H
