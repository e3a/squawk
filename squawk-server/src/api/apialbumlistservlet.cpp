/*
    API Albums Servlet
    Copyright (C) 2013  <copyright holder> <e.knecht@netwings.ch>

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

#include "apialbumlistservlet.h"

#include "commons.h"
#include "squawk.h"

#define QUERY_ALBUMS_COUNT "select count(*) from tbl_cds_albums"
#define QUERY_ALBUMS "select name, genre, year, ROWID from tbl_cds_albums ORDER BY name"
#define QUERY_ALBUMS_PAGE "select name, genre, year, ROWID from tbl_cds_albums ORDER BY name LIMIT ?, ?"
#define QUERY_ARTIST_BY_ALBUM "select artist.ROWID, artist.name from tbl_cds_artists artist " \
                              "JOIN tbl_cds_artists_albums m ON artist.ROWID = m.artist_id " \
                              "where m.album_id=?"

namespace squawk {
namespace api {

log4cxx::LoggerPtr ApiAlbumListServlet::logger(log4cxx::Logger::getLogger("squawk.servlet.ApiAlbumListServlet"));

void ApiAlbumListServlet::do_get( http::HttpRequest & request, ::http::HttpResponse & response ) {
    std::cout << "get albums:\n" << request << std::endl;
    squawk::db::Sqlite3Statement * stmt_count = NULL;
    squawk::db::Sqlite3Statement * stmt = NULL;
    squawk::db::Sqlite3Statement * stmt_artist = NULL;
    try {
        response << "{";
        std::string qAlbums = "from tbl_cds_albums albums ";
        bool first_query_token = true;
        if( request.containsAttribute( "artist-id" ) &&
            commons::string::is_number( commons::string::trim( request.attribute("artist-id") ) ) ) {
            first_query_token = false;
            qAlbums = "from tbl_cds_albums albums " \
                      "JOIN tbl_cds_artists_albums m ON albums.ROWID = m.album_id " \
                      "where m.artist_id=" + request.attribute( "artist-id" );
        }
        if( request.containsAttribute( "artist" ) ) {
            first_query_token = false;
            qAlbums = "from tbl_cds_albums albums " \
                    "JOIN tbl_cds_artists_albums m ON album.ROWID = m.album_id, " \
                    "JOIN tbl_cds_artists_albums m ON album.ROWID = m.album_id " \
                      "where m.artist_id=?";
        }
        if( request.containsAttribute( "letter" ) &&
            request.attribute( "letter" ).size() == 1 && request.attribute( "letter" ) != "\"") {
            if ( first_query_token ) {
                first_query_token = false;
                qAlbums += " where ";
            } else qAlbums += " and ";
            qAlbums += " letter = \"" + request.attribute( "letter" ) + "\"";
        }
        if( request.containsAttribute( "name" ) ) {
            if ( first_query_token ) {
                first_query_token = false;
                qAlbums += " where ";
            } else qAlbums += " and ";
            qAlbums += " clean_name like \"" + request.attribute( "name" ) + "\"";
        }
        if( request.containsAttribute( "genre" ) ) {
            if ( first_query_token ) {
                first_query_token = false;
                qAlbums += " where ";
            } else qAlbums += " and ";
            qAlbums += " genre = \"" + request.attribute( "genre" ) + "\"";
        }
        if( request.containsAttribute( "year" ) &&
            commons::string::is_number( request.attribute( "year" ) ) ) {
            if ( first_query_token ) {
                first_query_token = false;
                qAlbums += " where ";
            } else qAlbums += " and ";
            qAlbums += " year = \"" + request.attribute( "year" ) + "\"";
        }
/*        if( request.parameters.find("order-cryteria")==request.parameters.end() ) {

        } else {

        }
        if( request.parameters.find("order")==request.parameters.end() ) {

        } else {

        } */

        //create count element
        bool has_count = false;
        if( request.containsAttribute( "attributes" ) ||
            request.attribute( "attributes" ).find("count") != std::string::npos ) {

            if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "select count(*) " + qAlbums );
            has_count = true;
            stmt_count = db->prepare_statement( "select count(*) " + qAlbums );
            while( stmt_count->step() ) {
                response << "\"count\":" << std::to_string( stmt_count->get_int(0) );
            }
        }

        //set the pager
        if(request.containsAttribute( "index" ) && commons::string::is_number( request.attribute( "index" ) ) &&
           request.containsAttribute( "limit" ) &&  commons::string::is_number( request.attribute( "limit" ) ) ) {
            if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "select albums.name, albums.genre, albums.year, albums.ROWID " + qAlbums + " LIMIT ?, ?");
            stmt = db->prepare_statement( "select albums.name, albums.genre, albums.year, albums.ROWID " + qAlbums + " LIMIT ?, ?" );
            stmt->bind_int(1, commons::string::parse_string<int>(request.attribute( "index" ) ) );
            stmt->bind_int(2, commons::string::parse_string<int>(request.attribute( "limit" ) ) );
        } else {
            if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "select albums.name, albums.genre, albums.year, albums.ROWID " + qAlbums );
            stmt = db->prepare_statement( "select albums.name, albums.genre, albums.year, albums.ROWID " + qAlbums );
        }
        stmt_artist = db->prepare_statement( QUERY_ARTIST_BY_ALBUM );

        if( request.containsAttribute( "attributes" ) ||
            ( request.attribute( "attributes" ).find("name") != std::string::npos ||
              request.attribute( "attributes" ).find("genre") != std::string::npos ||
              request.attribute( "attributes" ).find("year") != std::string::npos ||
              request.attribute( "attributes" ).find("id") != std::string::npos ||
              request.attribute( "attributes" ).find("artist") != std::string::npos ) ) {

            if( has_count ) response << ",";
            response << "\"albums\":[";
            bool first_album = true;
            while( stmt->step() ) {

                response << ( first_album ? "{" : ",{" );
                if( first_album ) first_album = false;
                bool first_attribute = true;
                if( request.containsAttribute( "attributes" ) ||
                    request.attribute( "attributes" ).find("name") != std::string::npos ) {
                    if( first_attribute ) first_attribute = false; else response << ",";
                    response << "\"name\":\"" << commons::string::escape_json(stmt->get_string(0)) << "\"";
                }
                if( request.containsAttribute( "attributes" ) ||
                    request.attribute( "attributes" ).find("genre") != std::string::npos ) {
                    if( first_attribute ) first_attribute = false; else response << ",";
                    response << "\"genre\":\"" << commons::string::escape_json(stmt->get_string(1)) << "\"";
                }
                if( request.containsAttribute( "attributes" ) ||
                    request.attribute( "attributes" ).find("year") != std::string::npos ) {
                    if( first_attribute ) first_attribute = false; else response << ",";
                    response << "\"year\":\"" << commons::string::escape_json(stmt->get_string(2)) << "\"";
                }
                if( request.containsAttribute( "attributes" ) ||
                    request.attribute( "attributes" ).find("id") != std::string::npos ) {
                    if( first_attribute ) first_attribute = false; else response << ",";
                    response << "\"id\":" << std::to_string(stmt->get_int(3));
                }

                if( request.containsAttribute( "attributes" ) ||
                    request.attribute( "attributes" ).find("artist") != std::string::npos ) {
                    response << ", \"artists\":[";
                    stmt_artist->bind_int(1, stmt->get_int(3));
                    bool first_artist = true;
                    while( stmt_artist->step() ) {
                        if( first_artist ){
                            first_artist = false;
                        } else response << ", ";

                        response << "{\"id\":" << std::to_string(stmt_artist->get_int(0)) <<
                                    ",\"name\":\"" << commons::string::escape_json(stmt_artist->get_string(1)) << "\"}";
                    }
                    response << "]";
                    stmt_artist->reset();
                }
                response << "}";
            }
            response << "]";

            stmt->reset();
            db->release_statement(stmt_artist);
            db->release_statement(stmt);
        }
        response << "}";

    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not get albums, Exception:" << e.code() << "-> " << e.what());
        if(stmt != NULL) db->release_statement(stmt);
        if(stmt_artist != NULL) db->release_statement(stmt_artist);
        throw http::http_status::INTERNAL_SERVER_ERROR;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Other Excpeption in get_albums.");
        throw http::http_status::INTERNAL_SERVER_ERROR;
    }
    response.set_mime_type( ::http::mime::JSON );
    response.status( ::http::http_status::OK );
}
}}
