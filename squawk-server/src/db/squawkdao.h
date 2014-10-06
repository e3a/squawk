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

#ifndef SQUAWKDAO_H
#define SQUAWKDAO_H

#include <string>
#include <list>
#include <map>

#include "squawk.h"
#include "../squawkconfig.h"
#include "sqlite3database.h"

#include "sqlite3.h"

#include "log4cxx/logger.h"

#define SQL_TABEL_NAME_AUDIO "tbl_cds_audiofiles"
#define SQL_TABEL_NAME_ARTISTS_ALBUMS "tbl_cds_artists"
#define SQL_TABEL_NAME_ARTISTS "tbl_cds_artists"
#define SQL_TABEL_NAME_ALBUMS "tbl_cds_albums"
#define SQL_TABEL_NAME_IMAGES "tbl_cds_images"

namespace squawk {
namespace db {

/**
 * The Database access object for SQLLite3.
 */
class SquawkDAO {
public:

  SquawkDAO(squawk::SquawkConfig * config);
  SquawkDAO() {};
  virtual ~SquawkDAO();

  void start_transaction();
  void end_transaction();
  
  bool exist_audiofile(std::string filename, long mtime, long size, bool update);
  bool exist_imagefile(std::string filename, long mtime, long size, bool update);
  std::list< squawk::model::Album > get_albums();
  std::list< squawk::model::Album > getAlbumsByArtist(unsigned long id);
  squawk::model::Album get_album(unsigned long id);
  squawk::model::Album get_album(std::string path);
  std::list< squawk::model::Image > get_images_by_album(unsigned long id);

  void save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::model::Song * song);
  unsigned long save_imagefile(std::string filename, long mtime, long size, unsigned long album_id, squawk::model::Image * imagefile);
  
  /**
   * Save the artist.
   * \param artist the artist to save
   */
  unsigned long save_artist(squawk::model::Artist & artist);
  
  /**
   * Save the album and create the artist mappings.
   * \param path the file path 
   * \param album the album to save
   * \return the new album id
   */
  unsigned long save_album(std::string path, squawk::model::Album album);

  unsigned long getArtistId(std::string artist);
  std::list<squawk::model::Artist> get_artists();
  std::list<squawk::model::Artist> get_artists_by_album(unsigned long id);
  unsigned long getAlbumId(std::string path);
  /**
   * Get the Songs by album id.
   */
  std::list< squawk::model::Song > getSongsByAlbum(unsigned long id);
  /**
   * Get the Song id.
   */
  squawk::model::Song getSongById(unsigned long id);
  /**
   * Get the Image by id.
   */
  squawk::model::Image getImageById(unsigned long id);
  /**
   * Get the Front Image by album id.
   */
  squawk::model::Image getFrontImage(unsigned long id);
  
private:
  static log4cxx::LoggerPtr logger;
  Sqlite3Database * db;
  /**
   * Test if a table exist.
   * \param table_name the name of the table to check.
   * \return true if the table exist
   */
  bool exist_table(std::string table_name);
  /**
   * Exexute a create table statement.
   * \param table_name the statement.
   */
  void create_table(std::string query);
  /**
   * Test if a record already exists.
   * \param table the database table
   * \param filename the filename 
   * \param mtime the last access time
   * \param size the filesize
   * \param touch update the timestamp
   */
  bool exist(std::string table, std::string filename, long mtime, long size, bool touch);

  std::map<std::string, Sqlite3Statement *> stmtMap;
};
}}
#endif // SQUAWKDAO_H
