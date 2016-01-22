/*
    Upnp video directory module.
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef UPNPVIDEODIRECTORY_H
#define UPNPVIDEODIRECTORY_H

#include "squawk.h"

#include "upnpcontentdirectory.h"
#include "upnpcontentdirectorydao.h"

#include "log4cxx/logger.h"

namespace squawk {

class UpnpContentDirectoryVideo : public ContentDirectoryModule {
public:
    UpnpContentDirectoryVideo( http::HttpServletContext context, ptr_upnp_dao upnp_cds_dao ) :
        _upnp_cds_dao( upnp_cds_dao ), http_address_( context.parameter( squawk::CONFIG_HTTP_IP ) ),
        http_port_( context.parameter( squawk::CONFIG_HTTP_PORT ) ) {}

    virtual int getRootNode( ::didl::DidlWriter * didl_element );
    virtual bool match( ::upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual std::tuple<size_t, size_t> parseNode( didl::DidlWriter * didl_element, ::upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    static log4cxx::LoggerPtr logger;
    ptr_upnp_dao _upnp_cds_dao;
    std::string http_address_;
    std::string http_port_;

//    /* get video count */
//    static constexpr const char * SQL_VIDEO_COUNT = "select count(*) from tbl_cds_files where type=2";
//    int videoCount() {
//        int count = 0;
//        try {
//            db::db_statement_ptr stmt_videos = db->prepareStatement( SQL_VIDEO_COUNT );
//            if( stmt_videos->step() ) {
//                count = stmt_videos->get_int( 0 );
//            }

//        } catch( db::DbException & e ) {
//            LOG4CXX_FATAL(logger, "Can not get video count, Exception:" << e.code() << "-> " << e.what());
//            throw;
//        } catch( ... ) {
//            LOG4CXX_FATAL(logger, "Other Excpeption in video count.");
//            throw;
//        }
//        return count;
//    }

//    /* get videos */
//    static constexpr const char * SQL_VIDEOS = \
//            "select ROWID, name, mime_type, duration, filesize, sampleFrequency, " \
//            "width, height, bitrate, channels from tbl_cds_files where type=2 order by name";
//    void videos( std::function<void(didl::DidlMovie)> callback ) {
//        try {
//            db::db_statement_ptr stmt_videos = db->prepareStatement( SQL_VIDEOS );
//            while( stmt_videos->step() ) {

//                std::string mime_type = stmt_videos->get_string( 2 );
//                std::string tmp_type = mime_type;
//                if(mime_type == "video/avi") {
//                    tmp_type = "video/mpeg";
//                } else if( mime_type == "video/x-matroska" ) {
//                    tmp_type = "video/mpeg";
//                }

//                callback( didl::DidlMovie(
//                          "/video/" + std::to_string( stmt_videos->get_int( 0 ) ) /*id*/,
//                          "music.album", stmt_videos->get_string( 1 ) /*title*/,
//                          0, "", "",
//                            std::list< didl::DidlAlbumArtUri >(),
//                            std::list< didl::DidlAudioItemRes >( {
//                                didl::DidlAudioItemRes(
//                                 "http://" + http_address_ + ":" + http_port_ + "/video/" + std::to_string( stmt_videos->get_int( 0 ) ) + "." + http::mime::extension( mime_type ),
//                                 "http-get:*:" + mime_type  + ":*" ,
//                                    std::map<didl::UPNP_RES_ATTRIBUTES, std::string >( {
//                                        { didl::duration, stmt_videos->get_string( 3 ) },
//                                        { didl::size, stmt_videos->get_string( 4 ) },
//                                        { didl::sampleFrequency, stmt_videos->get_string( 5 ) },
//                                        { didl::resolution, stmt_videos->get_string( 6 ) + "x" + stmt_videos->get_string( 7 ) },
//                                        { didl::bitrate, stmt_videos->get_string( 8 ) },
//                                        { didl::channels, stmt_videos->get_string( 9 ) }
//                                    }
//                                )
//                            ) } )
//                ));
//            }

//        } catch( db::DbException & e ) {
//            LOG4CXX_FATAL(logger, "Can not get videos, Exception:" << e.code() << "-> " << e.what());
//            throw;
//        } catch( ... ) {
//            LOG4CXX_FATAL(logger, "Other Excpeption in get video.");
//            throw;
//        }
//    }
};
}
#endif // UPNPVIDEODIRECTORY_H
