#include "upnpcontentdirectoryfile.h"

#include "squawkserver.h"

namespace squawk {

log4cxx::LoggerPtr UpnpContentDirectoryFile::logger( log4cxx::Logger::getLogger( "squawk.UpnpContentDirectoryFile" ) );

bool UpnpContentDirectoryFile::match( upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId( request, "/file/" );
}
int UpnpContentDirectoryFile::getRootNode( ::didl::DidlXmlWriter * didl_element ) {
    didl_element->container( "/file/", "", didl::DidlContainer( 0, 0, "Filesystem", "", 0, 0, SquawkServer::instance()->dao()->childrenCount(didl::object, 0 ) ) );
    return 1;
}
std::tuple<size_t, size_t> UpnpContentDirectoryFile::parseNode( didl::DidlXmlWriter * didl_element, upnp::UpnpContentDirectoryRequest * request ) {

    size_t start_index_ = std::stoi( request->getValue( upnp::START_INDEX ) );
    size_t request_count_ = std::stoi( request->getValue( upnp::REQUESTED_COUNT ) );
    if( request_count_ == 0 ) request_count_ = 128;
    int parent_id_ = ContentDirectoryModule::item_id( request );
    std::tuple<size_t, size_t> res_;

    std::list< didl::DidlObject > item_list_ =
        SquawkServer::instance()->dao()->children<didl::DidlObject>( parent_id_, start_index_, request_count_ );

    for( auto & item__ : item_list_ ) {
        switch( item__.cls() ) {
            case didl::objectContainer: { didl_element->container( "/file/{}", "/file/{}", SquawkServer::instance()->dao()->object<didl::DidlContainer>( item__.id() ) ); break; }
            case didl::objectContainerAlbumMusicAlbum: { didl_element->container( "/file/{}", "/file/{}", SquawkServer::instance()->dao()->object<didl::DidlContainerAlbum>( item__.id() ) ); break; }
            case didl::objectContainerAlbumPhotoAlbum: { didl_element->container( "/file/{}", "/file/{}", SquawkServer::instance()->dao()->object<didl::DidlContainerPhotoAlbum>( item__.id() ) ); break; }
            case didl::objectItem: {didl_element->write( "/file/{}", "/file/{}", "resource/{0}.{1}", SquawkServer::instance()->dao()->object<didl::DidlItem>( item__.id() ) ); break; } //TODO uri
            case didl::objectItemImageItemPhoto: { didl_element->write( "/file/{}", "/file/{}", http_uri( "resource/{0}.{1}" ), SquawkServer::instance()->dao()->object<didl::DidlPhoto>( item__.id() ) ); break; }
            case didl::objectItemAudioItemMusicTrack: {didl_element->write( "/file/{}", "/file/{}", http_uri( "resource/{0}.{1}" ), SquawkServer::instance()->dao()->object<didl::DidlMusicTrack>( item__.id() ) ); break; }
            //case didl::objectItemVideoItemMovie: { didl_element->write( "/file/", "/file/", SquawkServer::instance()->dao()->object<didl::DidlMovie( item.id() ) ); break; } //TODO
            default: break;
        }
    }
    return std::tuple<size_t, size_t>( item_list_.size(), SquawkServer::instance()->dao()->childrenCount( didl::object, parent_id_ ) );
}
}// namespace squawk
