/*
    The UPNP Connection Manager
    Copyright (C) 2013  <etienne> <etienne@mail.com>

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

#include "upnpconnectionmanager.h"

#include "commons.h"
#include "squawk.h"

namespace squawk {
namespace upnp {

log4cxx::LoggerPtr UpnpConnectionManager::logger(log4cxx::Logger::getLogger("squawk.servlet.UpnpConnectionManager"));

void UpnpConnectionManager::do_post(::http::HttpRequest & request, ::http::HttpResponse & response) {

    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, request.requestBody() )

    try {
        commons::upnp::UpnpContentDirectoryRequest upnp_command = commons::upnp::parseRequest( request.requestBody() );
        LOG4CXX_DEBUG(logger, upnp_command )

        if( upnp_command.type == commons::upnp::UpnpContentDirectoryRequest::PROTOCOL_INFO ) {

            commons::xml::XMLWriter xmlWriter;
            commons::xml::Node envelope_node = xmlWriter.element( "Envelope" );
            xmlWriter.ns(envelope_node, commons::upnp::XML_NS_SOAP, "s", true);
            xmlWriter.ns(envelope_node, commons::upnp::XML_NS_SOAPENC, "soapenc", false);
            xmlWriter.ns(envelope_node, commons::upnp::XML_NS_SCHEMA, "xsd", false);
            xmlWriter.ns(envelope_node, commons::upnp::XML_NS_SCHEMA_INSTANCE, "xsi", false);
            xmlWriter.attribute(envelope_node, commons::upnp::XML_NS_SOAP, "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");

            commons::xml::Node body_node = xmlWriter.element(envelope_node, commons::upnp::XML_NS_SOAP, "Body");
            commons::xml::Node response_node = xmlWriter.element(body_node, "", "GetProtocolInfoResponse", "");
            xmlWriter.ns(response_node, commons::upnp::XML_NS_UPNP_CMS, "u", true);

            commons::xml::Node source_node = xmlWriter.element(response_node, "", "Source", "http-get:*:text/plain:*,http-get:*:video/mp4:*,http-get:*:video/mpeg:*,http-get:*:video/quicktime:*" /*SOURCE_TYPES*/);
            xmlWriter.attribute(source_node, commons::upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:string");
            commons::xml::Node sink_node = xmlWriter.element(response_node, "", "Sink", "");
            xmlWriter.attribute(sink_node, commons::upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:string");

            response <<  xmlWriter.str();

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
}}
