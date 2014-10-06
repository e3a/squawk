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

#include "squawkservice.h"

namespace squawk {

log4cxx::LoggerPtr SquawkServiceImpl::logger(log4cxx::Logger::getLogger("squawk.SquawkServiceImpl"));

void SquawkServiceImpl::start_transaction() {
  dao->start_transaction();
}
void SquawkServiceImpl::end_transaction() {
  dao->end_transaction();
}

std::list< squawk::model::Album > SquawkServiceImpl::get_albums() {
  try {
    return dao->get_albums();
  } catch(squawk::db::DaoException & e) {
    LOG4CXX_ERROR(logger, "get_albums exception:" << e.code() << ":" << e.what())
  }
  return std::list< squawk::model::Album >();
}
std::list< squawk::model::Album > SquawkServiceImpl::get_albums_by_artist(unsigned long artist_id) {
  try {
    return dao->getAlbumsByArtist(artist_id);
  } catch(squawk::db::DaoException & e) {
    LOG4CXX_ERROR(logger, "get_albums_by_artist exception: artist_id:" << artist_id << ", " << e.code() << ":" << e.what())
  }
  return std::list< squawk::model::Album >();
}
std::list< squawk::model::Artist > SquawkServiceImpl::get_artists() {
  try {
    return dao->get_artists();
  } catch(squawk::db::DaoException & e) {
    LOG4CXX_ERROR(logger, "get_artists exception:" << e.code() << ":" << e.what())
  }
  return std::list< squawk::model::Artist >();
}
squawk::model::Album SquawkServiceImpl::get_album(unsigned long id, bool recursive) {
  try {
    squawk::model::Album album = dao->get_album(id);
    if(recursive) {
      album.songs = dao->getSongsByAlbum(id);
      album.images = dao->get_images_by_album(id);
    }
    return album;
  } catch(squawk::db::DaoException & e) {
    LOG4CXX_ERROR(logger, "get_album exception: id=" << id << ", " << e.code() << ":" << e.what())
  }
  return squawk::model::Album();
}
squawk::model::Album SquawkServiceImpl::get_album(std::string path) {
  try {
    return(dao->get_album(path));
  } catch(squawk::db::DaoException & e) {
    LOG4CXX_ERROR(logger, "get_album exception: path=" << path << ", " << e.code() << ":" << e.what())
  }
  return squawk::model::Album();
}

bool SquawkServiceImpl::exist_audiofile(std::string path, unsigned long mtime, unsigned long size) {
  return(dao->exist_audiofile(path, mtime, size, true));
}
bool SquawkServiceImpl::exist_imagefile(std::string path, unsigned long mtime, unsigned long size) {
  return(dao->exist_imagefile(path, mtime, size, true));
}
unsigned long SquawkServiceImpl::save_artist(squawk::model::Artist & artist) {
  try {
    unsigned long artist_id = dao->getArtistId(artist.clean_name);
    if(artist_id == 0) {
      artist_id = dao->save_artist(artist);
    }
    return artist_id;
  } catch(squawk::db::DaoException & e) {
    LOG4CXX_ERROR(logger, "save_artist exception:" << e.code() << "=" << e.what())
  }
  return 0;
}
unsigned long SquawkServiceImpl::save_album(std::string path, squawk::model::Album * album) {
  try {
    int album_id = dao->getAlbumId(path);
    if(album_id == 0) {
      album_id = dao->save_album(path, *album);
    }
    return album_id;
  } catch(squawk::db::DaoException & e) {
    LOG4CXX_ERROR(logger, "save_album exception:" << e.code() << "=" << e.what())
  }
  return 0;
}
unsigned long SquawkServiceImpl::save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::model::Song * song) {
  dao->save_audiofile(filename, mtime, size, album_id, song);
  return -1; //XXX
}
unsigned long SquawkServiceImpl::save_imagefile(std::string filename, long mtime, long size, unsigned long album_id, squawk::model::Image * imagefile) {
  return dao->save_imagefile(filename, mtime, size, album_id, imagefile);
}
}
