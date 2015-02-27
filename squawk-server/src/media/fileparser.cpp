/*
    Parse Files and insert to Database
    
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

#include "fileparser.h"

#include <chrono>
#include <sstream>
#include <ctime>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>

#include "http.h"
#include "commons.h"
#include "image.h"
#include "media.h"

// #include "Imlib2.h"

namespace squawk {
namespace media {

log4cxx::LoggerPtr FileParser::logger(log4cxx::Logger::getLogger("squawk.media.FileParser"));
pcrecpp::RE FileParser::re("(.*)/CD[\\d+]");

void FileParser::parse( std::vector< std::string > paths ) {

    long start_time = std::time( 0 );
    statistic.clear();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    mediaDao->start_transaction();

    for( auto & path : paths ) {
        if( commons::filesystem::is_directory(path) ) {
            if( squawk::DEBUG ) LOG4CXX_DEBUG(logger, "import files:" << path)
            _parse(path,path);
        } else {
            LOG4CXX_WARN(logger, path << "is not a directory.")
        }
    }

    //output statistic
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    mediaDao->sweep( start_time );

    std::stringstream ss;
    int sum = 0;
    ss << std::endl << "***************************************************" << std::endl;

    for(std::map< std::string, int >::iterator iter = statistic.begin(); iter != statistic.end(); ++iter) {
        ss << iter->first << "\t" << iter->second << std::endl;
        sum += iter->second;
    }
    ss << "Total:\t" << sum << std::endl;
    ss << "Time:\t" << elapsed_seconds.count() << std::endl;
    LOG4CXX_DEBUG(logger, ss.str())
    mediaDao->end_transaction();

}

FileParser::DIRECTORY_TYPE FileParser::_parse(const std::string & basepath, const std::string & path) {

    std::map< FILE_TYPE, std::list< file_item > > files;
    // list< string > directories;
    DIRECTORY_TYPE dir_type;

    //search for the files and directories
    struct dirent *entry;
    DIR *dp;

    dp = opendir(path.c_str());
    if (dp == NULL) {
        LOG4CXX_ERROR(logger, "opendir: Path does not exist or could not be read." + std::string(path))
        return FileParser::NONE;
    }

    while ((entry = readdir(dp))) {
        if(std::string(entry->d_name) != std::string(".") && std::string(entry->d_name) != std::string("..") ) {
            std::string name = path + std::string("/") + std::string(entry->d_name);
            struct stat s;
            if(stat(name.c_str(), &s) == 0) {
                if( s.st_mode & S_IFREG ) {

                    std::string type = get_mime_type(name);

                    int count = 0;
                    if(statistic.find(type) != statistic.end())
                        count = statistic.find(type)->second;
                    statistic[type] = ++count;
                    //store file in the list
                    if( commons::string::starts_with(type, "audio/") ) {
                        files[AUDIOFILE].insert(files[AUDIOFILE].end(), file_item(name, type, s.st_mtim.tv_sec, s.st_size));
                    } else if( commons::string::starts_with(type, "image/") ) {
                        files[IMAGEFILE].insert(files[IMAGEFILE].end(), file_item(name, type, s.st_mtim.tv_sec, s.st_size));
                    } else if( commons::string::starts_with(type, "video/") ) {
                        files[VIDEOFILE].insert(files[VIDEOFILE].end(), file_item(name, type, s.st_mtim.tv_sec, s.st_size));
                    } else {
                        LOG4CXX_WARN(logger, "unknown file type:" << type << ":" << name)
                    }

                } else if ( s.st_mode & S_IFDIR ) {
                    dir_type = _parse(basepath, std::string(path + "/" + std::string(entry->d_name)));
                } else {
                    LOG4CXX_ERROR(logger, "path is neither a reqular file nor a directory:" << path)
                }
            }
        }
    }
    closedir(dp);

    squawk::media::Album album;
    if(files.find( AUDIOFILE ) != files.end()) {
        dir_type = MUSIC;

        for(std::list< file_item >::iterator list_iter = files[ AUDIOFILE ].begin(); list_iter != files[ AUDIOFILE ].end(); list_iter++) {

            if(! mediaDao->exist_audiofile( (*list_iter).name, (*list_iter).mtime, (*list_iter).size, true ) ) {

                //parse the mediafile
                commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( (*list_iter).name);
                if( media_file.getAudioStreams().size() > 1 ) {
                    LOG4CXX_WARN(logger, "more then one audio streams (" << media_file.getAudioStreams().size() << ") found in " << (*list_iter).name )
                }

                if( ! album.equals( media_file.getTag( commons::media::MediaFile::ALBUM ) ) ) {

                    album = squawk::media::Album( media_file.getTag( commons::media::MediaFile::ALBUM ),
                                                  media_file.getTag( commons::media::MediaFile::GENRE ),
                                                  media_file.getTag( commons::media::MediaFile::YEAR ) );

                    if( media_file.hasTag( commons::media::MediaFile::ARTIST ) ) {
                        squawk::media::Artist * artist = new Artist( media_file.getTag( commons::media::MediaFile::ARTIST ) ); //TODO remove new
                        if( album.add( artist ) ) {
                            unsigned long new_artist_id = mediaDao->save_artist( artist );
                            artist->id( new_artist_id );
                        } else delete artist;
                    }
                    if( media_file.hasTag( commons::media::MediaFile::COMPOSER ) ) {
                        squawk::media::Artist * artist = new Artist( media_file.getTag( commons::media::MediaFile::COMPOSER ) ); //TODO remove new
                        if( album.add( artist ) ) {
                            unsigned long new_artist_id = mediaDao->save_artist( artist );
                            artist->id( new_artist_id );
                        } else delete artist;
                    }
                    album.id = mediaDao->save_album(get_album_clean_path(path), &album);
                }
                squawk::media::Song song(media_file.getTag( commons::media::MediaFile::TITLE ),
                                         (*list_iter).mime_type, (*list_iter).name, (*list_iter).mtime,
                                         media_file.getAudioStreams()[0].bitrate(),
                                         (*list_iter).size,
                                         media_file.getAudioStreams()[0].sampleFrequency(),
                                         media_file.duration(),
                                         commons::string::parse_string<int>( media_file.getTag( commons::media::MediaFile::TRACK ) ),
                                         commons::string::parse_string<int>( media_file.getTag( commons::media::MediaFile::DISC ) ),
                                         media_file.getAudioStreams()[0].channels(),
                                         media_file.getAudioStreams()[0].bitsPerSample(),
                                         album.artists);
                mediaDao->save_audiofile((*list_iter).name, (*list_iter).mtime, (*list_iter).size, album.id, &song);
            }
        }
        files.erase(AUDIOFILE);
    }

    if(files.find( IMAGEFILE ) != files.end()) {
        if(dir_type == MUSIC) {
            for(std::list< file_item >::iterator list_iter = files[IMAGEFILE].begin(); list_iter != files[IMAGEFILE].end(); list_iter++) {

                if(! mediaDao->exist_imagefile((*list_iter).name, (*list_iter).mtime, (*list_iter).size, true)) {
                    if(album.id == 0) {
                        std::string cleanPath = get_album_clean_path( path );
                        album = mediaDao->get_album(cleanPath);
                    }
                    (*list_iter).type = ((*list_iter).name.find("front") == std::string::npos && (*list_iter).name.find("cover") == std::string::npos ?
                                         squawk::media::file_item::OTHER : squawk::media::file_item::COVER);

                    commons::image::Image image( (*list_iter).name );
                    int image_id = mediaDao->save_imagefile((*list_iter), album.id, &image);

                    std::stringstream image_stream;
                    image_stream << squawk_config->tmpDirectory() << "/image-" << image_id << ".jpg";

                    //TODO image.scale(1000, 1000, image_stream.str());
                    if( (*list_iter).type == squawk::media::file_item::COVER ) {
                        std::stringstream cover_stream;
                        cover_stream << squawk_config->tmpDirectory() << "/" << album.id << ".jpg";
                        std::string cover_filename = cover_stream.str();
                        image.scale(150, 150, cover_filename);
                    }
                }
            }
        } else {
            if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "found images " << files[IMAGEFILE].size() << ":" << path)
            //check if file exists
                    //create directory
                    std::string relative_path = path.substr( basepath.size() );
                    std::cout << "image path:" << relative_path << std::endl;
                    unsigned long path_id = mediaDao->createDirectory( relative_path );
                    //save file
                    for(std::list< file_item >::iterator list_iter = files[IMAGEFILE].begin(); list_iter != files[IMAGEFILE].end(); list_iter++) {
                        commons::image::Image image( (*list_iter).name );
                        unsigned long id = mediaDao->saveFile((*list_iter), path_id, &image);

                        //create tumbs
                        image.scale(150, 150, squawk_config->tmpDirectory() + "/images" + "/cover" + commons::string::to_string<unsigned long>(id) + ".jpg" );
                        // image.scale(1000, 1000, squawk_config->tmpDirectory() + "/images" + "/" + commons::string::to_string<unsigned long>(id) + ".jpg");
                    }
        }
        files.erase(IMAGEFILE);
    }

    if(files.find( VIDEOFILE ) != files.end()) {
        std::string relative_path = path.substr( basepath.size() );
        std::cout << "image path:" << relative_path << std::endl;
        unsigned long path_id = mediaDao->createDirectory( relative_path );
        //save file
        for(auto & video : files[VIDEOFILE] ) {
            if(! mediaDao->exist_videofile( video.name, video.mtime, video.size, true ) ) {

                commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( video.name );
                if(squawk::DEBUG) LOG4CXX_DEBUG(logger, "save video" << video.name )
                mediaDao->saveVideo( video, path_id, media_file );

                //create tumbs
                //TODO
            }
        }
    }
    return dir_type;
}
// TODO remove
std::string FileParser::get_artist_clean_name(std::string artist) {
    return commons::string::trim(commons::string::to_lower(artist));
}
std::string FileParser::get_artist_letter(std::string artist) {
    if(artist.length()>0) {
        return artist.substr(0, 1);
    } else {
        LOG4CXX_WARN(logger, "empty artist name:" << artist)
        return std::string("");
    }
}
std::string FileParser::get_album_clean_path(std::string path) {
    string clean_path;
    if(re.PartialMatch(path, &clean_path)) {
        return commons::string::trim(clean_path);
    } else {
        return commons::string::trim(path);
    }
}
std::string FileParser::get_mime_type(const std::string & filename) {
    if( commons::string::ends_with(filename, ".flac", true ) ) {
        return "audio/x-flac";
    } else if( commons::string::ends_with(filename, ".mp3", true) ) {
        return "audio/mpeg";
    } else if( commons::string::ends_with(filename, ".ogg", true) ) {
        return "audio/vorbis";
    } else if( commons::string::ends_with(filename, ".mpc", true) ) {
        return "audio/x-musepack";
    } else if( commons::string::ends_with(filename, ".aac", true) ) {
        return "audio/aac";
    } else if( commons::string::ends_with(filename, ".ape", true) ) {
        return "audio/x-ape";
    } else if( commons::string::ends_with(filename, ".jpg", true) || commons::string::ends_with(filename, ".jpeg", true) ) {
        return "image/jpeg";
    } else if( commons::string::ends_with(filename, ".gif", true) ) {
        return "image/gif";
    } else if( commons::string::ends_with(filename, ".png", true) ) {
        return "image/png";
    } else if( commons::string::ends_with(filename, ".pdf", true) ) {
        return "application/pdf";
    } else if( commons::string::ends_with(filename, ".mp4", true) ) {
        return "video/mp4";
    } else if( commons::string::ends_with(filename, ".mkv", true) ) {
        return "video/x-matroska";
    } else if( commons::string::ends_with(filename, ".avi", true) ) {
        return "video/avi";
    } else if( commons::string::ends_with(filename, ".cue", true) ||
               commons::string::ends_with(filename, ".m3u", true) ||
               commons::string::ends_with(filename, ".txt", true) ) {
        return "text/plain";
    } else return std::string("application/octet-stream");
}
}}
