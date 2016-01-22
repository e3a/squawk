#ifndef UPNPCONTENTDIRECTORYPARSER_H
#define UPNPCONTENTDIRECTORYPARSER_H

#include "gtest/gtest_prod.h"

#include "pcrecpp.h"

#include "squawk.h"
#include "upnpcontentdirectorydao.h"

namespace squawk {



class UpnpContentDirectoryParser {
public:
    UpnpContentDirectoryParser( http::HttpServletContext & context, ptr_upnp_dao upnp_container_dao );

    UpnpContentDirectoryParser ( const UpnpContentDirectoryParser& ) = delete;
    UpnpContentDirectoryParser ( UpnpContentDirectoryParser&& ) = delete;
    UpnpContentDirectoryParser& operator= ( const UpnpContentDirectoryParser& ) = delete;
    UpnpContentDirectoryParser& operator= ( UpnpContentDirectoryParser&& ) = delete;
    ~UpnpContentDirectoryParser() { }

    enum FILE_TYPE { MP3, OGG, FLAC, MUSEPACK, MONKEY_AUDIO, IMAGE, AUDIOFILE, IMAGEFILE, VIDEOFILE, EBOOK, UNKNOWN };
    enum DIRECTORY_TYPE { MUSIC, IMAGES, MOVIES, NONE };

    void parse ( std::vector< std::string > paths );
private:
    static log4cxx::LoggerPtr logger;
    ptr_upnp_dao _upnp_container_dao;
    const std::string _tmp_directory;

    std::map<std::string, int> statistic;

    void _parse ( const unsigned long & path_id, const std::string & path );
    void _import_audio ( const didl::DidlObject obje );
    void _import_movies ( const didl::DidlObject object );
    void _import_images ();
    void _import_books ();

    void _scale_image( const std::string & profile, const std::string & path, int image_id, const std::string & target );

    FRIEND_TEST( TestUpnpContentDirectoryParser, ParseMultidiscName );
    static bool _multidisc_name( const std::string & path ) {
        return pcrecpp::RE("CD[\\d+]", pcrecpp::RE_Options().set_caseless( true ) ).PartialMatch( path );
    }
    FRIEND_TEST(TestUpnpContentDirectoryParser, ParseCoverName );
    static bool _cover( const std::string & path ) {
        return( boost::algorithm::to_lower_copy( path ) == "cover" ||
                boost::algorithm::to_lower_copy( path ) == "front" );
    }
};
}//namespace squawk
#endif // UPNPCONTENTDIRECTORYPARSER_H
