/*
    Upnp video directory module.
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

#include "upnpvideodirectory.h"

#include "mimetypes.h"

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr UpnpVideoDirectory::logger( log4cxx::Logger::getLogger( "squawk.servlet.UpnpVideoDirectory" ) );

bool UpnpVideoDirectory::match( commons::upnp::UpnpContentDirectoryRequest * request ) {
    if( request->contains( "ObjectID" ) && commons::string::starts_with( request->getValue( "ObjectID" ), "video" ) ) {
        return true;
    } else return false;
}
void UpnpVideoDirectory::getRootNode( commons::xml::XMLWriter * xmlWriter, commons::xml::Node * element, commons::upnp::CdsResult * cds_result ) {

    commons::xml::Node container_element = xmlWriter->element( (*element), "", "container", "" );
    xmlWriter->attribute(container_element, "id", "video");
    xmlWriter->attribute(container_element, "parentID", "0");
    xmlWriter->attribute(container_element, "restricted", "1");
    xmlWriter->attribute(container_element, "childCount", "2");

    xmlWriter->element(container_element, commons::upnp::XML_NS_PURL, "title", "Video");
    xmlWriter->element(container_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
    xmlWriter->element(container_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

    cds_result->number_returned( cds_result->number_returned()+1 );
    cds_result->total_matches( cds_result->total_matches()+1 );
}
void UpnpVideoDirectory::parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result,
                                          commons::upnp::UpnpContentDirectoryRequest * request ) {

    commons::xml::Node didl_element = xmlWriter->element( "DIDL-Lite" );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DIDL );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PURL, "dc");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DLNA, "dlna");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_UPNP, "upnp");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PV, "pv");

    /* ----------- Root Node ----------- */
    if( request->contains( "ObjectID" ) && request->getValue( "ObjectID" ) == "video" ) {

        videos([&] (const int id, const std::string & name, const std::string & mime_type ) {

           commons::xml::Node item_element = xmlWriter->element( didl_element, "", "item", "" );
           xmlWriter->attribute(item_element, "id", "/video/" + commons::string::to_string<int>(id) );
           xmlWriter->attribute(item_element, "parentID", "/video" );
           xmlWriter->attribute(item_element, "restricted", "1");

           xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "title", name );
           xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "class", "object.item.videoItem" );

           xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "date", /* year_ + */ "2014-01-01" ); //TODO

           commons::xml::Node dlna_res_node = xmlWriter->element(item_element, "", "res",
               "http://" +squawk_config->httpAddress() + ":" + commons::string::to_string( squawk_config->httpPort() ) +
               "/video/" + commons::string::to_string( id ) + "." + http::mime::extension( mime_type ) );
           xmlWriter->attribute(dlna_res_node, "", "protocolInfo",
               "http-get:*:" + mime_type  + ":DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000" );

    });
    int video_count = videoCount();
    cds_result->number_returned( video_count );
    cds_result->total_matches( video_count );


    } else {
        LOG4CXX_WARN(logger,"unknown request: " << request->getValue( "ObjectID" ) );
    }
}
}}
