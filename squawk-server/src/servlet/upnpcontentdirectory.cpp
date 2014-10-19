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

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr UpnpContentDirectory::logger(log4cxx::Logger::getLogger("squawk.servlet.UpnpContentDirectory"));

void UpnpContentDirectory::registerContentDirectoryModule( commons::upnp::ContentDirectoryModule * module ) {
    modules.insert(modules.end(), module);
}

void UpnpContentDirectory::do_post(::http::HttpRequest & request, ::http::HttpResponse & response) {

    try {
        commons::upnp::UpnpContentDirectoryRequest upnp_command =  commons::upnp::parseRequest( request.body );
        LOG4CXX_DEBUG(logger, upnp_command )

        if( upnp_command.type == commons::upnp::UpnpContentDirectoryRequest::BROWSE ) {
            response << XML_HEADER << XML_START_ENVELOPE << XML_START_BODY << browse( upnp_command ) << XML_END_BODY << XML_END_ENVELOPE;
            response.set_mime_type( ::http::mime::XML );
            response.set_status( ::http::http_status::OK );
            return;

        } else {
            LOG4CXX_WARN(logger, "invoke::Unknown Method: " << upnp_command )
        }

    } catch( commons::upnp::UpnpException & ex ) {
        LOG4CXX_ERROR(logger, "UPNP parse error: " << ex.code() << ":" << ex.what() )
    } catch( commons::xml::XmlException & ex ) {
        LOG4CXX_ERROR(logger, "XML parse error: " << ex.code() << ":" << ex.what() )
    } catch( ... ) {
        LOG4CXX_ERROR(logger, "UPNP parse error." )
    }

    throw ::http::http_status::BAD_REQUEST;
}

std::string UpnpContentDirectory::browse( commons::upnp::UpnpContentDirectoryRequest upnp_command ) {

    if( upnp_command.contains( UPNP_CDS_BROWSE_FLAG ) && upnp_command.getValue( UPNP_CDS_BROWSE_FLAG ) == UPNP_CDS_BROWSE_FLAG_METADATA ) {
        return UPNP_CDS_BROWSE_METADATA;

    } else if( upnp_command.contains( UPNP_CDS_BROWSE_FLAG ) && upnp_command.getValue( UPNP_CDS_BROWSE_FLAG ) == UPNP_CDS_BROWSE_FLAG_DIRECT_CHILDREN && upnp_command.getValue( UPNP_CDS_OBJECT_ID ) == "0") {
        std::stringstream result;
        result << XML_START_BROWSE_RESPONSE << "<Result>" <<
                  "&lt;DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" " <<
                  "xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\"&gt;";

        //search for callback handler
        int count = 0;
        for (std::list< commons::upnp::ContentDirectoryModule * >::iterator it = modules.begin(); it != modules.end(); it++) {
            result << (*it)->getRootNode();
            count++; //TODO get the right children count
        }

        result << "&lt;/DIDL-Lite&gt;</Result>" <<
                  "<NumberReturned>" << count << "</NumberReturned>" <<
                  "<TotalMatches>" << count << "</TotalMatches>" <<
                  "<UpdateID>1</UpdateID></u:BrowseResponse>";
        return result.str();

    } else if( upnp_command.contains( UPNP_CDS_BROWSE_FLAG ) && upnp_command.getValue( UPNP_CDS_BROWSE_FLAG ) == UPNP_CDS_BROWSE_FLAG_DIRECT_CHILDREN ) {
        std::stringstream result;
        result << XML_START_BROWSE_RESPONSE << "<Result>" <<
                  "&lt;DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" " <<
                  "xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" xmlns:pv=\"http://www.pv.com/pvns/\"&gt;";

        //search fall callback handler
        int count = 0;
        for (std::list< commons::upnp::ContentDirectoryModule *>::iterator it = modules.begin(); it != modules.end(); it++) {
            commons::upnp::ContentDirectoryModule * myModule = (*it);
            if(myModule->match( upnp_command )) {
                result << myModule->parseNode(upnp_command );
            }
            count++;//TODO get real counter:
        }

        result << "&lt;/DIDL-Lite&gt;</Result>" <<
                  "<NumberReturned>2</NumberReturned>" <<
                  "<TotalMatches>2</TotalMatches>" <<
                  "<UpdateID>1</UpdateID></u:BrowseResponse>";

        return result.str();

    } else {
        LOG4CXX_DEBUG(logger, "Browse unknown command (" << upnp_command.getValue( UPNP_CDS_BROWSE_FLAG ) << ")")
        return std::string();
    }
}
}}
