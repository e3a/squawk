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

#include "apialbumsbyartist.h"

#include <sstream>

namespace squawk {
namespace servlet {

log4cxx::LoggerPtr ApiAlbumsByArtist::logger(log4cxx::Logger::getLogger("squawk.servlet.ApiAlbumsByArtist"));

void ApiAlbumsByArtist::do_get(::http::HttpRequest & request, ::http::HttpResponse & response) {

    int artist_id = 0;
    bool result = match(request.uri, &artist_id);
    if(result && artist_id > 0) {

        /* TODO std::list< squawk::model::Album > albums = service->get_albums_by_artist(artist_id);
        std::stringstream * buf = new std::stringstream();
        (*buf) << "{\"albums\":[";
        bool first = true;
        for(std::list< squawk::model::Album >::iterator list_iter = albums.begin(); list_iter != albums.end(); list_iter++) {
            if(first) first = false;
            else (*buf) << ",";
            (*buf) << (*list_iter);
        }
        (*buf) << "]}";

        response.set_mime_type( ::http::mime::JSON );
        response.set_status( ::http::http_status::OK );
*/
    } else {
        throw ::http::http_status::BAD_REQUEST;
    }
} 
}}
