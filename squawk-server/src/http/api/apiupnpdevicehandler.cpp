/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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


#include "apiupnpdevicehandler.h"

#include "ssdp.h"

#include <sstream>

namespace squawk {
namespace http {
namespace api {

void ApiUpnpDeviceHandler::do_get(::http::HttpRequest & request, ::http::HttpResponse & response) {

    std::map< std::string, squawk::ssdp::UpnpDevice > devices = ssdp_server->get_upnp_devices();
    std::stringstream * buf = new std::stringstream();
    (*buf) << "{\"upnp_devices\":[";
    bool first_device = true;

    for(std::map< string, squawk::ssdp::UpnpDevice >::iterator iter = devices.begin(); iter != devices.end(); ++iter) {
        if(first_device) first_device = false;
        else (*buf) << ",";
        (*buf) << (*iter).second;
    }

    (*buf) << "]}";
    create_response(response, ::http::http_status::OK, buf, (*buf).tellp(), ::http::mime::JSON );
}
}}}
