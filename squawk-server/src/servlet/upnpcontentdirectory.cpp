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

#include "upnpcontentdirectory.h"

// #include "squawk-utils.h"
#include "commons.h"
#include "http.h"

#define XML_HEADER std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?>")
#define XML_START_ENVELOPE std::string("<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">")
#define XML_END_ENVELOPE std::string("</s:Envelope>")
#define XML_START_BODY std::string("<s:Body>")
#define XML_END_BODY std::string("</s:Body>")
#define XML_START_BROWSE_RESPONSE std::string("<u:BrowseResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\">")

#define UPNP_CDS_BROWSE_METADATA std::string( \
      "<u:BrowseResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><Result>" \
      "&lt;DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" " \
      "xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\"&gt;" \
      "&lt;container id=\"0\" parentID=\"-1\" restricted=\"1\" childCount=\"4\"&gt;&lt;upnp:searchClass includeDerived=\"1\"&gt;object.item.audioItem&lt;/upnp:searchClass&gt;" \
      "&lt;upnp:searchClass includeDerived=\"1\"&gt;object.item.imageItem&lt;/upnp:searchClass&gt;&lt;upnp:searchClass includeDerived=\"1\"&gt;" \
      "object.item.videoItem&lt;/upnp:searchClass&gt;&lt;dc:title&gt;root&lt;/dc:title&gt;&lt;upnp:class&gt;object.container.storageFolder&lt;/upnp:class&gt;&lt;upnp:storageUsed&gt;-1&lt;/upnp:storageUsed&gt;&lt;/container&gt;&lt;/DIDL-Lite&gt;</Result>" \
      "<NumberReturned>1</NumberReturned>" \
      "<TotalMatches>1</TotalMatches>" \
      "<UpdateID>2</UpdateID></u:BrowseResponse>")

#define UPNP_CDS_OBJECT_ID "ObjectID"
#define UPNP_CDS_BROWSE_FLAG "BrowseFlag"
#define UPNP_CDS_BROWSE_FLAG_METADATA "BrowseMetadata"
#define UPNP_CDS_BROWSE_FLAG_DIRECT_CHILDREN "BrowseDirectChildren"

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr UpnpContentDirectory::logger(log4cxx::Logger::getLogger("squawk.servlet.UpnpContentDirectory"));

void UpnpContentDirectory::registerContentDirectoryModule(ContentDirectoryModule * module) {
  modules.insert(modules.end(), module);
}

void UpnpContentDirectory::do_post(::http::HttpRequest & request, ::http::HttpResponse & response) {

  LOG4CXX_DEBUG(logger, "HTTP Request:" << request.request_method << " " << request.uri << "\n" << request.body )
  std::string result = parseRequest( request.body );
  if(result.size()>0) {
    std::string soap_string = XML_HEADER + XML_START_ENVELOPE + XML_START_BODY + result + XML_END_BODY + XML_END_ENVELOPE;
    response << soap_string.c_str();

    response.set_mime_type( ::http::mime::XML );
    response.set_status( ::http::http_status::OK );

  } else {
      throw ::http::http_status::BAD_REQUEST;
  }
}

std::string UpnpContentDirectory::parseRequest(std::string r) {

  TiXmlDocument doc;
  doc.Parse(r.c_str(), 0, TIXML_ENCODING_UTF8);

  TiXmlElement * element = doc.RootElement();
  std::string rootName = element->Value();

  std::map<std::string, std::string> attributes;
  std::map<std::string, std::string> namespaces;

  readAttributes(element, &namespaces, &attributes);

  TiXmlElement * element1 = element->FirstChildElement();
  std::string element_name = std::string(element1->Value());
  readAttributes(element1, &namespaces, &attributes);
  if(element_name.find(":", 0) != std::string::npos) {
    std::string prefix = element_name.substr(0, element_name.find(":", 0));
    std::string name = element_name.substr(element_name.find(":", 0)+1, element_name.length());
    if(namespaces.find(prefix) != namespaces.end() && namespaces[prefix] == "http://schemas.xmlsoap.org/soap/envelope/" && name == "Body") {

      element1 = element1->FirstChildElement();
      readAttributes(element1, &namespaces, &attributes);

      std::string element_name = std::string(element1->Value());
      if(element_name.find(":", 0) != std::string::npos) {
    prefix = element_name.substr(0, element_name.find(":", 0));
    name = element_name.substr(element_name.find(":", 0)+1, element_name.length());
    if(namespaces.find(prefix) != namespaces.end() && namespaces[prefix] == "urn:schemas-upnp-org:service:ContentDirectory:1") {
      std::map<std::string, std::string> parameters;
      element1 = element1->FirstChildElement();
      if(element1) {
        do {
          parameters[std::string(element1->Value())] = (element1->GetText() == NULL ? std::string() : std::string(element1->GetText()));
          element1 = element1->NextSiblingElement();
        } while(element1);
        return invoke(name, parameters);
      }
    } else {
      std::cout << "not a contentdirectory element:" << namespaces[prefix] << ":" << name << std::endl;
    }

      } else {
    std::cout << "child element without namespace:" << element_name << std::endl;
      }
    } else {
      std::cout << "element body not found: " << name << std::endl;
    }
  } else {
    std::cout << "element without namespace:" << element_name << std::endl;
  }
}

void UpnpContentDirectory::readAttributes(TiXmlElement * element, std::map<std::string, std::string> * namespaces, std::map<std::string, std::string> * attributes) {
  TiXmlAttribute * attr = element->FirstAttribute();
  if(attr) {
    do {
      std::string str_ns = std::string(attr->Name());
      if( commons::string::starts_with(str_ns, std::string("xmlns:")) ) {
    std::string prefix = str_ns.substr(str_ns.find(":", 0)+1, str_ns.length());
    std::string ns = std::string(attr->Value());
    (*namespaces)[prefix] = ns;
      } else {
    (*attributes)[std::string(attr->Name())] = std::string(attr->Value());
      }
      attr = attr->Next();
    } while(attr);
  }
}

std::string UpnpContentDirectory::invoke(std::string name, std::map<std::string, std::string> parameters) {
  LOG4CXX_DEBUG(logger, "invoke with parameters:" << parameters.size())
    if(name == "Browse") {
      return browse(parameters);
    } else {
      LOG4CXX_WARN(logger, "invoke::Unknown Method: " << name)
      return std::string();
    }
}

std::string UpnpContentDirectory::browse(std::map<std::string, std::string> parameters) {
  LOG4CXX_DEBUG(logger, "Browse (" << parameters.size() << ") ObjectID:" << parameters[UPNP_CDS_OBJECT_ID])

  if(parameters.find(UPNP_CDS_BROWSE_FLAG) != parameters.end() && parameters[UPNP_CDS_BROWSE_FLAG] == UPNP_CDS_BROWSE_FLAG_METADATA) {
    return UPNP_CDS_BROWSE_METADATA;

  } else if(parameters.find(UPNP_CDS_BROWSE_FLAG) != parameters.end() && parameters[UPNP_CDS_BROWSE_FLAG] == UPNP_CDS_BROWSE_FLAG_DIRECT_CHILDREN && parameters[UPNP_CDS_OBJECT_ID] == "0") {
    std::stringstream result;
    result << XML_START_BROWSE_RESPONSE << "<Result>" <<
    "&lt;DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" " <<
    "xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\"&gt;";

      //search for callback handler
      int count = 0;
      for (std::list< ContentDirectoryModule * >::iterator it = modules.begin(); it != modules.end(); it++) {
    result << (*it)->getRootNode();
    count++; //TODO get the right children count
      }

      result << "&lt;/DIDL-Lite&gt;</Result>" <<
      "<NumberReturned>" << count << "</NumberReturned>" <<
      "<TotalMatches>" << count << "</TotalMatches>" <<
      "<UpdateID>1</UpdateID></u:BrowseResponse>";
    return result.str();

  } else if(parameters.find(UPNP_CDS_BROWSE_FLAG) != parameters.end() && parameters[UPNP_CDS_BROWSE_FLAG] == UPNP_CDS_BROWSE_FLAG_DIRECT_CHILDREN) {
    std::stringstream result;
    result << XML_START_BROWSE_RESPONSE << "<Result>" <<
      "&lt;DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" " <<
      "xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" xmlns:pv=\"http://www.pv.com/pvns/\"&gt;";

      //search fall callback handler
      int count = 0;
      for (std::list<ContentDirectoryModule *>::iterator it = modules.begin(); it != modules.end(); it++) {
    ContentDirectoryModule * myModule = (*it);
    if(myModule->match(parameters)) {
      result << myModule->parseNode(parameters);
    }
    count++;//TODO get real counter:
      }

      result << "&lt;/DIDL-Lite&gt;</Result>" <<
    "<NumberReturned>2</NumberReturned>" <<
    "<TotalMatches>2</TotalMatches>" <<
    "<UpdateID>1</UpdateID></u:BrowseResponse>";

    return result.str();

  } else {
    LOG4CXX_DEBUG(logger, "Browse unknown command (" << parameters[UPNP_CDS_BROWSE_FLAG] << ")")
    return std::string();
  }
}
}}
