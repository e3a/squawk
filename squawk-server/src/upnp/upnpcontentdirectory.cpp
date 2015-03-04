/*
    Create the content directory description.
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

#include "upnpcontentdirectory.h"

#include "commons.h"
#include "squawk.h"

namespace squawk {
namespace upnp {

log4cxx::LoggerPtr UpnpContentDirectory::logger(log4cxx::Logger::getLogger("squawk.upnp.UpnpContentDirectory"));

void UpnpContentDirectory::registerContentDirectoryModule( commons::upnp::ContentDirectoryModule * module ) {
    modules.insert( modules.end(), module );
}

void UpnpContentDirectory::do_subscribe(::http::HttpRequest & request, ::http::HttpResponse & response) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "SUBSCRIBE: " << request.body )
    response.set_mime_type( ::http::mime::XML );
    response.set_status( ::http::http_status::OK );
    response.add_header("Content-Type", "text/xml; charset=\"utf-8\"");
    response.add_header("Timeout", "Second-1800");
    response.add_header("SID", "uuid:");
}

void UpnpContentDirectory::do_post(::http::HttpRequest & request, ::http::HttpResponse & response) {

    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, request.body )

    try {
        commons::upnp::UpnpContentDirectoryRequest upnp_command = commons::upnp::parseRequest( request.body );
        LOG4CXX_DEBUG(logger, upnp_command )

        if( upnp_command.type == commons::upnp::UpnpContentDirectoryRequest::BROWSE ) {

            commons::xml::XMLWriter xmlWriter;
            browse( &xmlWriter, &upnp_command );
            response <<  xmlWriter.str();

            response.set_mime_type( ::http::mime::XML );
            response.set_status( ::http::http_status::OK );

        } else if( upnp_command.type == commons::upnp::UpnpContentDirectoryRequest::X_FEATURE_LIST ) {
            LOG4CXX_WARN(logger, "X_GetFeatureList: " << request << "\n" << upnp_command  )
            commons::xml::XMLWriter xmlWriter;
            commons::xml::Node envelope_node = xmlWriter.element( "Envelope" );
            xmlWriter.ns(envelope_node, commons::upnp::XML_NS_SOAP, "s", true);
            xmlWriter.ns(envelope_node, commons::upnp::XML_NS_SOAPENC, "soapenc", false);
            xmlWriter.ns(envelope_node, commons::upnp::XML_NS_SCHEMA, "xsd", false);
            xmlWriter.ns(envelope_node, commons::upnp::XML_NS_SCHEMA_INSTANCE, "xsi", false);
            xmlWriter.attribute(envelope_node, commons::upnp::XML_NS_SOAP, "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");

            commons::xml::Node body_node = xmlWriter.element(envelope_node, commons::upnp::XML_NS_SOAP, "Body");
            commons::xml::Node response_node = xmlWriter.element(body_node, "", "X_GetFeatureListResponse", "");
            xmlWriter.ns(response_node, commons::upnp::XML_NS_UPNP_CDS, "u", true);

            commons::xml::XMLWriter didlWriter;
            commons::xml::Node features_node = didlWriter.element( "Features" );
            xmlWriter.ns(features_node, "urn:schemas-upnp-org:av:avs", "", true );
            xmlWriter.ns(features_node, "http://www.w3.org/2001/XMLSchema-instance", "xsi" );
            xmlWriter.attribute(features_node, "http://www.w3.org/2001/XMLSchema-instance", "encodingStyle", "urn:schemas-upnp-org:av:avs http://www.upnp.org/schemas/av/avs.xsd");

            commons::xml::Node feature_node = didlWriter.element( features_node, "", "Feature" );
            xmlWriter.attribute(feature_node, "", "name", "samsung.com_BASICVIEW");
            xmlWriter.attribute(feature_node, "", "version", "1");

            commons::xml::Node audio_node = didlWriter.element( feature_node, "", "container" );
            xmlWriter.attribute(audio_node, "", "id", "music");
            xmlWriter.attribute(audio_node, "", "type", "object.item.audioItem");

            commons::xml::Node video_node = didlWriter.element( feature_node, "", "container" );
            xmlWriter.attribute(video_node, "", "id", "video");
            xmlWriter.attribute(video_node, "", "type", "object.item.videoItem");

            commons::xml::Node image_node = didlWriter.element( feature_node, "", "container" );
            xmlWriter.attribute(image_node, "", "id", "image");
            xmlWriter.attribute(image_node, "", "type", "object.item.imageItem");

            commons::xml::Node result_node = xmlWriter.element(response_node, "", "FeatureList", didlWriter.str() );

            std::cout << xmlWriter.str() << std::endl;

            response <<  xmlWriter.str();

            response.set_mime_type( ::http::mime::XML );
            response.set_status( ::http::http_status::OK );

        } else {
            LOG4CXX_WARN(logger, "invoke::Unknown Method: " << upnp_command )
            throw ::http::http_status::BAD_REQUEST;
        }

    } catch( commons::upnp::UpnpException & ex ) {
        LOG4CXX_ERROR(logger, "UPNP parse error: " << ex.code() << ":" << ex.what() )
    } catch( commons::xml::XmlException & ex ) {
        LOG4CXX_ERROR(logger, "XML parse error: " << ex.code() << ":" << ex.what() )
    } catch( ... ) {
        LOG4CXX_ERROR(logger, "UPNP parse error." )
    }
}

void UpnpContentDirectory::browse( commons::xml::XMLWriter * xmlWriter, commons::upnp::UpnpContentDirectoryRequest * upnp_command ) {

    commons::xml::Node envelope_node = xmlWriter->element( "Envelope" );
    xmlWriter->ns(envelope_node, commons::upnp::XML_NS_SOAP, "s", true);
    xmlWriter->ns(envelope_node, commons::upnp::XML_NS_SOAPENC, "soapenc", false);
    xmlWriter->ns(envelope_node, commons::upnp::XML_NS_SCHEMA, "xsd", false);
    xmlWriter->ns(envelope_node, commons::upnp::XML_NS_SCHEMA_INSTANCE, "xsi", false);
    xmlWriter->attribute(envelope_node, commons::upnp::XML_NS_SOAP, "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");

    commons::xml::Node body_node = xmlWriter->element(envelope_node, commons::upnp::XML_NS_SOAP, "Body");
    commons::xml::Node response_node = xmlWriter->element(body_node, "", "BrowseResponse", "");
    xmlWriter->ns(response_node, commons::upnp::XML_NS_UPNP_CDS, "u", true);

    commons::xml::XMLWriter didlWriter;
    commons::upnp::CdsResult cds_result;

    // browse metadata
    if( upnp_command->contains( UPNP_CDS_BROWSE_FLAG ) &&
        upnp_command->getValue( UPNP_CDS_BROWSE_FLAG ) == UPNP_CDS_BROWSE_FLAG_METADATA ) {

        commons::xml::Node didl_element = didlWriter.element( "DIDL-Lite" );
        didlWriter.ns(didl_element, commons::upnp::XML_NS_DIDL );
        didlWriter.ns(didl_element, commons::upnp::XML_NS_PURL, "dc");
        didlWriter.ns(didl_element, commons::upnp::XML_NS_DLNA, "dlna");
        didlWriter.ns(didl_element, commons::upnp::XML_NS_UPNP, "upnp");
        didlWriter.ns(didl_element, commons::upnp::XML_NS_PV, "pv");

        commons::xml::Node container_element = didlWriter.element(didl_element, "", "container", "");
        didlWriter.attribute(container_element, "id", "0");
        didlWriter.attribute(container_element, "parentID", "-1");
        didlWriter.attribute(container_element, "restricted", "1");
        didlWriter.attribute(container_element, "childCount", "2");

        commons::xml::Node audio_element = didlWriter.element(container_element, commons::upnp::XML_NS_UPNP, "searchClass", "object.item.audioItem");
        didlWriter.attribute(audio_element, "includeDerived", "1");
        commons::xml::Node image_element = didlWriter.element(container_element, commons::upnp::XML_NS_UPNP, "searchClass", "object.item.imageItem");
        didlWriter.attribute(image_element, "includeDerived", "1");
        commons::xml::Node video_element = didlWriter.element(container_element, commons::upnp::XML_NS_UPNP, "searchClass", "object.item.videoItem");
        didlWriter.attribute(video_element, "includeDerived", "1");
        didlWriter.element(container_element, commons::upnp::XML_NS_PURL, "title", "root");
        didlWriter.element(container_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
        didlWriter.element(container_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

        cds_result.number_returned(1);
        cds_result.total_matches(1);
        cds_result.update_id(1); //TODO

    } else if( upnp_command->contains( UPNP_CDS_BROWSE_FLAG ) &&
               upnp_command->getValue( UPNP_CDS_BROWSE_FLAG ) == UPNP_CDS_BROWSE_FLAG_DIRECT_CHILDREN &&
               upnp_command->getValue( "ObjectID" ) == "0" ) {

        //search for callback handler
        commons::xml::Node didl_element = didlWriter.element( "DIDL-Lite" );
        didlWriter.ns(didl_element, commons::upnp::XML_NS_DIDL );
        didlWriter.ns(didl_element, commons::upnp::XML_NS_PURL, "dc");
        didlWriter.ns(didl_element, commons::upnp::XML_NS_DLNA, "dlna");
        didlWriter.ns(didl_element, commons::upnp::XML_NS_UPNP, "upnp");
        didlWriter.ns(didl_element, commons::upnp::XML_NS_PV, "pv");

        for (std::list< commons::upnp::ContentDirectoryModule * >::iterator it = modules.begin(); it != modules.end(); it++) {
            (*it)->getRootNode( &didlWriter, &didl_element, &cds_result );
        }

    } else if( upnp_command->contains( UPNP_CDS_BROWSE_FLAG ) &&
               upnp_command->getValue( UPNP_CDS_BROWSE_FLAG ) == UPNP_CDS_BROWSE_FLAG_DIRECT_CHILDREN ) {

        //search all callback handler
        for (std::list< commons::upnp::ContentDirectoryModule *>::iterator it = modules.begin(); it != modules.end(); it++) {
            commons::upnp::ContentDirectoryModule * myModule = (*it);
            if( myModule->match( upnp_command ) ) {
                myModule->parseNode( &didlWriter, &cds_result, upnp_command );
            }
        }

    } else {
        LOG4CXX_DEBUG(logger, "Browse unknown command (" << upnp_command->getValue( UPNP_CDS_BROWSE_FLAG ) << ")")
    }
    commons::xml::Node result_node = xmlWriter->element(response_node, "", "Result", didlWriter.str() );
    xmlWriter->attribute(result_node, commons::upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:string");

    commons::xml::Node nr_node = xmlWriter->element(response_node, "", "NumberReturned", commons::string::to_string<int>( cds_result.number_returned() ) );
    xmlWriter->attribute(nr_node, commons::upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:int");
    commons::xml::Node tm_node = xmlWriter->element(response_node, "", "TotalMatches", commons::string::to_string<int>( cds_result.total_matches() ) );
    xmlWriter->attribute(tm_node, commons::upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:int");
    commons::xml::Node uid_node = xmlWriter->element(response_node, "", "UpdateID", commons::string::to_string<int>( cds_result.update_id() ) );
    xmlWriter->attribute(uid_node, commons::upnp::XML_NS_SCHEMA_INSTANCE, "type", "xsd:int");
}
}}
