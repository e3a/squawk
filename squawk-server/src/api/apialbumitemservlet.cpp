/*
    <one line to give the program's name and a brief idea of what it does.>
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

#include "apialbumitemservlet.h"

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiAlbumItemServlet::logger ( log4cxx::Logger::getLogger ( "squawk.api.ApiAlbumItemServlet" ) );

void ApiAlbumItemServlet::do_get ( http::HttpRequest & request, http::HttpResponse & response ) {

	int album_id = 0;
	bool result = match ( request.uri(), &album_id );

	if ( result && album_id > 0 ) {

		response << "{";

		try {
			squawk::db::db_statement_ptr stmt_album = db->prepareStatement ( squawk::sql::QUERY_ALBUM );
			stmt_album->bind_int ( 1, album_id );

			while ( stmt_album->step() ) {
				response << "\"name\":\"" << commons::string::escape_json ( stmt_album->get_string ( 0 ) ) <<
						 "\", \"genre\":\"" << commons::string::escape_json ( stmt_album->get_string ( 1 ) ) <<
						 "\", \"year\":\"" << stmt_album->get_string ( 2 ) << "\", \"id\":" << std::to_string ( stmt_album->get_int ( 3 ) );
			}

			//  get artists
			squawk::db::db_statement_ptr stmt_artists = db->prepareStatement ( squawk::sql::QUERY_ARTIST_BY_ALBUM );
			stmt_artists->bind_int ( 1, album_id );
			response << ", \"artists\":[";
			bool first_artist = true;

			while ( stmt_artists->step() ) {
				if ( first_artist ) {
					first_artist = false;

				} else { response << ", "; }

				response << "{\"id\":" << std::to_string ( stmt_artists->get_int ( 0 ) ) <<
						 ",  \"name\":\"" << commons::string::escape_json ( stmt_artists->get_string ( 1 ) ) << "\"}";
			}

			//  get songs
			squawk::db::db_statement_ptr stmt_songs = db->prepareStatement ( squawk::sql::QUERY_SONGS );
			stmt_songs->bind_int ( 1, album_id );
			response << "], \"songs\":[";
			bool first_song = true;

			while ( stmt_songs->step() ) {
				if ( first_song ) {
					first_song = false;

				} else { response << ", "; }

				response << "{\"id\":" << std::to_string ( stmt_songs->get_int ( 0 ) ) <<
						 ", \"title\":\"" << commons::string::escape_json ( stmt_songs->get_string ( 1 ) ) <<
						 "\", \"track\":" << std::to_string ( stmt_songs->get_int ( 2 ) ) <<
						 ", \"filename\":\"" << commons::string::escape_json ( stmt_songs->get_string ( 3 ) ) <<
						 "\", \"length\":" << commons::string::escape_json ( stmt_songs->get_string ( 4 ) ) <<
						 ", \"bitrate\":" << std::to_string ( stmt_songs->get_int ( 5 ) ) <<
						 ", \"samplerate\":" << std::to_string ( stmt_songs->get_int ( 6 ) ) <<
						 ", \"bps\":" << std::to_string ( stmt_songs->get_int ( 7 ) ) <<
						 ", \"channels\":" << std::to_string ( stmt_songs->get_int ( 8 ) ) <<
						 ", \"mimetype\":\"" << commons::string::escape_json ( stmt_songs->get_string ( 9 ) ) <<
						 "\", \"disc\":" << std::to_string ( stmt_songs->get_int ( 10 ) ) << "}";
			}

			//  get images
			squawk::db::db_statement_ptr stmt_images = db->prepareStatement ( squawk::sql::QUERY_IMAGES );
			stmt_images->bind_int ( 1, album_id );
			response << "], \"images\":[";
			bool first_image = true;

			while ( stmt_images->step() ) {
				if ( first_image ) {
					first_image = false;

				} else { response << ", "; }
                //TODO Thumbnail
                response << "{\"thumb\":\"/album/image/" << std::to_string ( stmt_images->get_int ( 0 ) ) << ".jpg\", \"img\":\"/album/image/" <<
                            std::to_string ( stmt_images->get_int ( 0 ) ) << ".jpg\"}";
			}

			response << "]}";

		} catch ( squawk::db::DbException & e ) {
			LOG4CXX_FATAL ( logger, "Can not get album by id, Exception:" << e.code() << "-> " << e.what() );
			throw;
		}

	} else {
		throw http::http_status::BAD_REQUEST;
	}

	response.set_mime_type ( http::mime::JSON );
	response.status ( http::http_status::OK );
}
} // api
} // squawk
