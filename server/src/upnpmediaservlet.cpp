/*
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
#include "squawkserver.h"
#include "upnpcontentdirectorydao.h"

namespace squawk {

inline std::string tmp_path() {
    std::stringstream ss;
    std::string tmp_directory_ = SquawkServer::instance()->config()->tmpDirectory();
    ss << ( tmp_directory_.find_last_of ( "/" ) == tmp_directory_.length() ? tmp_directory_ : tmp_directory_ ) + "/";
    return ss.str();
}

void UpnpMediaServlet::do_get ( http::HttpRequest & request, http::HttpResponse & response ) {

    _process_file ( request, response );

    //check if file exists
    boost::filesystem::path path_ ( request.uri() );

    if ( boost::filesystem::exists ( path_ ) && boost::filesystem::is_regular_file ( path_ ) ) {
        // Open the file to send back.
        std::ifstream * is = new std::ifstream ( request.uri().c_str(), std::ios::in | std::ios::binary ); //TODO remove new

        if ( !is->is_open() ) {
            throw http::http_status::NOT_FOUND;
        }

        // Fill out the reply to be sent to the client.
        size_t file_size_ = boost::filesystem::file_size ( path_ );

        if ( request.containsParameter ( http::header::RANGE ) ) {
            response.status ( http::http_status::PARTIAL_CONTENT );
            std::tuple<int, int> range = http::utils::parseRange ( request.parameter ( http::header::RANGE ) );
            response.parameter ( "Content-Range", "bytes " + std::to_string ( std::get<0> ( range ) ) + "-" +
                                 ( std::get<1> ( range ) == -1 ? std::to_string ( file_size_ ) :
                                   std::to_string ( std::get<1> ( range ) - 1 ) ) +
                                 "/" + std::to_string ( file_size_ ) );
            response.parameter ( http::header::CONTENT_LENGTH, ( std::get<1> ( range ) == -1 ? std::to_string ( file_size_ - std::get<0> ( range ) ) :
                                 std::to_string ( std::get<1> ( range ) - std::get<0> ( range ) ) ) );
            is->seekg ( std::get<0> ( range ), std::ios_base::beg );

        } else {
            response.parameter ( http::header::CONTENT_LENGTH, std::to_string ( file_size_ ) );
            response.status ( http::http_status::OK );
        }

        response.set_istream ( is );
        _dlna_headers ( request, response );

    } else {
        throw http::http_status::NOT_FOUND;
    }
}
void UpnpMediaServlet::do_head ( http::HttpRequest & request, http::HttpResponse & response ) {

    _process_file ( request, response );

    //check if file exists
    boost::filesystem::path path_ ( request.uri() );

    if ( boost::filesystem::exists ( path_ ) && boost::filesystem::is_regular_file ( path_ ) ) {

        // Fill out the reply to be sent to the client.
        size_t file_size_ = boost::filesystem::file_size ( path_ );

        if ( request.containsParameter ( http::header::RANGE ) ) {
            response.status ( http::http_status::PARTIAL_CONTENT );
            std::tuple<int, int> range = http::utils::parseRange ( request.parameter ( http::header::RANGE ) );
            response.parameter ( "Content-Range", "bytes " + std::to_string ( std::get<0> ( range ) ) + "-" +
                                 ( std::get<1> ( range ) == -1 ? std::to_string ( file_size_ ) :
                                   std::to_string ( std::get<1> ( range ) - 1 ) ) +
                                 "/" + std::to_string ( file_size_ ) );
            response.parameter ( http::header::CONTENT_LENGTH, ( std::get<1> ( range ) == -1 ? std::to_string ( file_size_ - std::get<0> ( range ) ) :
                                 std::to_string ( std::get<1> ( range ) - std::get<0> ( range ) ) ) );

        } else {
            response.parameter ( http::header::CONTENT_LENGTH, std::to_string ( file_size_ ) );
            response.status ( http::http_status::OK );
        }

        _dlna_headers ( request, response );
    }
}

void UpnpMediaServlet::_process_file ( http::HttpRequest & request, http::HttpResponse & response ) {

    if ( request.uri().empty() || request.uri().find ( ".." ) != std::string::npos ) {
        throw http::http_status::BAD_REQUEST;
    }

    std::string type_, filename_, extension_;

    if ( match ( request.uri(), &type_, &filename_, &extension_ ) ) {

        if ( type_ == "resource" ) {
            try {
                db::db_statement_ptr stmt_resource = SquawkServer::instance()->db()->prepareStatement ( "select path, mime_type from tbl_cds_resource where ROWID = ?" );
                stmt_resource->bind_int ( 1, std::stoi ( filename_ ) );

                if ( stmt_resource->step() ) {
                    request.uri ( stmt_resource->get_string ( 0 ) );
                    response.set_mime_type ( http::mime::mime_type ( stmt_resource->get_string ( 0 ) ) );

                } else {
                    throw http::http_status::NOT_FOUND;
                }

                //Add the DLNA headers if requested
                if ( request.containsParameter ( "Getmediainfo.sec" ) && request.parameter ( "Getmediainfo.sec" ) == "1" ) {
                    response.parameter ( "getMediaInfo.sec", "SEC_Duration=" + std::to_string ( 1000 * stmt_resource->get_int ( 1 ) ) + ";" );
                }

            } catch ( db::DbException & e ) {
                CLOG(ERROR, "upnp") << "Can not get resource from db: " << e.code() << ":" << e.what();
                throw http::http_status::INTERNAL_SERVER_ERROR;
            }

        } else if ( type_ == "albumArtUri" ) {

            request.uri ( SquawkServer::instance()->config()->tmpDirectory() + "/AlbumArtUri/" + filename_ + ".jpg" );
            response.set_mime_type ( http::mime::JPEG );
        }

    } else {
        throw http::http_status::NOT_FOUND;
    }
}
void UpnpMediaServlet::_dlna_headers ( http::HttpRequest & request, http::HttpResponse & response ) {
    if ( request.containsParameter ( "Getcontentfeatures.dlna.org" ) &&
            request.parameter ( "Getcontentfeatures.dlna.org" ) == "1" ) {
        response.parameter ( "transferMode.dlna.org", "Streaming" );
        response.parameter ( "Accept-Ranges", "bytes" );
        response.parameter ( "contentFeatures.dlna.org", "DLNA.ORG_OP=01;DLNA.ORG_CI=0;DLNA.ORG_FLAGS=017000 00000000000000000000000000" );
        response.parameter ( "EXT", "" );
    }

    response.parameter ( "Server", "Debian/wheezy/sid DLNADOC/1.50 UPnP/1.0 Squawk/0.1" ); //TODO
    response.parameter ( "Content-Disposition", "inline; filename= \"" +
                         ( request.uri().find_last_of ( "/" ) != std::string::npos ?
                           request.uri().substr ( request.uri().find_last_of ( "/" ) ) : request.uri() ) + "\"" );
    response.set_last_modified ( boost::filesystem::last_write_time ( request.uri() ) );
    response.set_expires ( 3600 * 24 );
    response.status ( http::http_status::OK );
}
}//namespace squawk
