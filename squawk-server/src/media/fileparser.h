/*
    file parser definition.
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

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <map>
#include <memory>

#include <squawk.h>
#include <media.h>
#include "mediadao.h"

#include "sys/stat.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace media {

class FileParser {
public:
	FileParser ( const std::string & database_file, const std::string & tmp_directory ) :
		mediaDao ( std::unique_ptr<MediaDao> ( new MediaDao ( database_file ) ) ), tmp_directory_ ( tmp_directory ) {}

	FileParser ( const FileParser& ) = delete;
	FileParser ( FileParser&& ) = delete;
	FileParser& operator= ( const FileParser& ) = delete;
	FileParser& operator= ( FileParser&& ) = delete;
	~FileParser() { }

	enum FILE_TYPE {MP3, OGG, FLAC, MUSEPACK, MONKEY_AUDIO, IMAGE, AUDIOFILE, IMAGEFILE, VIDEOFILE, EBOOK, UNKNOWN};
	enum DIRECTORY_TYPE { MUSIC, IMAGES, MOVIES, NONE };

	void parse ( std::vector< std::string > paths );

private:
	static log4cxx::LoggerPtr logger;
	std::unique_ptr< MediaDao > mediaDao;
	const std::string tmp_directory_;

	std::map<std::string, int> statistic;
	DIRECTORY_TYPE _parse ( const unsigned long & path_id, const std::string & basepath, const std::string & path );

	static std::string get_mime_type ( const std::string & filename );
    void parseImage( Image & image, const std::string & prefix, int image_id );
};
}
}
#endif // FILEPARSER_H
