#include "upnpimagedirectory.h"
#include "commons.h"
#include "http.h"

namespace squawk {
namespace upnp {

log4cxx::LoggerPtr UpnpImageDirectory::logger( log4cxx::Logger::getLogger( "squawk.upnp.UpnpImageDirectory" ) );

bool UpnpImageDirectory::match( commons::upnp::UpnpContentDirectoryRequest * request ) {
    if( request->contains( "ObjectID" ) && commons::string::starts_with( request->getValue( "ObjectID" ), "image" ) ) {
        return true;
    } else return false;
}
void UpnpImageDirectory::getRootNode( commons::xml::XMLWriter * xmlWriter, commons::xml::Node * element, commons::upnp::CdsResult * cds_result ) {

    commons::xml::Node container_element = xmlWriter->element( (*element), "", "container", "" );
    xmlWriter->attribute(container_element, "id", "image");
    xmlWriter->attribute(container_element, "parentID", "0");
    xmlWriter->attribute(container_element, "restricted", "1");
    xmlWriter->attribute(container_element, "childCount", "2");

    xmlWriter->element(container_element, commons::upnp::XML_NS_PURL, "title", "Image");
    xmlWriter->element(container_element, commons::upnp::XML_NS_UPNP, "class", "object.container.storageFolder");
    xmlWriter->element(container_element, commons::upnp::XML_NS_UPNP, "storageUsed", "-1");

    cds_result->number_returned( cds_result->number_returned()+1 );
    cds_result->total_matches( cds_result->total_matches()+1 );
}
void UpnpImageDirectory::parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result,
                                          commons::upnp::UpnpContentDirectoryRequest * request ) {

    commons::xml::Node didl_element = xmlWriter->element( "DIDL-Lite" );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DIDL );
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PURL, "dc");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_DLNA, "dlna");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_UPNP, "upnp");
    xmlWriter->ns(didl_element, commons::upnp::XML_NS_PV, "pv");

    /* ----------- Root Node ----------- */

    int video_count = 0; //videoCount();
    cds_result->number_returned( video_count );
    cds_result->total_matches( video_count );


/*    } else {
        LOG4CXX_WARN(logger,"unknown request: " << request->getValue( "ObjectID" ) );
    } */
}
}}
