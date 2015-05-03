/*
    media servlet implementation.
    Copyright (C) 2013  e.knecht@netwings.ch

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

#include "upnpmediaservlet.h"

#include <sstream>

#include "commons.h"
#include "mimetypes.h"
#include "squawk.h"

#define QUERY_SONG

namespace squawk {
namespace upnp {

log4cxx::LoggerPtr UpnpMediaServlet::logger(log4cxx::Logger::getLogger("squawk.upnp.UpnpMediaServlet"));

void UpnpMediaServlet::do_get( ::http::HttpRequest & request, ::http::HttpResponse & response ) {
    try {
        getFile( request, response );
/* TODO some double dots might also be in file name.
        if (request.uri.empty() || request.uri[0] != '/' || request.uri.find("..") != std::string::npos) {
          throw http::http_status::BAD_REQUEST;
        } */

        std::string full_path = request.uri();

        struct stat filestatus;
        stat( full_path.c_str(), &filestatus );
        if(S_ISDIR(filestatus.st_mode)) {
            full_path += std::string("/index.html");
            stat( full_path.c_str(), &filestatus );
        }
        std::cout << "FileServlet Full Path:" << full_path << std::endl;

        // Determine the filename and extension.
        std::size_t last_slash_pos = full_path.find_last_of("/");
        std::size_t last_dot_pos = full_path.find_last_of(".");
        std::string extension, filename;
        if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
          extension = full_path.substr(last_dot_pos + 1);
        }
        if (last_slash_pos != std::string::npos && last_slash_pos < full_path.size() ) {
          filename = full_path.substr(last_slash_pos + 1 );
        }

        // Open the file to send back.
        std::ifstream * is = new std::ifstream(full_path.c_str(), std::ios::in | std::ios::binary); //TODO remove new
        if ( !is->is_open() ) {
            std::cout << "can not open file:" << full_path << std::endl;
            throw http::http_status::NOT_FOUND;
        }

        // Fill out the reply to be sent to the client.
        if( request.containsParameter( http::header::RANGE ) ) {
            std::cout << "get Range" << std::endl;
            response.status( http::http_status::PARTIAL_CONTENT );
            std::tuple<int, int> range = http::utils::parseRange( request.parameter( http::header::RANGE ) );
            std::cout << "get range: " << std::get<0>(range) << "-" << std::get<1>(range) << std::endl;
            response.parameter( "Content-Range", "bytes " + std::to_string( std::get<0>(range) ) + "-" +
                                 ( std::get<1>(range) == -1 ? std::to_string( filestatus.st_size - 1 ) :
                                                              std::to_string( std::get<1>(range) - 1 ) ) +
                                   "/" + std::to_string( filestatus.st_size ) );
            response.parameter( http::header::CONTENT_LENGTH, ( std::get<1>(range) == -1 ? std::to_string( filestatus.st_size - std::get<0>(range) ) :
                                                                                          std::to_string( std::get<1>(range) - std::get<0>(range) ) ) );
            is->seekg( std::get<0>( range ), std::ios_base::beg ); //TODO check if range is available

        } else {
            response.parameter( http::header::CONTENT_LENGTH, std::to_string( filestatus.st_size ) );
            response.status( http::http_status::OK );
        }
    //    response.add_header( HTTP_HEADER_CONTENT_DISPOSITION, "inline; filename= \"" + filename + "\"" );
        if( ::http::mime::mime_type(extension )== http::mime::AVI || ::http::mime::mime_type(extension) == http::mime::MKV )
            response.set_mime_type( http::mime::VIDEOMPEG );
        else
            response.set_mime_type( ::http::mime::mime_type( extension ) );

    //    response.set_last_modified( filestatus.st_mtime );
    //    response.set_expires( 3600 * 24 );
        response.set_istream( is );

    } catch( ... ) {
        throw;
    }
}
void UpnpMediaServlet::do_head( ::http::HttpRequest & request, ::http::HttpResponse & response ) {
    try {
        getFile( request, response );

        if (request.uri().empty() || request.uri()[0] != '/' || request.uri().find("..") != std::string::npos) {
          throw http::http_status::BAD_REQUEST;
        }

        std::string full_path = request.uri();

        struct stat filestatus;
        stat( full_path.c_str(), &filestatus );
        if(S_ISDIR(filestatus.st_mode)) {
            full_path += std::string("/index.html");
            stat( full_path.c_str(), &filestatus );
        }
        std::cout << "FileServlet Full Path:" << full_path << std::endl;

        // Determine the filename and extension.
        std::size_t last_slash_pos = full_path.find_last_of("/");
        std::size_t last_dot_pos = full_path.find_last_of(".");
        std::string extension, filename;
        if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
          extension = full_path.substr(last_dot_pos + 1);
        }
        if (last_slash_pos != std::string::npos && last_slash_pos < full_path.size() ) {
          filename = full_path.substr(last_slash_pos + 1 );
        }

        // Open the file to send back.
        std::ifstream * is = new std::ifstream(full_path.c_str(), std::ios::in | std::ios::binary); //TODO remove new
        if ( !is->is_open() ) {
            std::cout << "can not open file:" << full_path << std::endl;
            throw http::http_status::NOT_FOUND;
        }
        delete is;

        // Fill out the reply to be sent to the client.
        response.parameter( http::header::CONTENT_LENGTH, std::to_string( filestatus.st_size ) );
    //    response.add_header( HTTP_HEADER_CONTENT_DISPOSITION, "inline; filename= \"" + filename + "\"" );
        response.status( http::http_status::OK );

        if( ::http::mime::mime_type(extension )== http::mime::AVI || ::http::mime::mime_type(extension) == http::mime::MKV )
            response.set_mime_type( http::mime::VIDEOMPEG );
        else
            response.set_mime_type( ::http::mime::mime_type( extension ) );

        response.set_last_modified( filestatus.st_mtime );
    //    response.set_expires( 3600 * 24 );

    } catch( ... ) {
        throw;
    }
}

void UpnpMediaServlet::getFile( ::http::HttpRequest & request, ::http::HttpResponse & response ) {

    squawk::db::Sqlite3Statement * stmt_song = NULL;

    LOG4CXX_TRACE( logger, "get media file: " << request )

    int start = 0, end = 0;
    if(request.containsParameter( "Range" ) ) {
        std::string str_range = request.parameter( "Range" );
        if(commons::string::starts_with(str_range, "bytes=")) {
            str_range = str_range.substr(6, str_range.size());
            size_t dash_pos = str_range.find_first_of("-");
            if( dash_pos != string::npos ) {
                start = commons::string::parse_string<int>( str_range.substr(0, dash_pos));
                if(dash_pos < str_range.size() ) {
                    end = commons::string::parse_string<int>( str_range.substr(dash_pos+1, str_range.size()));
                }
            }
            if( squawk::DEBUG) LOG4CXX_TRACE( logger, "get range: " << start << "-" << end )
        }
    }
    std::string type; int song_id;
    bool result = match(request.uri(), &type, &song_id);
    if(result && song_id > 0) {

        try {

            LOG4CXX_TRACE( logger, "id: " << type << ":" << song_id )

            if( type == "audio" ) {
                stmt_song = db->prepare_statement( "select songs.filename from tbl_cds_audiofiles songs where songs.ROWID = ?" );
            } else if( type == "video" ) {
                stmt_song = db->prepare_statement( "select video.filename from tbl_cds_files video where video.ROWID = ?" );
            } else if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "can not find type: " << type )

            stmt_song->bind_int( 1, song_id );
            if( stmt_song->step() ) {
                request.uri( stmt_song->get_string(0) );

            } else {
                LOG4CXX_TRACE( logger, "file not found: " << song_id )
            }
            db->release_statement( stmt_song );

            //Add the DLNA headers if requested
            //TODO correct them ;)
            if(request.containsParameter( "Getcontentfeatures.dlna.org" ) &&
               request.parameter( "Getcontentfeatures.dlna.org" ) == "1") {
                response.parameter("transferMode.dlna.org", "Streaming");
                response.parameter("Accept-Ranges", "bytes");
                // response.add_header("realTimeInfo.dlna.org", "DLNA.ORG_TLAG=*");
                response.parameter("contentFeatures.dlna.org", "DLNA.ORG_OP=01;DLNA.ORG_CI=0;DLNA.ORG_FLAGS=01700000000000000000000000000000");
                // response.add_header("Connection", "close");
                response.parameter("EXT", "");
            }
            response.parameter("Server", "Debian/wheezy/sid DLNADOC/1.50 UPnP/1.0 Squawk/0.1");
        } catch( squawk::db::DbException & e ) {
            if( stmt_song != NULL ) {
                db->release_statement( stmt_song );
            }
            LOG4CXX_FATAL( logger, "can not get song path: " << e.code() << ":" << e.what() )
            throw ::http::http_status::INTERNAL_SERVER_ERROR;
        }

    } else {
        throw ::http::http_status::BAD_REQUEST;
    }
}
}}
