/*
    header file for the media database access object.
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

#ifndef MEDIADAO_H
#define MEDIADAO_H

#include "log4cxx/logger.h"
#include "../db/sqlite3database.h" //TODO include generic header
#include "../db/database.h"

#include "image.h"

namespace squawk {
namespace media {


struct file_item {
  enum IMAGE_TYPE { COVER, OTHER, FOLDER } type;
  file_item(std::string name, std::string mime_type, unsigned long mtime, unsigned long size) : name(name), mime_type(mime_type), mtime(mtime), size(size) {};
  std::string name, mime_type;
  unsigned long mtime, size;
};

class MediaDao {
public:
    MediaDao(squawk::db::Sqlite3Database * db);
    ~MediaDao();
    void start_transaction();
    void end_transaction();
    bool exist_audiofile(std::string filename, long mtime, long size, bool update);
    bool exist_videofile(std::string filename, long mtime, long size, bool update);
    bool exist_imagefile(std::string filename, long mtime, long size, bool update);

    unsigned long getOrCreateDirectory(const std::string & path, const std::string & name, const unsigned long & parent, const int & type );
    unsigned long saveFile(const file_item & file, const unsigned long & parent, commons::image::Image * imagefile);

    squawk::media::Album get_album(std::string path);
    unsigned long save_album(std::string path, squawk::media::Album * album);
    unsigned long save_artist(squawk::media::Artist * artist);
    void save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::media::Song * song);
    unsigned long createDirectory(const std::string path );
    unsigned long save_imagefile(const file_item & file, const unsigned long & album, commons::image::Image * imagefile);
    unsigned long save_videofile(std::string filename, long mtime, long size, std::string mime_type);
    void sweep( long mtime );

private:
    static log4cxx::LoggerPtr logger;
    squawk::db::Sqlite3Database * db;
    std::map<std::string, squawk::db::Sqlite3Statement *> stmtMap;
    bool exist(std::string table, std::string filename, long mtime, long size, bool update);
    const std::string create_statements[10] {
        "create table if not exists tbl_cds_audiofiles(album_id, filename NOT NULL, filesize, mtime, title, track, timestamp, mime_type, bitrate, sample_rate, bits_per_sample, channels, length, disc);",
        "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexFilename ON tbl_cds_audiofiles (filename)",
        "create table if not exists tbl_cds_artists_albums(album_id, artist_id, role);",
        "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexMapping ON tbl_cds_artists_albums (album_id, artist_id)",
        "create table if not exists tbl_cds_artists(name, clean_name, letter);",
        "create table if not exists tbl_cds_albums(path, name, genre, year, clean_name, letter);",
        "create table if not exists tbl_cds_images(album, filename, mtime, timestamp, filesize, type, mime_type, width, height);",
        "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexImagesFilename ON tbl_cds_images (filename)",
        "create table if not exists tbl_cds_movies(name, parent, type, filename, mtime, timestamp, filesize, mime_type);",
        "create table if not exists tbl_cds_files(name, parent, type, filename, mtime, timestamp, filesize, mime_type, width, height, color);"
    };
};
}}
#endif // MEDIADAO_H
