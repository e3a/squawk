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

#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <map>

#include <squawk.h>
#include "../squawkconfig.h"
#include "libavcpp.h"
#include "flacparser.h"
#include "mediadao.h"

#include "pcrecpp.h" 
#include "sys/stat.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace media {

class FileParser {
  public:
    FileParser(squawk::db::Sqlite3Database * db, SquawkConfig * squawk_config) : mediaDao(new MediaDao(db)), squawk_config(squawk_config) {
        parsers.insert( parsers.end(), new squawk::media::FlacParser() );
        parsers.insert( parsers.end(), new squawk::media::LibAVcpp() );
        mkdir( ( squawk_config->string_value(CONFIG_TMP_DIRECTORY) + "/audio" ).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
        mkdir( ( squawk_config->string_value(CONFIG_TMP_DIRECTORY) + "/images" ).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
        mkdir( ( squawk_config->string_value(CONFIG_TMP_DIRECTORY) + "/video" ).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    };
    FileParser() {
        parsers.insert( parsers.end(), new squawk::media::FlacParser() );
        parsers.insert( parsers.end(), new squawk::media::LibAVcpp() );
    }
    ~FileParser() {
        for( auto * parser : parsers )
            delete parser;
    }
    enum FILE_TYPE {MP3, OGG, FLAC, MUSEPACK, MONKEY_AUDIO, IMAGE, AUDIOFILE, IMAGEFILE, VIDEOFILE, UNKNOWN};
    void parse(std::string path);

    enum DIRECTORY_TYPE { MUSIC, IMAGES, MOVIES, NONE };

    std::string get_artist_clean_name(std::string artist);
    std::string get_artist_letter(std::string artist);
    std::string get_album_clean_path(std::string path);
    
  private:
    static log4cxx::LoggerPtr logger;
    SquawkConfig * squawk_config;
    MediaDao * mediaDao;

    std::vector< MetadataParser* > parsers;

    std::map<std::string, int> statistic;
    DIRECTORY_TYPE _parse(const std::string & basepath, const std::string & path);
    
    std::string get_mime_type(const std::string & filename);
  
    static pcrecpp::RE re;
};
}}
#endif // FILEPARSER_H
