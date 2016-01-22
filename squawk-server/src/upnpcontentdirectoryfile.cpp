#include "upnpcontentdirectoryfile.h"

namespace squawk {

log4cxx::LoggerPtr UpnpContentDirectoryFile::logger( log4cxx::Logger::getLogger( "squawk.UpnpContentDirectoryFile" ) );

bool UpnpContentDirectoryFile::match( upnp::UpnpContentDirectoryRequest * request ) {
    return ContentDirectoryModule::matchObjectId( request, "/file/" );
}
int UpnpContentDirectoryFile::getRootNode( ::didl::DidlWriter * didl_element ) {
    didl_element->container("/file/", "", didl::DidlContainer( 0, 0, "Filesystem", "", 0, 0, _upnp_cds_dao->objectsCount( didl::object, 0 ) ) );
    return 1;
}
std::tuple<size_t, size_t> UpnpContentDirectoryFile::parseNode( didl::DidlWriter * didl_element, upnp::UpnpContentDirectoryRequest * request ) {

    size_t start_index_ = std::stoi( request->getValue( upnp::START_INDEX ) );
    size_t request_count_ = std::stoi( request->getValue( upnp::REQUESTED_COUNT ) );
    if( request_count_ == 0 ) request_count_ = 128;
    size_t returned_ = 0, total = 0;
    int parent_id_ = ContentDirectoryModule::item_id( request );

    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "Request File: parent=" << parent_id_ << ", start_index=" << start_index_ << ", request_count=" << request_count_ );

    try {
        std::list< didl::DidlObject > item_list =
            _upnp_cds_dao->objects( parent_id_, start_index_, request_count_ );

        for( auto & item : item_list ) {
            returned_ ++;
            switch( item.cls() ) {
            case didl::objectContainer: { didl_element->container( "/file/", "/file/", _upnp_cds_dao->container( item.id() ) ); break; }
            case didl::objectItemImageItemPhoto: { didl_element->write( "/file/", "/file/", _upnp_cds_dao->photo( item.id() ) ); break; }
            case didl::objectItemAudioItemMusicTrack: {didl_element->write( "/file/", "/file/", "http://", _upnp_cds_dao->track( item.id() ) ); break; } //TODO uri
            case didl::objectItemVideoItemMovie: { didl_element->write( "/file/", "/file/", _upnp_cds_dao->movie( item.id() ) ); break; }
            }
        }
    } catch( db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not get files, Exception:" << e.code() << "-> " << e.what());
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get file.");
    }
    return( std::tuple< size_t, size_t >( returned_, total ) );
}
}//squawk
