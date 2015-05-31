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
#include "../utils/pdfparser.h"

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
                    } else if( type == "application/pdf" ) {
                        files[EBOOK].insert(files[EBOOK].end(), file_item(name, type, s.st_mtim.tv_sec, s.st_size));
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

        for( auto & audiofile : files[ AUDIOFILE ] ) {

            if(! mediaDao->exist_audiofile( audiofile.name, audiofile.mtime, audiofile.size, true ) ) {

                //parse the mediafile
                commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( audiofile.name);
                if( media_file.getAudioStreams().size() > 1 ) {
                    LOG4CXX_WARN(logger, "more then one audio streams (" << media_file.getAudioStreams().size() << ") found in " << audiofile.name )
                }

                if( ! album.equals( media_file.getTag( commons::media::MediaFile::ALBUM ) ) ) {

                    album = squawk::media::Album( media_file.getTag( commons::media::MediaFile::ALBUM ),
                                                  media_file.getTag( commons::media::MediaFile::GENRE ),
                                                  media_file.getTag( commons::media::MediaFile::YEAR ) );

                    if( media_file.hasTag( commons::media::MediaFile::ARTIST ) ) {
                        LOG4CXX_WARN(logger, "artist: " << media_file.getTag( commons::media::MediaFile::ARTIST ) )
                        squawk::media::Artist * artist = new Artist( media_file.getTag( commons::media::MediaFile::ARTIST ) ); //TODO remove new
                        if( album.add( artist ) ) {
                            unsigned long new_artist_id = mediaDao->save_artist( artist );
                            artist->id( new_artist_id );
                        } else delete artist;
                    }
                    if( media_file.hasTag( commons::media::MediaFile::COMPOSER ) ) {
                        LOG4CXX_WARN(logger, "artist: " << media_file.getTag( commons::media::MediaFile::COMPOSER ) )
                        squawk::media::Artist * artist = new Artist( media_file.getTag( commons::media::MediaFile::COMPOSER ) ); //TODO remove new
                        if( album.add( artist ) ) {
                            unsigned long new_artist_id = mediaDao->save_artist( artist );
                            artist->id( new_artist_id );
                        } else delete artist;
                    }
                    album.id = mediaDao->save_album(get_album_clean_path(path), &album);
                }
                squawk::media::Song song(media_file.getTag( commons::media::MediaFile::TITLE ),
                                         audiofile.mime_type, audiofile.name, audiofile.mtime,
                                         media_file.getAudioStreams()[0].bitrate(),
                                         audiofile.size,
                                         media_file.getAudioStreams()[0].sampleFrequency(),
                                         media_file.duration(),
                                         commons::string::parse_string<int>( media_file.getTag( commons::media::MediaFile::TRACK ) ),
                                         commons::string::parse_string<int>( media_file.getTag( commons::media::MediaFile::DISC ) ),
                                         media_file.getAudioStreams()[0].channels(),
                                         media_file.getAudioStreams()[0].bitsPerSample(),
                                         album.artists);
                mediaDao->save_audiofile(audiofile.name, audiofile.mtime, audiofile.size, album.id, &song);
            }
        }
        files.erase(AUDIOFILE);
    }

    if(files.find( IMAGEFILE ) != files.end()) {
        if(dir_type == MUSIC) {
            for(file_item & file : files[IMAGEFILE] ) {

                if(! mediaDao->exist_imagefile(file.name, file.mtime, file.size, true)) {
                    if(album.id == 0) {
                        std::string cleanPath = get_album_clean_path( path );
                        album = mediaDao->get_album(cleanPath);
                    }
                    file.type = (file.name.find("front") == std::string::npos && file.name.find("cover") == std::string::npos ?
                                         squawk::media::file_item::OTHER : squawk::media::file_item::COVER);

                    commons::image::Image image( file.name );
                    int image_id = mediaDao->save_imagefile(file, album.id, &image);

                    std::stringstream image_stream;
                    image_stream << tmp_directory_ << "/image-" << image_id << ".jpg";
                    image.scale(400, 400, image_stream.str());

                    if( file.type == squawk::media::file_item::COVER ) {
                        std::stringstream cover_stream;
                        cover_stream << tmp_directory_ << "/" << album.id << ".jpg";
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
                    unsigned long path_id = mediaDao->createDirectory( relative_path );
                    //save file
                    for(std::list< file_item >::iterator list_iter = files[IMAGEFILE].begin(); list_iter != files[IMAGEFILE].end(); list_iter++) {
                        commons::image::Image image( (*list_iter).name );
                        unsigned long id = mediaDao->saveFile((*list_iter), path_id, &image);

                        //create tumbs
                        image.scale(150, 150, tmp_directory_ + "/images" + "/cover" + commons::string::to_string<unsigned long>(id) + ".jpg" );
                        // image.scale(1000, 1000, tmp_directory_ + "/images" + "/" + commons::string::to_string<unsigned long>(id) + ".jpg");
                    }
        }
        files.erase(IMAGEFILE);
    }

    if(files.find( VIDEOFILE ) != files.end()) {
        std::string relative_path = path.substr( basepath.size() );
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

    if(files.find( EBOOK ) != files.end()) {
        std::string relative_path = path.substr( basepath.size() );
        unsigned long path_id = mediaDao->createDirectory( relative_path );
        //save file
        for(auto & book : files[EBOOK] ) {
            std::string isbn = PdfParser::parsePdf( book.name );
            if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "found book:" << book.name << ", ISBN:" << isbn)
        }
    }

    return dir_type;
}
/* / TODO remove
void FileParser::get_artist_clean_name(std::string & artist) {
    boost::algorithm::to_lower( artist );
    boost::algorithm::trim( artist );
} */
std::string FileParser::get_artist_letter(const std::string & artist) {
    if(artist.length()>0) {
        return artist.substr(0, 1);
    } else {
        LOG4CXX_WARN(logger, "empty artist name:" << artist)
        return std::string("");
    }
}
std::string FileParser::get_album_clean_path(const std::string & path) {
    string clean_path;
    if(re.PartialMatch(path, &clean_path)) {
        return boost::algorithm::trim_copy(clean_path);
    } else {
        return boost::algorithm::trim_copy(path);
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
        return "video/x-msvideo";
    } else if( commons::string::ends_with(filename, ".cue", true) ||
               commons::string::ends_with(filename, ".m3u", true) ||
               commons::string::ends_with(filename, ".txt", true) ) {
        return "text/plain";
    } else return std::string("application/octet-stream");
}
} // media
} // squawk
