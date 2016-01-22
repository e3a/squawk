#include "upnpcontentdirectoryparser.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "upnpcontentdirectory.h"

#include "utils/image.h"
#include "media.h"
#include "utils/pdfparser.h"

namespace squawk {
log4cxx::LoggerPtr UpnpContentDirectoryParser::logger ( log4cxx::Logger::getLogger ( "squawk.UpnpContentDirectoryParser" ) );

/** \brief get the mime-type from the boost path object. */
inline http::mime::MIME_TYPE mime_type ( const boost::filesystem::path & path ) {
    std::string ext_ = path.extension().string();

    if ( ext_.length() > 0 && ext_.find ( "." ) == 0 )
    { ext_ = ext_.substr ( 1, ext_.length() -1 ); } // remove the dot

    boost::algorithm::to_lower ( ext_ );

    return http::mime::mime_type ( ext_ );
}

UpnpContentDirectoryParser::UpnpContentDirectoryParser ( http::HttpServletContext & context, ptr_upnp_dao upnp_container_dao ) :
    _upnp_container_dao ( upnp_container_dao ),
    _tmp_directory ( context.parameter ( squawk::CONFIG_TMP_DIRECTORY ) ) {

    if ( ! boost::filesystem::is_directory ( _tmp_directory + "/AlbumArtUri" ) ) {
        boost::filesystem::create_directory ( _tmp_directory + "/AlbumArtUri" );
    }
}


void UpnpContentDirectoryParser::parse ( std::vector< std::string > paths ) {
    long start_time = std::time ( 0 );
    statistic.clear();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    _upnp_container_dao->startTransaction();

    for ( auto & path : paths ) {
        if ( boost::filesystem::is_directory ( path ) ) {
            if ( squawk::DEBUG ) LOG4CXX_DEBUG ( logger, "import files:" << path )
                _parse ( 0, path );

        } else {
            LOG4CXX_WARN ( logger, path << "is not a directory." )
        }
    }

    //output statistic
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    _upnp_container_dao->sweep ( start_time );

    std::stringstream ss;
    int sum = 0;
    ss << std::endl << "***************************************************" << std::endl;

    for ( std::map< std::string, int >::iterator iter = statistic.begin(); iter != statistic.end(); ++iter ) {
        ss << iter->first << "\t" << iter->second << std::endl;
        sum += iter->second;
    }

    ss << "Total:\t" << sum << std::endl;
    ss << "Time:\t" << elapsed_seconds.count() << std::endl;
    LOG4CXX_INFO ( logger, ss.str() )

//        for( auto root_node_ : _upnp_container_dao->objects( 0, 0, 0 ) ) {
    _import_audio ( didl::DidlContainer ( 0, 0, "Root", "", 0, 0, 0 ) );
    _import_movies ( didl::DidlContainer ( 0, 0, "Root", "", 0, 0, 0 ) );
//        }

    _upnp_container_dao->endTransaction();
}

void UpnpContentDirectoryParser::_import_audio ( const didl::DidlObject object ) {
    bool audiofiles_imported_ = false;
    std::string album_name_, artist_, composer_, genre_;
    size_t year_;

    //Loop and parse the tracks in this album
    for ( auto & album_ : _upnp_container_dao->albums ( object.id(), 0, 0 ) ) {
        for ( auto & track_ : _upnp_container_dao->tracks ( album_.id(), 0, 0 ) ) {

            if ( !track_.import() ) {

                audiofiles_imported_ = true;
                //Get the track information
                commons::media::MediaFile media_file = commons::media::MediaParser::parseFile ( track_.path() );
                album_name_ = media_file.getTag ( commons::media::MediaFile::ALBUM );
                artist_ = _upnp_container_dao->artist ( media_file.getTag ( commons::media::MediaFile::ARTIST ) ).cleanName();
                composer_ = _upnp_container_dao->artist ( media_file.getTag ( commons::media::MediaFile::COMPOSER ) ).cleanName();
                genre_ = media_file.getTag ( commons::media::MediaFile::GENRE );

                year_ = ContentDirectoryModule::epoch_time( std::stoi( media_file.getTag ( commons::media::MediaFile::YEAR ) ) );

                //TODO disc_, comment_

                //Get mime-type
                boost::filesystem::path file_path_ ( track_.path() );
                std::string type_ = http::mime::mime_type( mime_type( file_path_ ) );

                //Create the audio item
                std::list< didl::DidlResource > audio_item_res;
                for ( auto & stream_ : media_file.getAudioStreams() ) {
                    audio_item_res.push_back ( didl::DidlResource ( 0, 0, "", "", std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string > ( {
                        { didl::DidlResource::bitrate, std::to_string ( stream_.bitrate() ) },
                        { didl::DidlResource::bitsPerSample, std::to_string ( stream_.bitsPerSample() ) },
                        { didl::DidlResource::nrAudioChannels, std::to_string ( stream_.channels() ) },
                        { didl::DidlResource::duration, std::to_string ( media_file.duration() ) },
                        { didl::DidlResource::sampleFrequency, std::to_string ( stream_.sampleFrequency() ) },
                        { didl::DidlResource::mimeType, type_ },
                        /* TODO { didl::dlnaProfile, stream_.dlnaProfile() } */
                    } ) ) );
                }

                //And save the track
                _upnp_container_dao->save ( didl::DidlMusicTrack (
                                                track_.id(), track_.parentId(),
                                                media_file.getTag ( commons::media::MediaFile::TITLE ),
                                                track_.path(), track_.mtime(), track_.objectUdpateId(),
                                                boost::filesystem::file_size ( file_path_ ), type_, track_.rating(), year_,
                                                std::stoi ( media_file.getTag ( commons::media::MediaFile::TRACK ) ),
                                                track_.playbackCount(), track_.contributor(), artist_, genre_, album_name_,
                                                track_.lastPlaybackTime(), audio_item_res, true ) );
            }
        } //loop tracks

        //Correct album and save image
        if ( audiofiles_imported_ ) {

            std::list< size_t > album_id_( { album_.id() } );

            //When the pathname suggests a multidisc album
            if ( UpnpContentDirectoryParser::_multidisc_name ( album_.title() ) ) {

                //Set object to album and album to container
                _upnp_container_dao->save ( didl::DidlContainer (
                    album_.id(), album_.parentId(), album_.title(), album_.path(), album_.mtime(), album_.objectUdpateId(), 0 ) );
                //set parent to album
                _upnp_container_dao->save ( didl::DidlContainerAlbum (
                    object.id(), object.parentId(), album_name_, object.path(), object.mtime(), 0, 0, 0, year_, 0, composer_,
                    artist_, genre_, std::list< didl::DidlAlbumArtUri >(), true ) );

                album_id_.push_back( object.id() );

/*TODO                for ( auto & image_ : _upnp_container_dao->photos ( album.id(), 0, 0 ) ) {
                    if ( _cover ( image_.title() ) ) {
                        _scale_image ( "JPEG_TN", image_.path(), object.id(), _tmp_directory + "/AlbumArtUri" );
                        _upnp_container_dao->save ( didl::DidlAlbumArtUri( 0, object_.id(),
                            _tmp_directory + "/AlbumArtUri/tn_" + std::to_string( album_.id() ) + ".jpg",
                            "/art/tn_" + std::to_string( album_.id() ) + ".jpg", "JPEG_TN" ) );
                    } */
            } else {
                _upnp_container_dao->save ( didl::DidlContainerAlbum (
                    album_.id(), album_.parentId(), album_name_, album_.path(), album_.mtime(), 0, 0, 0, year_, 0, composer_,
                    artist_, genre_, std::list< didl::DidlAlbumArtUri >(), true ) );
            }

            //When there were no audiofiles, continue with the remaining containers
            if( audiofiles_imported_ ) {
                for( auto id : album_id_ ) {
                    for ( auto & image_ : _upnp_container_dao->photos ( id, 0, 0 ) ) {

                        if ( _cover ( image_.title() ) ) {
                            size_t image_album_id_ = album_id_.back();
                                _upnp_container_dao->save ( didl::DidlAlbumArtUri( 0, image_album_id_,
                                _tmp_directory + "/AlbumArtUri/tn_" + std::to_string( image_album_id_ ) + ".jpg",
                                "", "JPEG_TN" ) );


                                _scale_image ( "JPEG_TN", image_.path(), image_album_id_, _tmp_directory + "/AlbumArtUri" );
                        }
                    }//loop images
                }//loop id's
            }//end import cover image
        }//audiofile imported
    }//loop albums

    //Continue with the child containers
    for ( auto & container_ : _upnp_container_dao->containers ( object.id(), 0, 0 ) ) {
        _import_audio ( container_ );
    }
}

void UpnpContentDirectoryParser::_import_movies ( const didl::DidlObject object ) {

    for ( auto & movie_ : _upnp_container_dao->movies ( object.id(), 0, 0 ) ) {
        std::cout << "+++ found video:" << movie_.id() << ":" << movie_.title() << std::endl;

        commons::media::MediaFile media_file = commons::media::MediaParser::parseFile ( movie_.path() );
        boost::filesystem::path file_path_ ( movie_.path() );
        std::string type_ = http::mime::mime_type( mime_type( file_path_ ) );

        didl::DidlMovie( 0, object.id(), movie_.title(), movie_.path(), movie_.mtime(), 0,
                         boost::filesystem::file_size ( file_path_ ), type_, true );

    }//loop videos

    //Continue with the child containers
    for ( auto & container_ : _upnp_container_dao->containers ( object.id(), 0, 0 ) ) {
        _import_movies ( container_ );
    }
}
void UpnpContentDirectoryParser::_import_images () {
//    for ( auto & image_ : _upnp_container_dao->photos( object.id(), 0, 0 ) ) {
//        std::cout << "+++ found Image:" << image_.id() << ":" << image_.title() << std::endl;
//    }
}
void UpnpContentDirectoryParser::_import_books () {

}

void UpnpContentDirectoryParser::_parse ( const unsigned long & path_id, const std::string & path ) {

    //save the directory
    boost::filesystem::path _file_path ( path );
    _upnp_container_dao->touch ( path, boost::filesystem::last_write_time ( _file_path ) );
    didl::DidlObject container = didl::DIDL_CONTAINER_EMPTY;

    bool has_audiofiles_ = false;
    boost::filesystem::directory_iterator end_itr;

    for ( boost::filesystem::directory_iterator itr ( _file_path ); itr != end_itr; ++itr ) {

        std::string filename_ = itr->path().filename().string();
        std::string stem_ = itr->path().stem().string();
        std::string filepath_ = path + "/" + filename_;

        if ( boost::filesystem::is_regular_file ( itr->status() ) ) {

            unsigned long mtime_ = boost::filesystem::last_write_time ( itr->path() );
            std::string type = http::mime::mime_type ( mime_type ( itr->path() ) );

            //Update the statistic
            statistic[type] = ( statistic.find ( type ) != statistic.end() ?
                                statistic.find ( type )->second + 1 : 1 );

            if ( _upnp_container_dao->touch ( filepath_, mtime_ ) == 0 ) {

                //Search object tyoe
                didl::DIDL_CLASS cls;

                if ( commons::string::starts_with ( type, "audio/" ) ) {
                    cls = didl::objectItemAudioItemMusicTrack;
                    has_audiofiles_ = true;

                } else if ( commons::string::starts_with ( type, "image/" ) ) {
                    cls = didl::objectItemImageItemPhoto;

                } else if ( commons::string::starts_with ( type, "video/" ) ) {
                    cls = didl::objectItemVideoItemMovie;

                } else if ( type == "application/pdf" ) {
                    // TODO cls == didl::DidlBook;
                    cls = didl::objectItem; //TODO remove

                } else {
                    cls = didl::objectItem;
                    LOG4CXX_INFO ( logger, "can not find object type for:" << type << ":" << filename_ )
                }

                if( container.id() == 0 || ( cls == didl::objectItemAudioItemMusicTrack && container.cls() != didl::objectContainerAlbumMusicAlbum ) ) {
                    didl::DIDL_CLASS container_type_ = didl::objectContainer;
                    if(  cls == didl::objectItemAudioItemMusicTrack ) {
                        container_type_ = didl::objectContainerAlbumMusicAlbum;
                    }
                    container = _upnp_container_dao->save ( didl::DidlObject ( container_type_, 0, path_id,
                                                    _file_path.filename().string(), path,
                                                    boost::filesystem::last_write_time ( _file_path ), 0, true ) );
                }

                _upnp_container_dao->save ( didl::DidlObject ( cls, 0 /*new id*/,
                                            container.id(), stem_, filepath_,
                                            mtime_, 0, false ) );
            }

        } else if ( boost::filesystem::is_directory ( itr->status() ) ) {
            _parse ( container.id(), filepath_ );

        } else {
            LOG4CXX_WARN ( logger, "path is neither a reqular file nor a directory:" << path )
        }

        if ( has_audiofiles_ )
            //Update as music album
            _upnp_container_dao->save ( didl::DidlObject ( didl::objectContainerAlbumMusicAlbum, container.id(), container.parentId(),
                                        container.title(), container.path(), container.mtime(), 0, false ) );

    }
}

//create the thumbnails
void UpnpContentDirectoryParser::_scale_image ( const std::string & profile, const std::string & path, int image_id, const std::string & target ) {
    image::Image image ( path );

    if ( profile == "JPEG_TN" ) { //TODO DLNA TYPES
        std::stringstream image_stream;
        image_stream << target << "/tn_" << image_id << ".jpg";
        image.scale ( 160, 160, image_stream.str() );

    } else if ( profile == "JPEG_SM" ) {
        std::stringstream cover_stream;
        cover_stream << target << "/sm_" << image_id << ".jpg";
        std::string cover_filename = cover_stream.str();
        image.scale ( 480, 480, cover_filename );

    } else if ( profile == "JPEG_LRG" ) {
        std::stringstream cover_stream;
        cover_stream << target << "/lrg_" << image_id << ".jpg";
        std::string cover_filename = cover_stream.str();
        image.scale ( 768, 768, cover_filename );
    }
}
}//namespace squawk
