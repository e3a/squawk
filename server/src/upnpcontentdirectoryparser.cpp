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
#include "upnpcontentdirectoryparser.h"

#include "utils/media.h"
#include "utils/image.h"
#include "utils/pdfparser.h"
#include "squawkserver.h"
#include "upnpcontentdirectorydao.h"

namespace squawk {
log4cxx::LoggerPtr UpnpContentDirectoryParser::logger ( log4cxx::Logger::getLogger ( "squawk.UpnpContentDirectoryParser" ) );

UpnpContentDirectoryParser::UpnpContentDirectoryParser () {
    if ( ! boost::filesystem::is_directory ( SquawkServer::instance()->config()->tmpDirectory() + "/AlbumArtUri" ) ) {
        boost::filesystem::create_directory ( SquawkServer::instance()->config()->tmpDirectory() + "/AlbumArtUri" );
    }

    if ( ! boost::filesystem::is_directory ( SquawkServer::instance()->config()->tmpDirectory() + "/Resource" ) ) {
        boost::filesystem::create_directory ( SquawkServer::instance()->config()->tmpDirectory() + "/Resource" );
    }
}

void UpnpContentDirectoryParser::parse ( std::list< std::string > paths ) {
    long start_time = std::time ( 0 );
    statistic.clear();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    SquawkServer::instance()->dao()->startTransaction();

    for ( auto & path : paths ) {
        if ( boost::filesystem::is_directory ( path ) ) {
            didl::DidlContainer parent_ ( 0, 0, "ROOT", "/", 0, 0, 0, true );
            _parse ( parent_, path );

        } else {
            LOG4CXX_WARN ( logger, path << "is not a directory." )
        }
    }

    SquawkServer::instance()->dao()->sweep ( start_time );

    SquawkServer::instance()->dao()->endTransaction();

//    SquawkServer::instance()->dao()->startTransaction();

//    _import_audio( didl::DidlContainer ( 0, 0, "Root", "", 0, 0, 0 ) );
//    _import_movies( didl::DidlContainer ( 0, 0, "Root", "", 0, 0, 0 ) );
//    _import_images( didl::DidlContainer ( 0, 0, "Root", "", 0, 0, 0 ) );

//    SquawkServer::instance()->dao()->endTransaction();

    //output statistic
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

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
}

void UpnpContentDirectoryParser::_import_audio ( const didl::DidlObject object ) {
    if ( squawk::SUAWK_SERVER_DEBUG ) LOG4CXX_DEBUG ( logger, "import audio:" << object )

    }

didl::DidlMusicTrack inline UpnpContentDirectoryParser::_import_track ( const didl::DidlItem & track  ) {
    //Get the track information
    commons::media::MediaFile media_file = commons::media::MediaParser::parseFile ( track.path() );
    std::string album_name_ = media_file.getTag ( commons::media::MediaFile::ALBUM );
    std::string artist_ = "";

    if ( ! media_file.getTag ( commons::media::MediaFile::ARTIST ).empty() ) {
        artist_ = SquawkServer::instance()->dao()->artist ( media_file.getTag ( commons::media::MediaFile::ARTIST ) ).cleanName();
    }

    std::string composer_ = "";

    if ( ! media_file.getTag ( commons::media::MediaFile::COMPOSER ).empty() ) {
        composer_ = SquawkServer::instance()->dao()->artist ( media_file.getTag ( commons::media::MediaFile::COMPOSER ) ).cleanName();
    }

    std::string genre_ = media_file.getTag ( commons::media::MediaFile::GENRE );
    unsigned long year_;

    std::string str_year_ = media_file.getTag ( commons::media::MediaFile::YEAR );

    if ( !str_year_.empty() && std::all_of ( str_year_.begin(), str_year_.end(), ::isdigit ) ) { //when str_year is number
        year_ = ContentDirectoryModule::epoch_time ( std::stoi ( str_year_ ) );
    }

    size_t track_number_ = 0;
    std::string str_track_ = media_file.getTag ( commons::media::MediaFile::TRACK );

    if ( !str_track_.empty() && std::all_of ( str_track_.begin(), str_track_.end(), ::isdigit ) ) { //when is number
        track_number_ = std::stoi ( str_track_ );
    }

    size_t disc_ = 0;
    std::string str_disc_ = media_file.getTag ( commons::media::MediaFile::DISC );

    if ( !str_disc_.empty() && std::all_of ( str_disc_.begin(), str_disc_.end(), ::isdigit ) ) { //when is number
        disc_ = std::stoi ( str_disc_ );
    }

    std::string comment_ = media_file.getTag ( commons::media::MediaFile::COMMENT );

    //Create the audio item
    std::list< didl::DidlResource > audio_item_res;

    for ( auto & stream_ : media_file.getAudioStreams() ) {
        audio_item_res.push_back ( didl::DidlResource ( 0, 0, track.size(), "" /*uri*/, track.path(), "protocolInfo", "dlnaProfile", track.mimeType(),
        std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string > ( {
            { didl::DidlResource::bitrate, std::to_string ( stream_.bitrate() ) },
            { didl::DidlResource::bitsPerSample, std::to_string ( stream_.bitsPerSample() ) },
            { didl::DidlResource::nrAudioChannels, std::to_string ( stream_.channels() ) },
            { didl::DidlResource::duration, std::to_string ( media_file.duration() ) },
            { didl::DidlResource::sampleFrequency, std::to_string ( stream_.sampleFrequency() ) }
        } ) ) );
    }

    //save the track and return new object
    return SquawkServer::instance()->dao()->save ( didl::DidlMusicTrack (
                track.id(), track.parentId(),
                media_file.getTag ( commons::media::MediaFile::TITLE ),
                track.path(), track.mtime(), track.objectUdpateId(),
                boost::filesystem::file_size ( track.path() ), track.mimeType(),
                audio_item_res, 0 /*rating*/, year_, track_number_, disc_,
                0 /*playbackCount*/, composer_, artist_, genre_,
                album_name_, comment_, 0 /*lastPlaybackTime*/, false ) );
}

void UpnpContentDirectoryParser::_import_movies ( const didl::DidlObject object ) {
//    if(squawk::DEBUG ) LOG4CXX_DEBUG( logger, "import movies" )

//    for ( auto & movie_ : SquawkServer::instance()->dao()->children<didl::DidlMovie>( object.id(), 0, 0 ) ) {
//        if( ! movie_.import() ) {
//            std::cout << "+++ found video:" << movie_.id() << ":" << movie_.title() << std::endl;

//            commons::media::MediaFile media_file = commons::media::MediaParser::parseFile ( movie_.path() );
//            boost::filesystem::path file_path_ ( movie_.path() );

//            SquawkServer::instance()->dao()->save(
//                didl::DidlMovie( object.id(), object.parentId(), movie_.title(), movie_.path(), movie_.mtime(), 0,
//                                 boost::filesystem::file_size ( file_path_ ), movie_.mimeType(), std::list<didl::DidlResource>(), true )
//                );
//        }
//    }//loop videos

//    //Continue with the child containers
//    for ( auto & container_ : SquawkServer::instance()->dao()->children<didl::DidlContainer> ( object.id(), 0, 0 ) ) {
//        _import_movies ( container_ );
//    }
}

void UpnpContentDirectoryParser::_import_movie ( const didl::DidlItem & movie ) {
    if ( squawk::SUAWK_SERVER_DEBUG ) LOG4CXX_DEBUG ( logger, "import movies" )

        commons::media::MediaFile media_file = commons::media::MediaParser::parseFile ( movie.path() );

    boost::filesystem::path file_path_ ( movie.path() );

    std::list< didl::DidlResource > resources_;

    for ( auto stream_ : media_file.getVideoStreams() ) {
        resources_.push_back ( didl::DidlResource ( 0, movie.id(), movie.size(), "", movie.path(), "", "", movie.mimeType(), std::map<didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string> ( {
            { didl::DidlResource::resolution, std::to_string ( stream_.width() ) + "x" + std::to_string ( stream_.height() ) },
            { didl::DidlResource::bitrate, std::to_string ( stream_.bitrate() ) }
            /* { didl::DidlResource::colorDepth, "" },
            { didl::DidlResource::duration stream_. } */
        } ) ) );
    }

    int season_, episode_;
    std::string title_;
    _parse_series ( movie.title(), &season_, &episode_, &title_ );

    SquawkServer::instance()->dao()->save (
        didl::DidlMovie ( movie.id(), movie.parentId(), movie.title(), movie.path(), movie.mtime(), 0,
                          boost::filesystem::file_size ( file_path_ ), movie.mimeType(), resources_, 0, season_, episode_, title_, true )
    );
}

void UpnpContentDirectoryParser::_import_images ( const didl::DidlObject /* object */ ) {
//    if(squawk::DEBUG ) LOG4CXX_TRACE( logger, "import_images:" << object )
//    for ( auto & image_ : SquawkServer::instance()->dao()->children<didl::DidlPhoto>( object.id(), 0, 0 ) ) {
//        if ( !image_.import() ) {
//            if(squawk::DEBUG ) LOG4CXX_TRACE( logger, "import_image:" << image_ )
//            std::list<didl::DidlResource> resources_;
//            std::string image_meta_resolution, image_meta_mime_type_, image_meta_color_depth_;
//            {
//                image::Image image_meta_( image_.path() );

//                std::stringstream s_resolution_;
//                s_resolution_ << image_meta_.width() << "x" << image_meta_.height();
//                image_meta_resolution = s_resolution_.str();

//                image_meta_mime_type_ = image_meta_.mimeType();
//                image_meta_color_depth_ = image_meta_.colorDepth();
//            }
//            _scale_image ( "JPEG_TN", image_.path(), image_.id(), SquawkServer::instance()->config()->tmpDirectory() + "/Resource" );
//            resources_.push_back ( didl::DidlResource ( 0, 0, "", "", std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string > ( {
//                { didl::DidlResource::resolution, image_meta_resolution },
//                { didl::DidlResource::mimeType, image_meta_mime_type_ },
//                { didl::DidlResource::colorDepth, image_meta_color_depth_ },
//                { didl::DidlResource::size, std::to_string( boost::filesystem::file_size( SquawkServer::instance()->config()->tmpDirectory() + "/Resource/tn_" + std::to_string( image_.id() ) + ".jpg" ) ) },
//                { didl::DidlResource::dlnaProfile, "JPEG_TN" }
//            })));
//            SquawkServer::instance()->dao()->save(
//                        didl::DidlPhoto( image_.id(), image_.parentId(), image_.title(), image_.path(), image_.mtime(),
//                                         image_.objectUdpateId(), image_.size(), image_.mimeType(), resources_, true ) );
//        }
//    }

//    //Continue with the child containers
//    for ( auto & container_ : SquawkServer::instance()->dao()->children<didl::DidlContainer> ( object.id(), 0, 0 ) ) {
//        _import_images( container_ );
//    }
}

void UpnpContentDirectoryParser::_import_photo ( const didl::DidlItem & photo ) {
    if ( squawk::SUAWK_SERVER_DEBUG ) LOG4CXX_TRACE ( logger, "import_image:" << photo )

        std::list<didl::DidlResource> resources_;

    std::hash<std::string> hash_fn;
    image::Image image_meta_ ( photo.path() );

    std::stringstream s_resolution_;
    s_resolution_ << image_meta_.width() << "x" << image_meta_.height();
    http::mime::MIME_TYPE mime_type = http::mime::mime_type ( image_meta_.mimeType() );

    //create a resized preview when the photo is to large
    if ( image_meta_.width() > 768 || image_meta_.height() > 768 ) {
        std::stringstream cover_filename_hash_;
        cover_filename_hash_ << SquawkServer::instance()->config()->tmpDirectory() + "/Resource/lrg_" << hash_fn ( photo.path() ) << ".jpg";
        image_meta_.scale ( 768, 768, cover_filename_hash_.str() );
        resources_.push_back ( didl::DidlResource (
                                   0, photo.id(),
                                   boost::filesystem::file_size ( cover_filename_hash_.str() ) /*size*/, "", cover_filename_hash_.str(),
                                   "" /*protocolInfo*/, "JPEG_LRG", http::mime::mime_type ( http::mime::JPEG ),
        std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string > ( {
            { didl::DidlResource::resolution, "768x768" },
            { didl::DidlResource::colorDepth, image_meta_.colorDepth() }
        } )
                               ) );
    }

    //create a resized preview thumbnail
    std::stringstream cover_filename_hash_;
    cover_filename_hash_ << SquawkServer::instance()->config()->tmpDirectory() + "/Resource/tn_" << hash_fn ( photo.path() ) << ".jpg";
    image_meta_.scale ( 160, 160, cover_filename_hash_.str() );
    resources_.push_back ( didl::DidlResource (
                               0, photo.id(),
                               boost::filesystem::file_size ( cover_filename_hash_.str() ) /*size*/, "", cover_filename_hash_.str(),
                               "" /*protocolInfo*/, "JPEG_TN", http::mime::mime_type ( http::mime::JPEG ),
    std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string > ( {
        { didl::DidlResource::resolution, "160x160" },
        { didl::DidlResource::colorDepth, image_meta_.colorDepth() }
    } ) )
                         );
    //save the original resource
    resources_.push_back ( didl::DidlResource (
                               0, photo.id(),
                               boost::filesystem::file_size ( photo.path() ) /*size*/, "", photo.path(),
                               "" /*protocolInfo*/, "XXX_" + s_resolution_.str(), http::mime::mime_type ( mime_type ),
    std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string > ( {
        { didl::DidlResource::resolution, s_resolution_.str() },
        { didl::DidlResource::colorDepth, image_meta_.colorDepth() }
    } ) )
                         );

    SquawkServer::instance()->dao()->save (
        didl::DidlPhoto ( photo.id(), photo.parentId(), photo.title(), photo.path(), photo.mtime(),
                          photo.objectUdpateId(), photo.size(), photo.mimeType(), resources_, true ) );
}

void UpnpContentDirectoryParser::_import_books () {
    if ( squawk::SUAWK_SERVER_DEBUG ) LOG4CXX_DEBUG ( logger, "import books" )


    std::list< didl::DidlEBook > book_list_;
}

void UpnpContentDirectoryParser::_import_ebook ( const didl::DidlItem & ebook ) {
    if ( squawk::SUAWK_SERVER_DEBUG ) LOG4CXX_DEBUG ( logger, "import:" << ebook )

        std::list<didl::DidlResource> resources_;

    resources_.push_back ( didl::DidlResource (
                               0, ebook.id(),
                               boost::filesystem::file_size ( ebook.path() ) /*size*/, "", ebook.path(),
                               "" /*protocolInfo*/, "EBOOK", ebook.mimeType(), //TODO
                               std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string > () )
                         );

    std::string  isbn_ = PdfParser::parsePdf ( ebook.path() );

    if ( squawk::SUAWK_SERVER_DEBUG ) LOG4CXX_DEBUG ( logger, "import (isbn):" << isbn_ )
    SquawkServer::instance()->dao()->save (
        didl::DidlEBook ( ebook.id(), ebook.parentId(), ebook.title(), ebook.path(), ebook.mtime(),
                          ebook.objectUdpateId(), ebook.size(), ebook.mimeType(), resources_, isbn_, true ) );
}

UpnpContentDirectoryParser::DIDL_PARSE_TYPES UpnpContentDirectoryParser::_parse ( didl::DidlContainer & parent, const std::string & path ) {

    boost::filesystem::path _fs_path ( path );
    std::string container_stem_ = _fs_path.stem().string();

    //save the directory
    didl::DidlContainer container_ = SquawkServer::instance()->dao()->object<didl::DidlContainer> ( path );

    if ( container_.id() == 0 ) {
        container_ = SquawkServer::instance()->dao()->save ( didl::DidlContainer ( 0, parent.id(), _fs_path.stem().string(), path, 0, 0, 0, true ) );
    }

    bool parent_changed_ = false;
    DIDL_PARSE_TYPES container_type_ = DIDL_PARSE_TYPES::container;

    didl::DidlContainerAlbum album_;
    didl::DidlContainerPhotoAlbum photo_album_;
    std::list< didl::DidlAlbumArtUri > albumArtUri_;

    std::list< std::string > child_directories_;

    if ( squawk::SUAWK_SERVER_DEBUG ) LOG4CXX_TRACE ( logger, "Parse Path:" << container_ )

    //search for regular files
    boost::filesystem::directory_iterator end_itr;
    for ( boost::filesystem::directory_iterator itr ( _fs_path ); itr != end_itr; ++itr ) {

        std::string item_filepath_ = path + "/" + itr->path().filename().string();
        if ( boost::filesystem::is_regular_file ( itr->status() ) ) {

            std::string mime_type_ = http::mime::mime_type ( _mime_type ( itr->path().extension().string() ) );

            //Update the statistic
            statistic[mime_type_] = ( statistic.find ( mime_type_ ) != statistic.end() ?
                                      statistic.find ( mime_type_ )->second + 1 : 1 );

            if ( SquawkServer::instance()->dao()->touch ( item_filepath_, boost::filesystem::last_write_time ( itr->path() ) ) == 0 ) {
                std::string item_stem_ = itr->path().stem().string();
                auto file_type_ = _file_type ( mime_type_ );
                didl::DidlItem item_ (
                    file_type_, 0, container_.id(), item_stem_, item_filepath_,
                    boost::filesystem::last_write_time ( itr->path() ), 0,
                    boost::filesystem::file_size ( itr->path() ), mime_type_, std::list<didl::DidlResource>(), false );

                if ( file_type_ == didl::objectItemAudioItemMusicTrack ) {
                    //parse and save audiofile
                    didl::DidlMusicTrack track_ = _import_track ( item_ );

                    if ( ! UpnpContentDirectoryParser::_multidisc_name ( container_stem_ ) ) {
                        parent_changed_ = true;
                        container_type_ = DIDL_PARSE_TYPES::music_album;
                        album_ = didl::DidlContainerAlbum (
                                     container_.id(), container_.parentId(), track_.album(),
                                     container_.path(), container_.mtime(), 0, 0, 0,
                                     track_.year(), 0, track_.contributor(), track_.artist(),
                                     track_.genre(), std::list< didl::DidlAlbumArtUri >(), false );

                    } else {
                        //XXX is multidisc folder
                        //XXX actual dir is not a album
                        //XXX change parent to album
                        parent_changed_ = true;
                        container_type_ = DIDL_PARSE_TYPES::multidisc;
                        didl::DidlContainer _multidisc_container = SquawkServer::instance()->dao()->object<didl::DidlContainer> ( container_.parentId() );
                        album_ =  didl::DidlContainerAlbum ( _multidisc_container.id(), _multidisc_container.parentId(), track_.album(),
                                                             _multidisc_container.path(), _multidisc_container.mtime(), 0, 0, 0,
                                                             track_.year(), 0, track_.contributor(), track_.artist(),
                                                             track_.genre(), std::list< didl::DidlAlbumArtUri >(), false );
                    }

                } else if ( file_type_ == didl::objectItemImageItemPhoto ) {
                    _import_photo ( item_ );

                    if ( container_type_ == DIDL_PARSE_TYPES::container ) {
                        parent_changed_ = true;
                        container_type_ = DIDL_PARSE_TYPES::photo_album;
                        photo_album_ = didl::DidlContainerPhotoAlbum ( container_.id(), container_.parentId(), container_stem_, container_.path(),
                                       container_.mtime(), container_.objectUdpateId(), 0, 0, 0 /*aear*/, 0, "", "",
                                       "", std::list< didl::DidlAlbumArtUri >(), false );
                    }

                    if ( _cover ( item_stem_ ) ) {
                        albumArtUri_.push_back (
                            didl::DidlAlbumArtUri ( 0, 0,
                                                    SquawkServer::instance()->config()->tmpDirectory() + "/AlbumArtUri/" + std::to_string ( container_.id() ) + ".jpg", "URI", "JPEG_TN" ) );

                        std::stringstream cover_stream_;
                        cover_stream_ << SquawkServer::instance()->config()->tmpDirectory() << "/AlbumArtUri/" << container_.id() << ".jpg";
                        image::Image image ( item_filepath_ );
                        image.scale ( 160, 160, cover_stream_.str() );
                    }

                } else if ( file_type_ == didl::objectItemVideoItemMovie ) {
                    _import_movie ( item_ );

                } else if ( file_type_ == didl::objectItemEBook ) {
                    _import_ebook ( item_ );

                } else {
                    SquawkServer::instance()->dao()->save ( item_ );
                }
            }

        } else if ( boost::filesystem::is_directory ( itr->status() ) ) {
            child_directories_.push_back ( item_filepath_ );

        } else {
            LOG4CXX_WARN ( logger, "path is neither a reqular file nor a directory:" << path )
        }
    }//end directory iterator (_parent_changed, child_directories)

    if ( parent_changed_ ) {
        if ( container_type_ == DIDL_PARSE_TYPES::music_album ||
                container_type_ == DIDL_PARSE_TYPES::multidisc ) {
            SquawkServer::instance()->dao()->save ( didl::DidlContainerAlbum ( album_.id(), album_.parentId(), album_.title(), album_.path(),
                                                    album_.mtime(), album_.objectUdpateId(), album_.childCount(),
                                                    album_.rating(), album_.year(), album_.playbackCount(),
                                                    album_.contributor(), album_.artist(), album_.genre(),
                                                    albumArtUri_, false ) );

        } else if ( container_type_ == DIDL_PARSE_TYPES::photo_album ) {
            SquawkServer::instance()->dao()->save ( photo_album_ );

        } else {
            SquawkServer::instance()->dao()->save ( container_ );
        }
    }

    //continue with children
    bool is_multidisc_ = false;

    for ( auto s : child_directories_ ) {
        DIDL_PARSE_TYPES child_type = _parse ( container_, s );

        if ( child_type == DIDL_PARSE_TYPES::multidisc ) {
            is_multidisc_ = true;
        }
    }

    if ( is_multidisc_ && parent_changed_ ) {
        didl::DidlContainerAlbum album__ = SquawkServer::instance()->dao()->object<didl::DidlContainerAlbum> ( container_.id() );
        SquawkServer::instance()->dao()->save ( didl::DidlContainerAlbum ( album__.id(), album__.parentId(), album__.title(), album__.path(),
                                                album__.mtime(), album__.objectUdpateId(), album__.childCount(),
                                                album__.rating(), album__.year(), album__.playbackCount(),
                                                album__.contributor(), album__.artist(), album__.genre(),
                                                albumArtUri_, false ) );
        std::list<didl::DidlContainerPhotoAlbum> photo_album__ = SquawkServer::instance()->dao()->children<didl::DidlContainerPhotoAlbum> ( container_.id(), 0, 0 );

        for ( auto photo : photo_album__ ) {
            SquawkServer::instance()->dao()->save ( didl::DidlContainer ( photo.id(), photo.parentId(), photo.title(), photo.path(),
                                                    photo.mtime(), photo.objectUdpateId(), 0, false ) );

        }
    }

    return container_type_;
}

std::string UpnpContentDirectoryParser::_clean_name ( const std::string & name ) {
    std::string s = name;

    std::string allowed = "+-()";
    s.erase ( remove_if ( s.begin(), s.end(), [&allowed] ( const char& c ) {
        return allowed.find ( c ) != string::npos;
    } ), s.end() );

    boost::algorithm::trim ( s );
    boost::algorithm::to_lower ( s );

    if ( s.rfind ( "the ", 0 ) == 0 )
    { s.erase ( 0, 4 ); }

    if ( s.rfind ( "die ", 0 ) == 0 )
    { s.erase ( 0, 4 ); }

    if ( s.rfind ( "das ", 0 ) == 0 )
    { s.erase ( 0, 4 ); }

    if ( s.rfind ( "der ", 0 ) == 0 )
    { s.erase ( 0, 4 ); }

    return s;
}
}//namespace squawk
