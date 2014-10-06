/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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

#include "squawkdao.h"

#include <iostream>

#include <ctime>
#include <chrono>

const char *SQL_TABLE_CREATE_TABLE_AUDIO = "create table tbl_cds_audiofiles(album_id, filename NOT NULL, filesize, mtime, title, track, timestamp, mime_type, bitrate, sample_rate, bits_per_sample, channels, length, disc);";
const char *SQL_TABLE_CREATE_TABLE_AUDIO_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexFilename ON tbl_cds_audiofiles (filename)";
const char *SQL_TABLE_CREATE_TABLE_ARTISTS_ALBUMS = "create table tbl_cds_artists_albums(album_id, artist_id, role);";
const char *SQL_TABLE_CREATE_TABLE_ARTISTS_ALBUMS_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexFilename ON tbl_cds_audiofiles (album_id, artist_id)";
const char *SQL_TABLE_CREATE_TABLE_ARTISTS = "create table tbl_cds_artists(name, clean_name, letter);";
const char *SQL_TABLE_CREATE_TABLE_ALBUMS = "create table tbl_cds_albums(path, name, genre, year);";
const char *SQL_TABLE_CREATE_TABLE_IMAGES = "create table tbl_cds_images(album, filename, mtime, timestamp, filesize, type, mime_type, width, height);";
const char *SQL_TABLE_CREATE_TABLE_IMAGES_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexFilename ON tbl_cds_images (filename)";

namespace squawk {
namespace db {

log4cxx::LoggerPtr SquawkDAO::logger(log4cxx::Logger::getLogger("squawk.db.SquawkDAO"));

SquawkDAO::SquawkDAO(squawk::SquawkConfig * config) {
  db = new Sqlite3Database();
  db->open(config->string_value(CONFIG_DATABASE_FILE));

  //create tables if they dont exist
  if(exist_table(SQL_TABEL_NAME_AUDIO)==false) {
      create_table(SQL_TABLE_CREATE_TABLE_AUDIO);
  }
  create_table(SQL_TABLE_CREATE_TABLE_AUDIO_INDEX);
  if(exist_table(SQL_TABEL_NAME_ARTISTS_ALBUMS)==false) {
      create_table(SQL_TABLE_CREATE_TABLE_ARTISTS_ALBUMS);
  }
  create_table(SQL_TABLE_CREATE_TABLE_ARTISTS_ALBUMS_INDEX);
  if(exist_table(SQL_TABEL_NAME_ARTISTS)==false) {
      create_table(SQL_TABLE_CREATE_TABLE_ARTISTS);
  }
  if(exist_table(SQL_TABEL_NAME_ALBUMS)==false) {
      create_table(SQL_TABLE_CREATE_TABLE_ALBUMS);
  }
  if(exist_table(SQL_TABEL_NAME_IMAGES)==false) {
      create_table(SQL_TABLE_CREATE_TABLE_IMAGES);
  }
  
  stmtMap["tbl_cds_audiofiles"] = db->prepare_statement("select ROWID from tbl_cds_audiofiles where filename=? and filesize=? and mtime=?");
  stmtMap["update_tbl_cds_audiofiles"] = db->prepare_statement("update tbl_cds_audiofiles set timestamp=? where ROWID=?");
  stmtMap["tbl_cds_images"] = db->prepare_statement("select ROWID from tbl_cds_images where filename=? and filesize=? and mtime=?");
  stmtMap["update_tbl_cds_images"] = db->prepare_statement("update tbl_cds_images set timestamp=? where ROWID=?");
}
SquawkDAO::~SquawkDAO() {
    db->close();
    delete db;
}

void SquawkDAO::start_transaction() {
  db->exec("BEGIN;");
}
void SquawkDAO::end_transaction() {
  db->exec("END;");
}

bool SquawkDAO::exist_audiofile(std::string filename, long mtime, long size, bool update) {
  return exist(SQL_TABEL_NAME_AUDIO, filename, mtime, size, update);
}
bool SquawkDAO::exist_imagefile(std::string filename, long mtime, long size, bool update) {
  return exist(SQL_TABEL_NAME_IMAGES, filename, mtime, size, update);
}

bool SquawkDAO::exist(std::string table, std::string filename, long mtime, long size, bool update) {
  bool found = false;
  Sqlite3Statement * stmt = NULL;
  Sqlite3Statement * stmtUpdate = NULL;
  try {
    stmt = stmtMap[table];
    stmt->bind_text(1, filename);
    stmt->bind_int(2, size);
    stmt->bind_int(3, mtime);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
            int result = stmt->get_int(0);
	    found = true;
	    if(update) {
	      stmtUpdate = stmtMap[std::string("update_") + table];
	      stmtUpdate->bind_int(1, time(0));
	      stmtUpdate->bind_int(2, result);
	      int step_status = stmtUpdate->step();
	      
	      if(step_status != SQLITE_DONE)
		throw step_status; //TODO SQLException
	      stmtUpdate->reset();
//	      db->release_statement(stmtUpdate);
	    }
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
//    db->release_statement(stmt);
  } catch(DaoException * e) {
    LOG4CXX_FATAL(logger, "Can not save audiofile, Exception:" << e->code() << "-> " << e->what());
//    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return found;
}
void SquawkDAO::save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::model::Song * audiofile) {
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("insert into tbl_cds_audiofiles(" 
      "filename, filesize, mtime, timestamp, album_id, title, bitrate, sample_rate, bits_per_sample, channels, track, mime_type, length, disc) values (?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    stmt->bind_text(1, filename);
    stmt->bind_int(2, size);
    stmt->bind_int(3, mtime);
    stmt->bind_int(4, std::time(0));
    stmt->bind_int(5, album_id);
    stmt->bind_text(6, audiofile->title);
    stmt->bind_int(7, audiofile->bitrate);
    stmt->bind_int(8, audiofile->samplerate);
    stmt->bind_int(9, audiofile->bits_per_sample);
    stmt->bind_int(10, audiofile->channels);
    stmt->bind_int(11, audiofile->track);
    stmt->bind_text(12, audiofile->mime_type);
    stmt->bind_int(13, audiofile->playLength);
    stmt->bind_int(14, audiofile->disc);
    stmt->insert();
    stmt->reset();
    db->release_statement(stmt);
    
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not save audiofile, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
}
unsigned long SquawkDAO::save_imagefile(std::string filename, long mtime, long size, unsigned long album, squawk::model::Image * imagefile) {
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("insert into tbl_cds_images(album, filename, mtime, timestamp, filesize, type, mime_type, width, height) values (?,?,?,?,?,?,?,?,?)");
    stmt->bind_int(1, album);
    stmt->bind_text(2, filename);
    stmt->bind_int(3, mtime);
    stmt->bind_int(4, std::time(0));
    stmt->bind_int(5, size);
    stmt->bind_int(6, imagefile->type);
    stmt->bind_text(7, imagefile->mime_type);
    stmt->bind_int(8, imagefile->width);
    stmt->bind_int(9, imagefile->height);
    stmt->insert();
    stmt->reset();
    db->release_statement(stmt);

    return db->last_insert_rowid();

  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not save imagefile, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
}
unsigned long SquawkDAO::save_artist(squawk::model::Artist & artist) {
  Sqlite3Statement * stmt = NULL;
  try {

    stmt = db->prepare_statement("insert into tbl_cds_artists(name, clean_name, letter) values (?,?,?)");
    
    stmt->bind_text(1, artist.name);
    stmt->bind_text(2, artist.clean_name);
    stmt->bind_text(3, artist.letter);
    stmt->insert();
    stmt->reset();
    db->release_statement(stmt);
    
    return db->last_insert_rowid();
    
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not save artist, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
}
unsigned long SquawkDAO::save_album(std::string path, squawk::model::Album album) {
  Sqlite3Statement * stmt = NULL;
  try {

    stmt = db->prepare_statement("insert into tbl_cds_albums(path, name, genre, year) values (?,?,?,?)");
    stmt->bind_text(1, path);
    stmt->bind_text(2, album.name);
    stmt->bind_text(3, album.genre);
    stmt->bind_text(4, album.year);

    if(stmt->step() != SQLITE_DONE) {
      throw stmt->step();
    }
    stmt->reset();
    db->release_statement(stmt);

    int album_id = db->last_insert_rowid();

    //and save the artist mappings
    for(std::list< squawk::model::Artist >::iterator list_iter = album.artists.begin(); list_iter != album.artists.end(); list_iter++) {
	squawk::model::Artist artist = *list_iter;
	
	stmt = db->prepare_statement("insert into tbl_cds_artists_albums(album_id, artist_id) values (?,?)");
	stmt->bind_int(1, album_id);
	stmt->bind_int(2, artist.id);
    	stmt->insert();
	db->release_statement(stmt);
    }
    
    return album_id;
    
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not save album, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
}
  
std::list< squawk::model::Album > SquawkDAO::get_albums() { //TODO to slow
  std::list< squawk::model::Album > result;

  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  std::cout << "get albums list" << std::endl;

  Sqlite3Statement * stmt = NULL;
  Sqlite3Statement * stmt_artist = NULL;
  try {

      end = std::chrono::system_clock::now();
     std::cout << "get albums list: statement prepared: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;

    stmt = db->prepare_statement("select name, genre, year, ROWID from tbl_cds_albums");
    stmt_artist = db->prepare_statement(
                "select artist.ROWID, artist.name, artist.letter, artist.clean_name from tbl_cds_artists artist "
                "JOIN tbl_cds_artists_albums m ON artist.ROWID = m.artist_id "
                "where m.album_id=? ");

    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  
	  squawk::model::Album album;
	  album.name = stmt->get_string(0);
	  album.genre = stmt->get_string(1);
	  album.year = stmt->get_string(2);
	  album.id = stmt->get_int(3);

    //get the artists
      std::list< squawk::model::Artist > result_artist;
      int step_status_artist;
      stmt_artist->bind_int(1, album.id);

      do {
          step_status_artist = stmt_artist->step();
          if(step_status_artist == SQLITE_ROW) {
        squawk::model::Artist artist;
        artist.id = stmt_artist->get_int(0);
        artist.name = stmt_artist->get_string(1);
        artist.letter = stmt_artist->get_string(2);
        artist.clean_name = stmt_artist->get_string(3);

        result_artist.insert(result_artist.end(), artist);
          }
      } while(step_status_artist != SQLITE_DONE);
      stmt_artist->reset();
      album.artists = result_artist;

	  result.insert(result.end(), album);
        }
    } while(step_status != SQLITE_DONE);

    end = std::chrono::system_clock::now();
    std::cout << "get albums list: end: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;

    stmt->reset();
    db->release_statement(stmt_artist);
    db->release_statement(stmt);

  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get albums, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return result;
}
  squawk::model::Album SquawkDAO::get_album(std::string path) {
  squawk::model::Album album;
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select album.name, album.genre, album.year, album.ROWID from tbl_cds_albums album where album.path = ?");
    stmt->bind_text(1, path);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  album.name = stmt->get_string(0);
	  album.genre = stmt->get_string(1);
	  album.year = stmt->get_string(2);
	  album.id = stmt->get_int(3);
	}
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get album by path, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return album;
}
squawk::model::Album SquawkDAO::get_album(unsigned long id) {
  squawk::model::Album album;
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select album.name, album.genre, album.year, album.ROWID, artist.name from tbl_cds_albums album, tbl_cds_artists artist, tbl_cds_artists_albums m "
      "where album.ROWID = ? and artist.ROWID = m.artist_id and album.ROWID = album_id");
    stmt->bind_int(1, id);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  album.name = stmt->get_string(0);
	  album.genre = stmt->get_string(1);
	  album.year = stmt->get_string(2);
	  album.id = stmt->get_int(3);
      album.artists = get_artists_by_album(album.id);
	}
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get album by id, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return album;
}
std::list< squawk::model::Album > SquawkDAO::getAlbumsByArtist(unsigned long id) {
  std::list< squawk::model::Album > result;
  
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement(
      "select album.name, album.genre, album.year, album.ROWID, artist.name from tbl_cds_albums album, tbl_cds_artists_albums m, tbl_cds_artists artist "
	"where artist.ROWID = ? and album.ROWID = m.album_id and artist.ROWID = m.artist_id");
    stmt->bind_int(1, id);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  squawk::model::Album album;
	  album.name = stmt->get_string(0);
	  album.genre = stmt->get_string(1);
	  album.year = stmt->get_string(2);
	  album.id = stmt->get_int(3);

	  album.artists = get_artists_by_album(album.id);

	  result.insert(result.end(), album);
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get album by id, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return result;
}
std::list< squawk::model::Song > SquawkDAO::getSongsByAlbum(unsigned long id) {
  std::list< squawk::model::Song > result;
  
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select songs.ROWID, songs.title, songs.track, songs.filename, songs.length, songs.bitrate, songs.sample_rate, songs.bits_per_sample, songs.channels, songs.mime_type, songs.disc "
      "from tbl_cds_audiofiles songs, tbl_cds_albums album where album.ROWID = ? and album.ROWID = songs.album_id order by songs.track, songs.disc");
    stmt->bind_int(1, id);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  squawk::model::Song song;
	  song.id= stmt->get_int(0);
	  song.title = stmt->get_string(1);
	  song.track = stmt->get_int(2);
	  song.filename = stmt->get_string(3);
	  song.playLength = stmt->get_int(4);
	  song.bitrate = stmt->get_int(5);
	  song.samplerate = stmt->get_int(6);
	  song.bits_per_sample = stmt->get_int(7);
	  song.channels = stmt->get_int(8);
	  song.mime_type = stmt->get_string(9);
	  song.disc = stmt->get_int(10);
	  
	  result.insert(result.end(), song);
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get album by id, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return result;
}
squawk::model::Song SquawkDAO::getSongById(unsigned long id) {
  squawk::model::Song song;

  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select songs.ROWID, songs.title, songs.track, songs.filename from tbl_cds_audiofiles songs where songs.ROWID = ?");
    stmt->bind_int(1, id);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  song.id= stmt->get_int(0);
	  song.title = stmt->get_string(1);
	  song.track = stmt->get_int(2);
	  song.filename = stmt->get_string(3);
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get song by id, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return song;  
}
std::list<squawk::model::Artist>  SquawkDAO::get_artists_by_album(unsigned long id) {
  std::list< squawk::model::Artist > result;
  
  Sqlite3Statement * stmt_artist = NULL;
  try {
    stmt_artist = db->prepare_statement("select artist.ROWID, artist.name, artist.letter, artist.clean_name from tbl_cds_artists artist, tbl_cds_albums album, tbl_cds_artists_albums m "
				 "where album.ROWID = ? and album.ROWID = m.album_id and artist.ROWID = m.artist_id ");
    stmt_artist->bind_int(1, id);

    int step_status;
    do {
        step_status = stmt_artist->step();
        if(step_status == SQLITE_ROW) {
	  squawk::model::Artist artist;
      artist.id = stmt_artist->get_int(0);
      artist.name = stmt_artist->get_string(1);
      artist.letter = stmt_artist->get_string(2);
      artist.clean_name = stmt_artist->get_string(3);
	  
	  result.insert(result.end(), artist);
        }
    } while(step_status != SQLITE_DONE);

    stmt_artist->reset();
    db->release_statement(stmt_artist);

  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get song by id, Exception:" << e.code() << "-> " << e.what());
    if(stmt_artist != NULL) db->release_statement(stmt_artist);
    throw;
  }
  return result;  
}
std::list<squawk::model::Artist> SquawkDAO::get_artists() {
  std::list< squawk::model::Artist > result;
  
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select ROWID, name, letter, clean_name from tbl_cds_artists order by name");
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  squawk::model::Artist artist;
	  artist.id = stmt->get_int(0);
	  artist.name = stmt->get_string(1);
	  artist.letter = stmt->get_string(2);
	  artist.clean_name = stmt->get_string(3);
	  
	  result.insert(result.end(), artist);
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get artists, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return result;
}

unsigned long SquawkDAO::getArtistId(std::string artist) {
  unsigned long id = 0;
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select ROWID from tbl_cds_artists where clean_name = ?");
    stmt->bind_text(1, artist);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  id = stmt->get_int(0);
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get artist id, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return id;
}

unsigned long SquawkDAO::getAlbumId(std::string path) {
  unsigned long id = 0;
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select ROWID from tbl_cds_albums where path = ?");
    stmt->bind_text(1, path);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  id = stmt->get_int(0);
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get album id, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return id;
}
squawk::model::Image SquawkDAO::getImageById(unsigned long id) {
  squawk::model::Image result;
  
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select ROWID, filename, mime_Type, width, height from tbl_cds_images where ROWID = ?");
    stmt->bind_int(1, id);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  result.id = stmt->get_int(0);
	  result.filename = stmt->get_string(1);
	  result.mime_type = stmt->get_string(2);
	  result.width = stmt->get_int(3);
	  result.height = stmt->get_int(4);
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get image by id, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return result;
}
squawk::model::Image SquawkDAO::getFrontImage(unsigned long id) {
  squawk::model::Image result;
  
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select ROWID, filename, mime_Type, width, height from tbl_cds_images where album = ? and type = 0");
    stmt->bind_int(1, id);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  result.id = stmt->get_int(0);
	  result.filename = stmt->get_string(1);
	  result.mime_type = stmt->get_string(2);
	  result.width = stmt->get_int(3);
	  result.height = stmt->get_int(4);
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get front image, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return result;
}
std::list< squawk::model::Image > SquawkDAO::get_images_by_album(unsigned long id) {
  std::list< squawk::model::Image > result;
  
  Sqlite3Statement * stmt = NULL;
  try {
    stmt = db->prepare_statement("select ROWID, filename, mime_Type, width, height from tbl_cds_images where album = ? order by type asc");
    stmt->bind_int(1, id);
    int step_status;
    do {
        step_status = stmt->step();
        if(step_status == SQLITE_ROW) {
	  squawk::model::Image image;
	  image.id = stmt->get_int(0);
	  image.filename = stmt->get_string(1);
	  image.mime_type = stmt->get_string(2);
	  image.width = stmt->get_int(3);
	  image.height = stmt->get_int(4);
	  result.insert(result.end(), image);
        }
    } while(step_status != SQLITE_DONE);
    stmt->reset();
    db->release_statement(stmt);
  } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not get images by album, Exception:" << e.code() << "-> " << e.what());
    if(stmt != NULL) db->release_statement(stmt);
    throw;
  }
  return result;
}

bool SquawkDAO::exist_table(std::string table_name) {
 LOG4CXX_DEBUG(logger, "exist table:"<< table_name)
 bool exist = false;
  Sqlite3Statement * statement;
  try {
    statement = db->prepare_statement("SELECT count(*) FROM sqlite_master WHERE name='"+table_name+"'");
//    int step_status;
//    do {
//      step_status = statement->step();
//      if(step_status == SQLITE_ROW) {
    while( statement->step() ){
	  int result = statement->get_int(0);
	  if(result == 1) { 
	    exist = true;
	  } else if(result > 1) {
	      LOG4CXX_WARN(logger, "found to many rows:" << result)
    //  }
      }
  } // while(step_status != SQLITE_DONE);
  statement->reset();
  db->release_statement(statement);
 } catch(DaoException & e) {
    LOG4CXX_FATAL(logger, "Can not test table, Exception:" << e.code() << "-> " << e.what());
    if(statement != NULL) db->release_statement(statement);
    throw;
 }
 return exist;
}
void SquawkDAO::create_table(std::string query) {
    LOG4CXX_DEBUG(logger, "create table:"<< query)
    Sqlite3Statement * stmt_table_create;
    try {
      stmt_table_create = db->prepare_statement(query);
      if( stmt_table_create->step() ) {
          throw new DaoException(-1, "could not create table");
      }
    } catch(DaoException * e) {
      LOG4CXX_FATAL(logger, "create table, Exception:" << e->code() << "-> " << e->what());
      if(stmt_table_create != NULL) db->release_statement(stmt_table_create);
      throw;
    }
    stmt_table_create->reset();
    db->release_statement(stmt_table_create);
}
}}

