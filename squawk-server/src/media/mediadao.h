/*
    media dao definition.
    Copyright (C) 2014  <e.knecht@netwings.ch>

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
#include "didl.h"
#include "album.h"
#include "image.h"
#include "media.h"
#include "song.h"

namespace squawk {
namespace media {

/**
 * @brief The file_item struct
 */
struct FileItem {
    // enum IMAGE_TYPE { COVER, OTHER, FOLDER } type;
	FileItem ( const std::string & name, const std::string & mime_type, const unsigned long & mtime, const unsigned long & size ) :
        type ( OTHER ), name ( name ), mime_type ( mime_type ), mtime ( mtime ), size ( size ) {}
    FILE_TYPE type;
	std::string name, mime_type;
	unsigned long mtime, size;
};

/**
 * @brief The MediaDao class
 */
class MediaDao {
public:
	explicit MediaDao ( const std::string & database_file );
	MediaDao ( const MediaDao& ) = delete;
	MediaDao ( MediaDao&& ) = delete;
	MediaDao& operator= ( const MediaDao& ) = delete;
	MediaDao& operator= ( MediaDao&& ) = delete;
	~MediaDao() {}

	/**
	 * @brief start database transaction
	 */
	void startTransaction();
	/**
	 * @brief end database transaction
	 */
	void endTransaction();


    std::list< didl::DidlContainer > search( const int & parent, const int & start_index, const int & result_count );

    /**
	 * @brief test if file exist
	 * @param filename the filename
	 * @param mtime the last access date
	 * @param size the file size
	 * @param update update the mtime with timestamp
	 * @return
	 */
	bool exist ( const std::string & filename, const long & mtime, long const & size, const bool & update );
	/**
	 * @brief save image
	 * @param parent
	 * @param file
	 * @param imagefile
	 * @return
	 */
	unsigned long saveFile ( const unsigned long & parent, const FileItem & file, Image & imagefile );
	/**
	 * @brief save cover
	 * @param path_id
	 * @param file
	 * @param album
	 * @param imagefile
	 * @return
	 */
	unsigned long saveFile ( const unsigned long & path_id, const FileItem & file, const unsigned long & album, Image & imagefile );
	/**
	 * @brief save video
	 * @param parent
	 * @param file
	 * @param media_file
	 * @return
	 */
    size_t save_video( const size_t & parent, const FileItem & file, commons::media::MediaFile & media_file );
	/**
	 * @brief save audiofile
	 * @param path_id
	 * @param filename
	 * @param mtime
	 * @param size
	 * @param album_id
	 * @param song
	 */
	void saveFile ( const unsigned long & path_id, const std::string & filename, const long & mtime, const long & size, const unsigned long & album_id, const Song & song );
    /**
     * @brief save book
     * @param path_id
     * @param filename
     * @param mtime
     * @param size
     * @param isbn
     */
    void saveFile ( const unsigned long & parent, const FileItem & file, const std::string & isbn );
    /**
	 * @brief save album
	 * @param album
	 * @return
	 */
	unsigned long save ( Album & album );
	/**
	 * @brief save artist
	 * @param artist
	 * @return
	 */
	unsigned long save ( Artist & artist );
	/**
	 * @brief save directory
	 * @param parent
	 * @param name
	 * @param path
	 * @return
	 */
	unsigned long save ( const unsigned long parent, const std::string & name, const std::string & path );
	/**
	 * @brief get album by path
	 * @param path
	 * @return
	 */
	Album getAlbum ( const std::string & path );
	/**
	 * @brief sweep all
	 * @param mtime
	 */
	void sweep ( long mtime );

private:
	static log4cxx::LoggerPtr logger;
    db::db_connection_ptr db_;
};
} // media
} // squawk
#endif // MEDIADAO_H
