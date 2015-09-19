/*
    api upnp event list servlet implementation.
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

#include "apiupnpeventservlet.h"

#include "ssdp.h"

#include <sstream>

namespace squawk {
namespace api {

void ApiUpnpEventServlet::do_get( http::HttpRequest &, http::HttpResponse & response) {

    std::map< std::string, ssdp::SsdpEvent > devices = ssdp_server->get_upnp_devices();

    response << "[";
    bool first_device = true;

    for( auto & device : devices ) {
        if(first_device) first_device = false;
        else response << ",";
        response << "{";

        response << "\"host\":\"" << device.second.host() << "\",";
        response << "\"location\":\"" << device.second.location() << "\",";
        response << "\"nt\":\"" << device.second.nt() << "\",";
        response << "\"nts\":\"" << device.second.nts() << "\",";
        response << "\"server\":\"" << device.second.server() << "\",";
        response << "\"usn\":\"" << device.second.usn() << "\"";

        response << "}";
    }

    response << "]";
    response.set_mime_type ( http::mime::JSON );
    response.status ( http::http_status::OK );
}
} // api
} // squawk
