/*
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

namespace squawk {

void UpnpConnectionManager::do_post ( http::HttpRequest & request, http::HttpResponse & response ) {

    try {
        upnp::UpnpContentDirectoryRequest upnp_command = upnp::parseRequest ( request.requestBody() );
        CLOG ( TRACE, "upnp" ) << "Default:" << upnp_command;

        if ( upnp_command.type == upnp::UpnpContentDirectoryRequest::PROTOCOL_INFO ) {

            commons::xml::XMLWriter xmlWriter;
            commons::xml::Node envelope_node = xmlWriter.element ( "Envelope" );
            xmlWriter.ns ( envelope_node, upnp::XML_NS_SOAP, "s", true );
            xmlWriter.ns ( envelope_node, upnp::XML_NS_SOAPENC, "soapenc", false );
            xmlWriter.ns ( envelope_node, upnp::XML_NS_SCHEMA, "xsd", false );
            xmlWriter.ns ( envelope_node, upnp::XML_NS_SCHEMA_INSTANCE, "xsi", false );
            xmlWriter.attribute ( envelope_node, upnp::XML_NS_SOAP, "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/" );

            commons::xml::Node body_node = xmlWriter.element ( envelope_node, upnp::XML_NS_SOAP, "Body" );
            commons::xml::Node response_node = xmlWriter.element ( body_node, "", "GetProtocolInfoResponse", "" );
            xmlWriter.ns ( response_node, upnp::XML_NS_UPNP_CMS, "u", true );

            commons::xml::Node source_node = xmlWriter.element ( response_node, "", "Source", "http-get:*:audio/mpeg:*,http-get:*:text/plain:*,http-get:*:video/mp4:*,http-get:*:video/mpeg:*,http-get:*:video/quicktime:*" /*SOURCE_TYPES*/ );
            xmlWriter.attribute ( source_node, upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:string" );
            commons::xml::Node sink_node = xmlWriter.element ( response_node, "", "Sink", "" );
            xmlWriter.attribute ( sink_node, upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:string" );

            response <<  xmlWriter.str();

            response.set_mime_type ( http::mime::XML );
            response.status ( http::http_status::OK );
            return;

        } else {
            CLOG ( ERROR, "upnp" ) << "invoke::Unknown Method: " << upnp_command;
        }

    } catch ( upnp::UpnpException & ex ) {
        CLOG ( ERROR, "upnp" ) << "UPNP parse error: " << ex.code() << ":" << ex.what();

    } catch ( commons::xml::XmlException & ex ) {
        CLOG ( ERROR, "upnp" ) << "XML parse error: " << ex.code() << ":" << ex.what();

    } catch ( ... ) {
        CLOG ( ERROR, "upnp" ) << "UPNP parse error.";
    }

    throw http::http_status::BAD_REQUEST;
}
void UpnpConnectionManager::do_default ( const std::string & method, http::HttpRequest & request, http::HttpResponse & response ) {
    CLOG ( TRACE, "upnp" ) << "Default:" << request;
    response.parameter ( http::header::CONTENT_LENGTH, "0" );
    response.parameter ( "SID", "uid:b419974c-9784-11e5-9793-9ae5d810837d" ); // TODO real UID
    response.parameter ( "TIMEOUT", "Second-1800" ); // TODO parameter
}
} // squawk
