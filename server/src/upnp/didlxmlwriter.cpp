#include "didlxmlwriter.h"

#include "fmt/format.h"
#include "fmt/time.h"

namespace didl {

DidlXmlWriter::DidlXmlWriter ( commons::xml::XMLWriter * xmlWriter ) :
    _xmlWriter ( xmlWriter ),
    _didl_element ( _xmlWriter->element ( "DIDL-Lite" ) ) {
    _xmlWriter->ns ( _didl_element, upnp::XML_NS_DIDL );
    _xmlWriter->ns ( _didl_element, upnp::XML_NS_PURL, "dc" );
    _xmlWriter->ns ( _didl_element, upnp::XML_NS_DLNA, "dlna" );
    _xmlWriter->ns ( _didl_element, upnp::XML_NS_UPNP, "upnp" );
    _xmlWriter->ns ( _didl_element, upnp::XML_NS_PV, "pv" );
}

void DidlXmlWriter::container ( const std::string & id_prefix, const std::string & parent_prefix, DidlContainer container ) {
    commons::xml::Node container_element = _xmlWriter->element ( _didl_element, "", "container", "" );
    _xmlWriter->attribute ( container_element, "id", fmt::format( id_prefix, container.id() ) );
    _xmlWriter->attribute ( container_element, "parentID", fmt::format( parent_prefix, container.parentId() ) );
    _xmlWriter->attribute ( container_element, "restricted", "1" );
    _xmlWriter->attribute ( container_element, "childCount", std::to_string ( container.childCount() ) );

    _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "title", container.title() );
    _xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "class", className ( container.cls() ) );
}
void DidlXmlWriter::container ( const std::string & id_prefix, const std::string & parent_prefix, std::list<DidlContainer> container_list ) {
    for ( auto & _container : container_list ) {
        container ( id_prefix, parent_prefix, _container );
    }
}
void DidlXmlWriter::container ( const std::string & id, const std::string & parent, const std::string & uri, DidlContainerAlbum container ) {
    commons::xml::Node container_element = _xmlWriter->element ( _didl_element, "", "container", "" );
    _xmlWriter->attribute ( container_element, "id", fmt::format( id, container.id() ) );
    _xmlWriter->attribute ( container_element, "parentID", fmt::format( parent, container.parentId() ) );
    _xmlWriter->attribute ( container_element, "restricted", "1" );
    _xmlWriter->attribute ( container_element, "childCount", std::to_string ( container.childCount() ) );

    _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "title", container.title() );
    _xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "class", className ( container.cls() ) );
    _xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "artist", container.artist() );
    _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "creator", container.contributor() );

    std::time_t time_ = container.year();
    _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "date", fmt::format("{:%Y-%m-%d}", *std::localtime( &time_ ) ) );

    commons::xml::Node dlna_album_art_node =
        _xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "albumArtURI", fmt::format( uri, container.id() ) );
    _xmlWriter->ns ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "dlna", false );
    _xmlWriter->attribute ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "profileID", "JPEG_TN" );
}
void DidlXmlWriter::container ( const std::string & id, const std::string & parent, const std::string & uri, DidlContainerPhotoAlbum container ) {
    commons::xml::Node container_element = _xmlWriter->element ( _didl_element, "", "container", "" );
    _xmlWriter->attribute ( container_element, "id", fmt::format( id, container.id() ) );
    _xmlWriter->attribute ( container_element, "parentID", fmt::format( parent, container.parentId() ) );
    _xmlWriter->attribute ( container_element, "restricted", "1" );
    _xmlWriter->attribute ( container_element, "childCount", std::to_string ( container.childCount() ) );

    _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "title", container.title() );
    _xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "class", className ( container.cls() ) );
    _xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "artist", container.artist() );
    _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "creator", container.contributor() );

    std::time_t time_ = container.year();
    _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "date", fmt::format("{:%Y-%m-%d}", *std::localtime( &time_ ) ) );

    commons::xml::Node dlna_album_art_node =
    _xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "albumArtURI", fmt::format( uri, container.id() ) );
    _xmlWriter->ns ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "dlna", false );
    _xmlWriter->attribute ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "profileID", "JPEG_TN" );
}
void DidlXmlWriter::container ( const std::string & id_prefix, const std::string & parent_prefix, DidlContainerArtist container ) {
    commons::xml::Node container_element = _xmlWriter->element ( _didl_element, "", "container", "" );
    _xmlWriter->attribute ( container_element, "id", fmt::format( id_prefix , container.id() ) );
    _xmlWriter->attribute ( container_element, "parentID", fmt::format( parent_prefix, container.parentId() ) );
    _xmlWriter->attribute ( container_element, "restricted", "1" );
    _xmlWriter->attribute ( container_element, "childCount", std::to_string ( container.childCount() ) );

    _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "title", container.title() );
    _xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "class", className ( container.cls() ) );
}
void DidlXmlWriter::write ( const std::string & id_prefix, const std::string & parent_prefix, const std::string & uri, DidlItem item ) {
    commons::xml::Node item_element = _xmlWriter->element ( _didl_element, "", "item", "" );
    _xmlWriter->attribute ( item_element, "id", fmt::format( id_prefix, item.id() ) );
    _xmlWriter->attribute ( item_element, "parentID", fmt::format( parent_prefix, item.parentId() ) );
    _xmlWriter->attribute ( item_element, "restricted", "1" );
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "class", className ( item.cls() ) );
    _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "title",item.title() );

    for ( auto & track : item.audioItemRes() ) {
        write ( track, item_element, uri );
    }
}
void DidlXmlWriter::write ( const std::string & id_prefix, const std::string & parent_prefix, const std::string & uri_res, const std::string & uri_album_art, DidlMusicTrack item ) {
    commons::xml::Node item_element = _xmlWriter->element ( _didl_element, "", "item", "" );
    _xmlWriter->attribute ( item_element, "id", fmt::format( id_prefix, item.id() ) );
    _xmlWriter->attribute ( item_element, "parentID", fmt::format( parent_prefix, item.parentId() ) );
    _xmlWriter->attribute ( item_element, "restricted", "1" );
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "class", className ( item.cls() ) );
    _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "title",item.title() );

    _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "originalTrackNumber", std::to_string ( item.track() ) );
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "album", item.album() );
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "description", item.comment() );

    std::time_t time_ = item.year();
    _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "date", fmt::format("{:%Y-%m-%d}", *std::localtime( &time_ ) ) );
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "genre", item.genre() );

    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "artist", item.artist() );
    _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "contributor", item.contributor() );

    std::time_t last_playback_time_ = item.lastPlaybackTime();
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "lastPlaybackTime", fmt::format("{:%Y-%m-%d %h:%m:%s}", *std::localtime( &last_playback_time_ ) ) );
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "playbackCount", std::to_string( item.playbackCount() ) );
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "rating", std::to_string( item.rating() ) );

    for ( auto & track : item.audioItemRes() ) {
        write ( track, item_element, uri_res );
    }
    commons::xml::Node dlna_album_art_node =
        _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "albumArtURI", fmt::format( uri_album_art, item.parentId() ) );
    _xmlWriter->ns ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "dlna", false );
    _xmlWriter->attribute ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "profileID", "JPEG_TN" );
}
void DidlXmlWriter::write ( const std::string & id_prefix, const std::string & parent_prefix, const std::string & uri, DidlPhoto item ) {

    commons::xml::Node item_element = _xmlWriter->element ( _didl_element, "", "item", "" );
    _xmlWriter->attribute ( item_element, "id", fmt::format( id_prefix, item.id() ) );
    _xmlWriter->attribute ( item_element, "parentID", fmt::format( parent_prefix, item.parentId() ) );
    _xmlWriter->attribute ( item_element, "restricted", "1" );
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "class", className ( item.cls() ) );
    _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "title",item.title() );

    for( auto & track : item.audioItemRes() ) {
        write( track, item_element, uri );
    }
}
void DidlXmlWriter::write ( const std::string & id_prefix, const std::string & parent_prefix, const std::string & uri, DidlMovie item ) {
    commons::xml::Node item_element = _xmlWriter->element ( _didl_element, "", "item", "" );
    _xmlWriter->attribute ( item_element, "id", fmt::format( id_prefix, item.id() ) );
    _xmlWriter->attribute ( item_element, "parentID", fmt::format( parent_prefix, item.parentId() ) );
    _xmlWriter->attribute ( item_element, "restricted", "1" );
    _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "class", className ( item.cls() ) );
    _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "title",item.title() );

    std::time_t time_ = item._year;
    _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "date", fmt::format("{:%Y-%m-%d}", *std::localtime( &time_ ) ) );

    for( auto & track : item.audioItemRes() ) {
        write( track, item_element, uri );
    }
}
void DidlXmlWriter::write ( const DidlResource & item, commons::xml::Node & item_element, const std::string & res_uri ) {

    commons::xml::Node dlna_res_node = _xmlWriter->element ( item_element, "", "res",
        fmt::format( res_uri, item.id(), http::mime::extension( item.mimeType() ).c_str() ) );
    _xmlWriter->attribute ( dlna_res_node, "", "protocolInfo",
                            "http-get:*:" + item.mimeType() + ":DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000" );


    _xmlWriter->attribute ( dlna_res_node, "", "size", std::to_string( item.size() ) );
    _xmlWriter->attribute ( dlna_res_node, "", "dlnaProfile", item.dlnaProfile() );
    _xmlWriter->attribute ( dlna_res_node, "", "mime-type", item.mimeType() );

    for ( auto & attr : item.attributes() ) {
        if( ! attr.second.empty() ) {
            switch ( attr.first ) {
            case DidlResource::duration:
                _xmlWriter->attribute ( dlna_res_node, "", "duration", attr.second ); break;
            case DidlResource::bitrate:
                _xmlWriter->attribute ( dlna_res_node, "", "bitrate", attr.second ); break;
            case DidlResource::bitsPerSample:
                _xmlWriter->attribute ( dlna_res_node, "", "bitsPerSample", attr.second ); break;
            case DidlResource::sampleFrequency:
                _xmlWriter->attribute ( dlna_res_node, "", "sampleFrequency", attr.second ); break;
            case DidlResource::nrAudioChannels:
                _xmlWriter->attribute ( dlna_res_node, "", "nrAudioChannels", attr.second ); break;
            case DidlResource::resolution:
                _xmlWriter->attribute ( dlna_res_node, "", "resolution", attr.second ); break;
            case DidlResource::colorDepth:
                _xmlWriter->attribute ( dlna_res_node, "", "colorDepth", attr.second ); break;
            case DidlResource::framerate:
                _xmlWriter->attribute ( dlna_res_node, "", "framerate", attr.second ); break;
            }
        }
    }
}
}//didl

