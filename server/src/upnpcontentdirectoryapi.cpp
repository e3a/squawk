#include "upnpcontentdirectoryapi.h"

namespace squawk {

log4cxx::LoggerPtr UpnpContentDirectoryApi::logger ( log4cxx::Logger::getLogger ( "squawk.UpnpContentDirectoryApi" ) );

void UpnpContentDirectoryApi::do_get ( http::HttpRequest & request, http::HttpResponse & response ) {

    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "API HTTP Request:" << request ); }

    std::string command = "";
    int id = 0, page = 0, limit = 100;
    std::list< std::string > attributes_;
    std::map< std::string, std::string > filters_;
    std::pair< std::string, std::string > sort_ {{"ROWID"}, {"asc"}};

    if ( request.containsAttribute ( "page" ) )
    { page = boost::lexical_cast<int> ( request.attribute ( "page" ) ); }

    if ( request.containsAttribute ( "limit" ) )
    { limit = boost::lexical_cast<int> ( request.attribute ( "limit" ) ); }

    if ( request.containsAttribute ( "filters" ) )
    { filters_ = parse_filters ( request.attribute ( "filters" ) ); }

    if ( request.containsAttribute ( "attributes" ) )
    { attributes_ = parse_attributes ( request.attribute ( "attributes" ) ); }

    if ( request.containsAttribute ( "sort" ) )
    { sort_ = parse_sort ( request.attribute ( "sort" ) ); }

    if ( match ( request.uri(), &command, &id ) ) {
        if ( squawk::SUAWK_SERVER_DEBUG ) {
            LOG4CXX_TRACE ( logger, "API Request:" << command << ": id:" << id );
        }

        if ( command == "artist" ) {

        } else if ( command == "album" ) {
            didl::DidlContainerAlbum album_ = _dao->object<didl::DidlContainerAlbum> ( id );
            didl::serializer< didl::DidlContainerAlbum >::serialize ( response, album_, attributes_ );

        } else if ( command == "video" ) {

        } else if ( command == "image" ) {

        } else if ( command == "track" ) {
            std::list< didl::DidlMusicTrack > track_list_ = _dao->children<didl::DidlMusicTrack> ( id, 0, 200 );
            didl::serializer< std::list< didl::DidlMusicTrack > >::serialize ( response, track_list_, attributes_ );

        } else if ( command == "browse" ) {
            response << "{\"objects_count\": " << _dao->childrenCount( didl::object, id ) << ",\"objects\": ";
            std::list< didl::DidlObject > objects_ = _dao->children<didl::DidlObject > ( id, page, limit );
            didl::serializer< std::list< didl::DidlObject > >::serialize ( response, objects_, attributes_ );
            response << "}";
        }

    } else if ( match ( request.uri(), &command ) ) {
        try  {
            if ( squawk::SUAWK_SERVER_DEBUG ) {
                std::stringstream str_log;
                str_log << ", Filters: { ";
                bool first = true;

                for ( auto item : filters_ ) {
                    if ( first ) { first = false; }

                    else { str_log << ", "; }

                    str_log << item.first << "=" << item.second;

                }

                str_log << " }, Attributes: { ";
                first = true;

                for ( auto item : attributes_ ) {
                    if ( first ) { first = false; }

                    else { str_log << ", "; }

                    str_log << item;

                }

                str_log << " }, Sort: { " << sort_.first << "=" << sort_.second;
                LOG4CXX_DEBUG ( logger, "API Request:" << command << ": Page:" << page << ", Limit:" << limit << str_log.str() << " }" );
            }

            if ( command == "artist" ) {
                std::list< didl::DidlContainerArtist > artists_ = _dao->artists ( page, limit, filters_, sort_ );
                response << "{artists_count=100, artists=";
                didl::serializer< std::list< didl::DidlContainerArtist > >::serialize ( response, artists_, attributes_ );
                response << "}";

            } else if ( command == "album" ) {
                std::list< didl::DidlContainerAlbum > albums_ = _dao->objects<didl::DidlContainerAlbum> ( page, limit, filters_, sort_ );
                response << "{\"albums_count\": " << _dao->objectsCount ( didl::objectContainerAlbumMusicAlbum, filters_ ) << ",\"albums\": ";
                didl::serializer< std::list< didl::DidlContainerAlbum > >::serialize ( response, albums_, attributes_ );
                response << "}";

            } else if ( command == "video" ) {

            } else if ( command == "image" ) {

            } else if ( command == "statistic" ) {
                didl::DidlStatistics statistic_ = _dao->statistics();
                didl::serializer< didl::DidlStatistics >::serialize ( response, statistic_, attributes_ );

            } else if ( command == "upnp/device" ) {
                std::map< std::string, upnp::UpnpDevice > devices = SquawkServer::instance()->upnp_devices();

                response << "[";
                bool first_device = true;

                for ( auto & event : devices ) {
                    try {
                        upnp::UpnpDevice & device = event.second;

                        if ( first_device ) { first_device = false; }

                        else { response << ","; }

                        response << "{";

                        response << "\"versionMajor\":\"" << std::to_string ( device.versionMajor() ) << "\",";
                        response << "\"versionMinor\":\"" << std::to_string ( device.versionMinor() ) << "\",";
                        response << "\"deviceType\":\"" << device.deviceType() << "\",";
                        response << "\"friendlyName\":\"" << device.friendlyName() << "\",";
                        response << "\"manufacturer\":\"" << device.manufacturer() << "\",";
                        response << "\"manufacturerUrl\":\"" << device.manufacturerUrl() << "\",";
                        response << "\"modelDescription\":\"" << device.modelDescription() << "\",";
                        response << "\"modelName\":\"" << device.modelName() << "\",";
                        response << "\"modelNumber\":\"" << device.modelNumber() << "\",";
                        response << "\"modelUrl\":\"" << device.modelUrl() << "\",";
                        response << "\"serialNumber\":\"" << device.serialNumber() << "\",";
                        response << "\"udn\":\"" << device.udn() << "\",";
                        response << "\"upc\":\"" << device.upc() << "\",";
                        response << "\"presentationUrl\":\"" << device.presentationUrl() << "\",";

                        response << "\"icons\":[";
                        bool first_icon = true;

                        for ( auto & icon : device.iconList() ) {
                            if ( first_icon ) { first_icon = false; }

                            else { response << ","; }

                            response << "{\"width\":" << std::to_string ( icon.width() );
                            response << ",\"height\":" << std::to_string ( icon.height() );
                            response << ",\"depth\":" << std::to_string ( icon.depth() );
                            response << ",\"mimeType\":\"" << icon.mimeType();
                            response << "\",\"url\":\"" << icon.url() << "\"}";
                        }

                        response << "], \"services\":[";
                        bool first_service = true;

                        for ( auto & service : device.serviceList() ) {
                            if ( first_service ) { first_service = false; }

                            else { response << ","; }

                            response << "{\"serviceType\":\"" << service.serviceType();
                            response << "\",\"serviceId\":\"" << service.serviceId();
                            response << "\",\"scpdUrl\":\"" << service.scpdUrl();
                            response << "\",\"controlUrl\":\"" << service.controlUrl();
                            response << "\",\"eventSubUrl\":\"" << service.eventSubUrl() << "\"}";
                        }

                        response << "]}";

                    } catch ( commons::xml::XmlException & ex ) {
                        std::cerr << "can not parse xml file: " << ex.what() << std::endl;

                    } catch ( ... ) {
                        std::cerr << "other exception in get description." << std::endl;
                    }
                }

                response << "]";
            }

        } catch ( db::DbException & e ) {
            std::stringstream ss;
            ss << "Can not get album by id, Exception:" << e.code() << "-> " << e.what();
            throw std::system_error( std::make_error_code( std::errc::io_error ), ss.str() );
        }

    } else {
        throw http::http_status::BAD_REQUEST;
    }

    response.set_mime_type ( http::mime::JSON );
    response.status ( http::http_status::OK );
}

std::list< std::string > UpnpContentDirectoryApi::parse_attributes ( const std::string & str ) {
    std::list< std::string > result;

    if ( str.length() == 0 )
    { return result; }

    boost::split ( result, str, boost::is_any_of ( "," ) );
    return result;
}

std::map< std::string, std::string > UpnpContentDirectoryApi::parse_filters ( const std::string & str ) {
    std::map< std::string, std::string > result;

    if ( str.length() == 0 )
    { return result; }

    size_t start_ = 0, end_ = str.find ( "," );

    while ( start_ < str.length() ) {
        std::string token_ = str.substr ( start_, ( end_==string::npos?str.length() :end_ ) );

        if ( token_.find ( "=" ) != string::npos ) {
            result[token_.substr ( 0, token_.find ( "=" ) )] = token_.substr ( token_.find ( "=" )+1 );
        }

        start_ = end_ + 1;
        end_ = ( str.find ( ",", start_ ) ==string::npos?str.length() :str.find ( ",", start_ ) );
    }

    return result;
}

std::pair< std::string, std::string > UpnpContentDirectoryApi::parse_sort ( const std::string & str ) {
    std::string name_ = "";
    std::string order_ = "asc";

    if ( str.length() == 0 )
    { return std::make_pair ( "", "" ); }

    if ( str.find ( "," ) == string::npos ) {
        name_ = str;

    } else {
        name_ = str.substr ( 0, str.find ( "," ) );
        order_ = str.substr ( str.find ( "," ) + 1 );
    }

    return std::make_pair ( name_, order_ );
}
}//namespace squawk
