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
#include "../db/database.h"

#include <sstream>

#include "commons.h"

#define QUERY_SONG

namespace squawk {
namespace upnp {

log4cxx::LoggerPtr UpnpMediaServlet::logger(log4cxx::Logger::getLogger("squawk.upnp.UpnpMediaServlet"));

void UpnpMediaServlet::do_get( ::http::HttpRequest & request, ::http::HttpResponse & response ) {
    try {
        getFile( request, response );
        FileServlet::do_get(request, response);

    } catch( ... ) {
        throw;
    }
}
void UpnpMediaServlet::do_head( ::http::HttpRequest & request, ::http::HttpResponse & response ) {
    try {
        getFile( request, response );
        FileServlet::do_head(request, response);

    } catch( ... ) {
        throw;
    }
}

void UpnpMediaServlet::getFile( ::http::HttpRequest & request, ::http::HttpResponse & response ) {

    squawk::db::Sqlite3Statement * stmt_song = NULL;

    LOG4CXX_TRACE( logger, "get media file: " << request )

    int start = 0, end = 0;
    if(request.request_lines.find("Range") != request.request_lines.end() ) {
        std::string str_range = request.request_lines["Range"];
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
    bool result = match(request.uri, &type, &song_id);
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
                request.uri = stmt_song->get_string(0);

            } else {
                LOG4CXX_TRACE( logger, "file not found: " << song_id )
            }
            db->release_statement( stmt_song );

            //Add the DLNA headers if requested
            //TODO correct them ;)
            if(request.request_lines.find("Getcontentfeatures.dlna.org") != request.request_lines.end() &&
               request.request_lines["Getcontentfeatures.dlna.org"] == "1") {
                response.add_header("transferMode.dlna.org", "Streaming");
                response.add_header("Accept-Ranges", "bytes");
                response.add_header("realTimeInfo.dlna.org", "DLNA.ORG_TLAG=*");
                response.add_header("contentFeatures.dlna.org", "AVC_MP4_HP_HD_AAC;DLNA.ORG_OP=01;DLNA.ORG_CI=0;DLNA.ORG_FLAGS=01700000000000000000000000000000" ); //DLNA.ORG_OP=01;DLNA.ORG_CI=0;DLNA.ORG_FLAGS=01700000000000000000000000000000");
                response.add_header("Connection", "close");
                response.add_header("EXT", "");
            }
            response.add_header("Server", "Debian/wheezy/sid DLNADOC/1.50 UPnP/1.0 Squawk/0.1");
        } catch( ::db::DbException * e ) {
            if( stmt_song != NULL ) {
                db->release_statement( stmt_song );
            }
            LOG4CXX_FATAL( logger, "can not get song path: " << e->code() << ":" << e->what() )
            throw ::http::http_status::INTERNAL_SERVER_ERROR;
        }

    } else {
        throw ::http::http_status::BAD_REQUEST;
    }
}
}}
