/*
    api upnp device list servlet implementation.
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

#include "apiupnpdeviceservlet.h"

#include "ssdp.h"
#include "xml.h"

#include <sstream>

namespace squawk {
namespace api {

void ApiUpnpDeviceServlet::do_get( http::HttpRequest &, http::HttpResponse & response) {

    std::map< std::string, ssdp::SsdpEvent > devices = ssdp_server->getUpnpDevices( "upnp:rootdevice" );

    response << "[";
    bool first_device = true;
    for( auto & event : devices ) {
        try {
            ssdp::UpnpDevice device = ssdp_server->deviceDescription( event.second );
            if(first_device) first_device = false;
            else response << ",";
            response << "{";

            response << "\"versionMajor\":\"" << std::to_string( device.versionMajor() ) << "\",";
            response << "\"versionMinor\":\"" << std::to_string( device.versionMinor() ) << "\",";
            response << "\"deviceType\":\"" << device.deviceType() << "\",";
            response << "\"friendlyName\":\"" << device.friendlyName() << "\",";
            response << "\"manufacturer\":\"" << device.manufacturer() << "\",";
            response << "\"manufacturerUrl\":\"" << device.manufacturerUrl() << "\",";
            response << "\"modelDescription\":\"" << device.modelDescription() << "\",";
            response << "\"modelName\":\"" << device.modelName() << "\",";
            response << "\"modelNumber\":\"" << device.modelNumber() << "\",";
            response << "\"modelUrl\":\"" << device.modelUrl() << "\",";
            response << "\"serialNumber\":\"" << device.serialNumber() << "\",";
            response << "\"udn\":\"" << device.udn() << "\",";
            response << "\"upc\":\"" << device.upc() << "\",";
            response << "\"presentationUrl\":\"" << device.presentationUrl() << "\",";

            response << "\"icons\":[";
            bool first_icon = true;
            for( auto & icon : device.iconList() ) {
                if(first_icon) first_icon = false;
                else response << ",";
                response << "{\"width\":" << std::to_string( icon.width() );
                response << ",\"height\":" << std::to_string( icon.height() );
                response << ",\"depth\":" << std::to_string( icon.depth() );
                response << ",\"mimeType\":\"" << icon.mimeType();
                response << "\",\"url\":\"" << icon.url() << "\"}";
            }
            response << "], \"services\":[";
            bool first_service = true;
            for( auto & service : device.serviceList() ) {
                if(first_service) first_service = false;
                else response << ",";
                response << "{\"serviceType\":\"" << service.serviceType();
                response << "\",\"serviceId\":\"" << service.serviceId();
                response << "\",\"scpdUrl\":\"" << service.scpdUrl();
                response << "\",\"controlUrl\":\"" << service.controlUrl();
                response << "\",\"eventSubUrl\":\"" << service.eventSubUrl() << "\"}";
            }
            response << "]}";

        } catch( commons::xml::XmlException & ex ) {
            std::cerr << "can not parse xml file: " << ex.what() << std::endl;
        } catch( ... ) {
            std::cerr << "other exception in get description." << std::endl;
        }
    }

    response << "]";
    response.set_mime_type ( http::mime::JSON );
    response.status ( http::http_status::OK );
}
} // api
} // squawk
