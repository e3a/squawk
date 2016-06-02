#ifndef UPNPCONTENTDIRECTORYPARSER_H
#define UPNPCONTENTDIRECTORYPARSER_H

#include <regex>

#include "squawk.h"
#include "squawkserver.h"
#include "upnpcontentdirectory.h"

namespace squawk {

class UpnpContentDirectoryParser {
public:
    UpnpContentDirectoryParser();

    UpnpContentDirectoryParser ( const UpnpContentDirectoryParser& ) = delete;
    UpnpContentDirectoryParser ( UpnpContentDirectoryParser&& ) = delete;
    UpnpContentDirectoryParser& operator= ( const UpnpContentDirectoryParser& ) = delete;
    UpnpContentDirectoryParser& operator= ( UpnpContentDirectoryParser&& ) = delete;
    ~UpnpContentDirectoryParser() { }

    enum FILE_TYPE { MP3, OGG, FLAC, MUSEPACK, MONKEY_AUDIO, IMAGE, AUDIOFILE, IMAGEFILE, VIDEOFILE, EBOOK, UNKNOWN };
    enum DIRECTORY_TYPE { MUSIC, IMAGES, MOVIES, NONE };

    void parse ( std::list< std::string > paths );

    /**
     * @brief get the clean name.
     * @param name
     * @return
     */
    static std::string _clean_name( const std::string & name );

private:
    std::map<std::string, int> statistic;

    enum DIDL_PARSE_TYPES { container, music_album, multidisc, photo_album };


    DIDL_PARSE_TYPES _parse( didl::DidlContainer & parent, const std::string & path );
    void _import_audio ( const didl::DidlObject obje );
    void _import_movies ( const didl::DidlObject object );
    void _import_images ( const didl::DidlObject object );
    void _import_books ();


    didl::DidlMusicTrack _import_track ( const didl::DidlItem & track );
    void _import_photo( const didl::DidlItem & photo );
    void _import_movie( const didl::DidlItem & movie );
    void _import_ebook( const didl::DidlItem & ebook );

    FRIEND_TEST( UpnpContentDirectoryParserTest, ParseSeries );
    static bool _parse_series( const std::string & filename, int * season, int * episode, std::string * name ) {
        const std::regex _series_pattern( "(.*)S(\\d*)E(\\d*).*", std::regex::icase );
        std::smatch match_;
        if ( std::regex_match( filename, match_, _series_pattern ) && match_.size() > 0 ) {
            *name = match_.str( 1 );
            std::replace( name->begin(), name->end(), '.', ' ');
            boost::trim(*name);
            *season = std::stoi( match_.str( 2 ) );
            *episode = std::stoi( match_.str( 3 ) );
            return true;
        } else return false;
    }

    FRIEND_TEST( UpnpContentDirectoryParserTest, ParseMultidiscName );
    static bool _multidisc_name( const std::string & path ) {
        const std::regex _pattern( "cd\\d*|disc \\d*+of\\d*|disk \\d*", std::regex::icase );
        return std::regex_match( path, _pattern );
    }

    static bool _cover( const std::string & path ) {
        for( auto item : SquawkServer::instance()->config()->coverNames() ) {
            if( boost::algorithm::trim_copy( boost::algorithm::to_lower_copy( item ) ) ==
                boost::algorithm::trim_copy( boost::algorithm::to_lower_copy( path ) ) ) {

                return true;
            }
        }
        return false;
    }

    FRIEND_TEST( UpnpContentDirectoryParserTest, MimeType );
    /**
     * @brief get the mime-type from the boost path object.
     * @param extension the file extension including the dot (i.e. .txt)
     * @return the mime-type
     */
    static http::mime::MIME_TYPE _mime_type ( const std::string & extension ) {
        if(  extension.size() == 0 || extension == "." ) return http::mime::MIME_TYPE::TEXT;
        else return http::mime::mime_type ( boost::algorithm::to_lower_copy( extension.substr( 1 ) ) );
    }

    FRIEND_TEST( UpnpContentDirectoryParserTest, FileType );
    /**
     * @brief _file_type
     * @param mime_type
     * @return
     */
    static didl::DIDL_CLASS _file_type( const std::string & mime_type ) {
        if ( mime_type.find( "audio/" ) == 0 ) {
            return didl::objectItemAudioItemMusicTrack;

        } if( mime_type.find( "image/" ) == 0 ) {
            return didl::objectItemImageItemPhoto;

        } else if  ( mime_type.find( "video/" ) == 0 ) {
            return didl::objectItemVideoItemMovie;

        } else if ( mime_type == "application/pdf" ) {
            return didl::objectItemEBook;

        } else if( squawk::SUAWK_SERVER_DEBUG ) {
            CLOG (ERROR, "upnp") << "can not find object type for:" << mime_type;
        }
        return didl::objectItem;
    }
};
}//namespace squawk
#endif // UPNPCONTENTDIRECTORYPARSER_H
