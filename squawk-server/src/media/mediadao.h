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
#include "../db/sqlite3connection.h"
#include "../db/sqlite3database.h"

#include "squawk.h"
#include "image.h"
#include "media.h"

namespace squawk {
namespace media {

/**
 * @brief The file_item struct
 */
struct file_item {
  enum IMAGE_TYPE { COVER, OTHER, FOLDER } type;
  file_item( const std::string & name, const std::string & mime_type, const unsigned long & mtime, const unsigned long & size) :
      name(name), mime_type(mime_type), mtime(mtime), size(size) {}
  std::string name, mime_type;
  unsigned long mtime, size;
};

class MediaDao {
public:
    MediaDao( squawk::db::db_connection_ptr db );
    ~MediaDao();
    void start_transaction();
    void end_transaction();
    bool exist_audiofile(std::string filename, long mtime, long size, bool update);
    bool exist_videofile(std::string filename, long mtime, long size, bool update);
    bool exist_imagefile(std::string filename, long mtime, long size, bool update);

    unsigned long getOrCreateDirectory( const std::string & path, const std::string & name, const unsigned long & parent, const int & type );
    unsigned long saveFile( const file_item & file, const unsigned long & parent, commons::image::Image * imagefile );
    unsigned long saveVideo( const file_item & file, const unsigned long & parent, commons::media::MediaFile & media_file );

    squawk::media::Album get_album(std::string path);
    unsigned long save_album(std::string path, squawk::media::Album * album);
    unsigned long save_artist(squawk::media::Artist * artist);
    void save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::media::Song * song);
    unsigned long createDirectory(const std::string path );
    unsigned long save_imagefile(const file_item & file, const unsigned long & album, commons::image::Image * imagefile);
    void sweep( long mtime );

private:
    static log4cxx::LoggerPtr logger;
    squawk::db::db_connection_ptr db;
    bool exist(const squawk::sql::TBL_NAMES & table_name, const std::string & filename, const long & mtime, const long & size, const bool & update);
};
} // media
} // squawk
#endif // MEDIADAO_H
