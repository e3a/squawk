/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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

#include "tinyxml.h"

using namespace std;

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr UpnpXmlDescription::logger(log4cxx::Logger::getLogger("squawk.servlet.UpnpXmlDescription"));

void UpnpXmlDescription::do_get(::http::HttpRequest & request, ::http::HttpResponse & response) {
    //TODO
    std::cout << "handle_request:" << request.uri << std::endl;   //TODO logging

  response << device();
  response.set_mime_type( ::http::mime::XML );
  response.set_status( ::http::http_status::OK );
}

string UpnpXmlDescription::cds() { //TODO remove
  TiXmlDocument doc;
  TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
  doc.LinkEndChild( decl );

  TiXmlElement * element = new TiXmlElement( "scpd" );
  element->SetAttribute("xmlns", "urn:schemas-upnp-org:service-1-0" );
  doc.LinkEndChild( element );

  TiXmlElement * specVersion = create_element(element, "specVersion", "");
  create_element(specVersion, "major", "1");
  create_element(specVersion, "minor", "0");

  TiXmlElement * action_list = create_element(element, "actionList", "");
  TiXmlElement * action_search_capabilities = create_element(action_list, "action", "");
  action_search_capabilities = create_element(action_search_capabilities, "name", "GetSearchCapabilities");
  TiXmlElement * action_search_capabilities_arguments = create_element(action_search_capabilities, "argumentList", "");
  TiXmlElement * action_search_capabilities_arguments_argument = create_element(action_search_capabilities_arguments, "argument", "");
  action_search_capabilities = create_element(action_search_capabilities_arguments_argument, "name", "SearchCaps");
  action_search_capabilities = create_element(action_search_capabilities_arguments_argument, "direction", "out");
  action_search_capabilities = create_element(action_search_capabilities_arguments_argument, "relatedStateVariable", "SearchCapabilities");

  TiXmlPrinter printer;
  doc.Accept( &printer );
  return  printer.CStr();
}

string UpnpXmlDescription::device() {
  TiXmlDocument doc;
  TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
  doc.LinkEndChild( decl );

  TiXmlElement * element = new TiXmlElement( "root" );
  element->SetAttribute("xmlns", "urn:schemas-upnp-org:device-1-0" );
  doc.LinkEndChild( element );


  TiXmlElement * specVersion = create_element(element, "specVersion", "");
  create_element(specVersion, "major", "1");
  create_element(specVersion, "minor", "0");


  //add the device description
  TiXmlElement * device = create_element(element, "device", "");
  create_element(device, "deviceType", "urn:schemas-upnp-org:device:MediaServer:1");
  create_element(device, "friendlyName", "Squawk Media Server");
  create_element(device, "manufacturer", "spielhuus");
  create_element(device, "manufacturerURL", "http://www.spielhuus.org");
  create_element(device, "modelDescription", "Squawk DLNA Server");
  create_element(device, "modelName", "Windows Media Connect compatible (SquawkDLNA)");
  create_element(device, "modelNumber", "1");
  create_element(device, "modelURL", "http://");
  create_element(device, "serialNumber", "123456");
  create_element(device, "UDN", "uuid:" + config->string_value(CONFIG_UUID));
  TiXmlElement * device_xmldoc = create_element(device, "dlna:X_DLNADOC", "DMS-1.50");
  device_xmldoc->SetAttribute("xmlns:dlna", "urn:schemas-dlna-org:device-1-0" );
  create_element(device, "presentationURL", "/");

  TiXmlElement * icon_list = create_element(device, "iconList", "");
  TiXmlElement * icon48 = create_element(icon_list, "icon", "");
  create_element(icon48, "mimetype", "image/png");
  create_element(icon48, "width", "48");
  create_element(icon48, "height", "48");
  create_element(icon48, "depth", "24");
  create_element(icon48, "url", "/icons/squawk48.png");

  TiXmlElement * icon64 = create_element(icon_list, "icon", "");
  create_element(icon64, "mimetype", "image/png");
  create_element(icon64, "width", "64");
  create_element(icon64, "height", "64");
  create_element(icon64, "depth", "24");
  create_element(icon64, "url", "/icons/squawk64.png");

  TiXmlElement * service_list = create_element(device, "serviceList", "");
  TiXmlElement * service_cds = create_element(service_list, "service", "");
  create_element(service_cds, "serviceType", "urn:schemas-upnp-org:service:ContentDirectory:1");
  create_element(service_cds, "serviceId", "urn:upnp-org:serviceId:ContentDirectory");
  create_element(service_cds, "controlURL", "/ctl/ContentDir");
  create_element(service_cds, "eventSubURL", "/evt/ContentDir");
  create_element(service_cds, "SCPDURL", "/ContentDir.xml");

  TiXmlElement * service_cms = create_element(service_list, "service", "");
  create_element(service_cms, "serviceType", "urn:schemas-upnp-org:service:ConnectionManager:1");
  create_element(service_cms, "serviceId", "urn:upnp-org:serviceId:ConnectionManager");
  create_element(service_cms, "controlURL", "/ctl/ConnectionMgr");
  create_element(service_cms, "eventSubURL", "/evt/ConnectionMgr");
  create_element(service_cms, "SCPDURL", "/ConnectionMgr.xml");

  TiXmlPrinter printer;
  doc.Accept( &printer );
  return  printer.CStr();
}

TiXmlElement * UpnpXmlDescription::create_element(TiXmlElement * parent, string name, string value) {
  TiXmlElement * element = new TiXmlElement( name );
  TiXmlText * text = new TiXmlText( value );
  element->LinkEndChild( text );
  parent->LinkEndChild( element );
}
}}
