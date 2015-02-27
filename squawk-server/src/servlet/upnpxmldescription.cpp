/*
    The UPNP Service Description
    Copyright (C) 2013  <etienne> <etienne@gmail.com>

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

#include "upnpxmldescription.h"

#include "xml.h"

using namespace std;

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr UpnpXmlDescription::logger(log4cxx::Logger::getLogger("squawk.servlet.UpnpXmlDescription"));

void UpnpXmlDescription::do_get(::http::HttpRequest & request, ::http::HttpResponse & response) {

    commons::xml::XMLWriter writer;
    commons::xml::Node root_node = writer.element( "root" );
    writer.ns( root_node, "urn:schemas-upnp-org:device-1-0" );

    commons::xml::Node spec_node = writer.element( root_node, "", "specVersion" );
    writer.element( spec_node, "", "major", "1" );
    writer.element( spec_node, "", "minor", "0" );

    commons::xml::Node device_node = writer.element( root_node, "", "device" );
    writer.element( device_node, "", "deviceType", "urn:schemas-upnp-org:device:MediaServer:1" );
    writer.element( device_node, "", "friendlyName", "Squawk Media Server" );
    writer.element( device_node, "", "manufacturer", "spielhuus" );
    writer.element( device_node, "", "manufacturerURL", "http://www.spielhuus.org" );
    writer.element( device_node, "", "modelDescription", "Squawk DLNA Server" );
    writer.element( device_node, "", "modelName", "Windows Media Connect compatible (SquawkDLNA)" );
    writer.element( device_node, "", "modelNumber", "1" );
    writer.element( device_node, "", "modelURL", "http://" );
    writer.element( device_node, "", "serialNumber", "123456" );
    writer.element( device_node, "", "UDN", "uuid:" + config->uuid() );

    commons::xml::Node dlna_node = writer.element( device_node, "", "X_DLNADOC", "DMS-1.50" );
    writer.ns( dlna_node, "urn:schemas-dlna-org:device-1-0", "dlna", true );

    writer.element( device_node, "", "presentationURL", "" );

    commons::xml::Node icon_list_node = writer.element( device_node, "", "iconList" );
    commons::xml::Node icon_small_node = writer.element( icon_list_node, "", "icon" );
    writer.element( icon_small_node, "", "mimetype", "image/png" );
    writer.element( icon_small_node, "", "width", "48" );
    writer.element( icon_small_node, "", "height", "48" );
    writer.element( icon_small_node, "", "depth", "24" );
    writer.element( icon_small_node, "", "url", "/icons/squawk48.png" );

    commons::xml::Node icon_big_node = writer.element( icon_list_node, "", "icon" );
    writer.element( icon_big_node, "", "mimetype", "image/png" );
    writer.element( icon_big_node, "", "width", "64" );
    writer.element( icon_big_node, "", "height", "64" );
    writer.element( icon_big_node, "", "depth", "24" );
    writer.element( icon_big_node, "", "url", "/icons/squawk64.png" );

    commons::xml::Node service_list_node = writer.element( device_node, "", "serviceList" );
    commons::xml::Node service_cds_node = writer.element( service_list_node, "", "service" );
    writer.element( service_cds_node, "", "serviceType", "urn:schemas-upnp-org:service:ContentDirectory:1" );
    writer.element( service_cds_node, "", "serviceId", "urn:upnp-org:serviceId:ContentDirectory" );
    writer.element( service_cds_node, "", "controlURL", "/ctl/ContentDir" );
    writer.element( service_cds_node, "", "eventSubURL", "/evt/ContentDir" );
    writer.element( service_cds_node, "", "SCPDURL", "/ContentDir.xml" );

    commons::xml::Node service_cms_node = writer.element( service_list_node, "", "service" );
    writer.element( service_cms_node, "", "serviceType", "urn:schemas-upnp-org:service:ConnectionManager:1" );
    writer.element( service_cms_node, "", "serviceId", "urn:upnp-org:serviceId:ConnectionManager" );
    writer.element( service_cms_node, "", "controlURL", "/ctl/ConnectionMgr" );
    writer.element( service_cms_node, "", "eventSubURL", "/evt/ConnectionMgr" );
    writer.element( service_cms_node, "", "SCPDURL", "/ConnectionMgr.xml" );

    response << writer.str();
    response.set_mime_type( ::http::mime::XML );
    response.set_status( ::http::http_status::OK );
}
}}
