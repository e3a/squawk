#include "upnpcontentdirectoryimage.h"
#include "commons.h"
#include "http.h"

namespace squawk {

log4cxx::LoggerPtr UpnpContentDirectoryImage::logger( log4cxx::Logger::getLogger( "squawk.upnp.UpnpImageDirectory" ) );

bool UpnpContentDirectoryImage::match( ::upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId( request, "images" );
}
int UpnpContentDirectoryImage::getRootNode( ::didl::DidlWriter * didl_element ) {
    didl_element->container("/images/", "", didl::DidlContainer( 0, 0,"Images", "/images", 0, 0, 0 /*TODO get child count */ ) );
    return 1;
}
std::tuple<size_t, size_t> UpnpContentDirectoryImage::parseNode( didl::DidlWriter * didl_element, ::upnp::UpnpContentDirectoryRequest * request ) {
    int start_index = commons::string::parse_string<int>( request->getValue( ::upnp::START_INDEX ) );
    int request_count = commons::string::parse_string<int>( request->getValue( ::upnp::REQUESTED_COUNT ) );
    if( request_count == 0 ) request_count = 128;

    /* ----------- Artists ----------- */
    if( request->contains( ::upnp::OBJECT_ID ) &&
               request->getValue( ::upnp::OBJECT_ID ) == "images" ) {

        // cds_result->number_returned( artists(start_index, request_count, std::bind((void(ssdp::DidlElement::*)(ssdp::DidlContainerArtist))&ssdp::DidlElement::container, &didl_element, std::placeholders::_1 ) ) );
        // cds_result->total_matches( artistCount() );

    } else {
        LOG4CXX_WARN(logger,"unknown request: " << request->getValue( ::upnp::OBJECT_ID ) );
    }

}
}
