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

#include "Imlib2.h"

namespace squawk {
namespace media {

log4cxx::LoggerPtr FileParser::logger(log4cxx::Logger::getLogger("squawk.media.FileParser"));
pcrecpp::RE FileParser::re("(.*)/CD[\\d+]");

void FileParser::parse(std::string path) {
    LOG4CXX_DEBUG(logger, "import files:" << path)

    long start_time = std::time( 0 );
    statistic.clear();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    if( commons::filesystem::is_directory(path) ) {
        mediaDao->start_transaction();
        _parse(path);

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

    } else {
        LOG4CXX_WARN(logger, path << "is not a directory.")
    }
}

FileParser::DIRECTORY_TYPE FileParser::_parse(std::string path) {

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
                    } else if( type != std::string("application/pdf") && type != std::string("text/plain") ) {
                        LOG4CXX_WARN(logger, "unkonwon file type:" << type << ":" << name)
                    }

                } else if ( s.st_mode & S_IFDIR ) {
                    dir_type = _parse(std::string(path + "/" + std::string(entry->d_name)));
                } else {
                    LOG4CXX_ERROR(logger, "path is neither a reqular file nor a directory:" << path)
                }
            }
        }
    }
    closedir(dp);

    squawk::model::Album album;
    if(files.find( AUDIOFILE ) != files.end()) {
        dir_type = MUSIC;
        int album_id = 0;
        for(std::list< file_item >::iterator list_iter = files[ AUDIOFILE ].begin(); list_iter != files[ AUDIOFILE ].end(); list_iter++) {
            if(! mediaDao->exist_audiofile( (*list_iter).name, (*list_iter).mtime, (*list_iter).size, true ) ) {

                squawk::media::Audiofile audiofile;

                bool metadata_found = false;
                for( MetadataParser * parser : parsers ) {
                    if( parser->parse(audiofile, (*list_iter).mime_type, (*list_iter).name) ) {
                        metadata_found = true;
                        break;
                    }
                }
                if( ! metadata_found)
                    LOG4CXX_WARN(logger, "can not parse audiofile::" << (*list_iter).name)

                //collect the artists
                std::list< squawk::model::Artist > artists;
                for(std::list< std::string >::iterator artist_iter = audiofile.artist.begin(); artist_iter != audiofile.artist.end(); artist_iter++) {
                    bool found = false;
                    std::string clean_name = get_artist_clean_name((*artist_iter));
                    for(std::list< squawk::model::Artist >::iterator artist_list_iter = artists.begin(); artist_list_iter != artists.end(); artist_list_iter++) {
                        if((*artist_list_iter).clean_name == clean_name ) {
                            found = true;
                        }
                    }
                    if(! found  ) {
                        squawk::model::Artist artist((*artist_iter),  get_artist_letter((*artist_iter)), clean_name);
                        artist.id = mediaDao->save_artist(artist);
                        artists.insert(artists.end(), artist);
                    }
                }

                if(album.id == 0 || album.name != audiofile.album) {
                    std::string cleanPath = get_album_clean_path(path);

                    album.artists = artists;
                    album.genre = audiofile.genre;
                    album.name = audiofile.album;
                    album.year = audiofile.year;

                    album.id = mediaDao->save_album(cleanPath, &album);
                }
                squawk::model::Song song(audiofile.title, (*list_iter).mime_type, (*list_iter).name, (*list_iter).mtime, audiofile.sample_rate, audiofile.bitrate,
                                         (*list_iter).size, audiofile.sample_frequency, audiofile.length, audiofile.track, audiofile.disc, audiofile.channels,
                                         audiofile.bits_per_sample, artists);
                mediaDao->save_audiofile((*list_iter).name, (*list_iter).mtime, (*list_iter).size, album.id, &song);
            }

        }
        files.erase(AUDIOFILE);
    }

    if(files.find( IMAGEFILE ) != files.end()) {
        if(dir_type == MUSIC) {
            for(std::list< file_item >::iterator list_iter = files[IMAGEFILE].begin(); list_iter != files[IMAGEFILE].end(); list_iter++) {

                if(album.id == 0) {
                    std::string cleanPath = get_album_clean_path( path );
                    album = mediaDao->get_album(cleanPath);
                }
                std::stringstream cover_stream;
                cover_stream << squawk_config->string_value(CONFIG_TMP_DIRECTORY) << "/" << album.id << ".jpg";
                std::string cover_filename = cover_stream.str();
                std::string filename = get_artist_clean_name((*list_iter).name);

                if(! mediaDao->exist_imagefile((*list_iter).name, (*list_iter).mtime, (*list_iter).size, true)) {
                    Imlib_Image image = imlib_load_image((*list_iter).name.c_str());
                    imlib_context_set_image(image);
                    if (image) {
                        squawk::model::Image imagefile;
                        imagefile.height = imlib_image_get_height();
                        imagefile.width = imlib_image_get_width();
                        imagefile.type = ((*list_iter).name.find("front") == std::string::npos && (*list_iter).name.find("cover") == std::string::npos ? squawk::model::Image::OTHER : squawk::model::Image::COVER);
                        imagefile.mime_type = "image/jpeg";
                        int image_id = mediaDao->save_imagefile((*list_iter).name,  (*list_iter).mtime, (*list_iter).size, album.id, &imagefile);
                        imlib_free_image();

                        std::stringstream image_stream;
                        image_stream << squawk_config->string_value(CONFIG_TMP_DIRECTORY) << "/image-" << image_id << ".jpg";

                        resize_image((*list_iter).name, image_stream.str());
                        if( imagefile.type == squawk::model::Image::COVER ) {
                            create_image_thumb((*list_iter).name, cover_filename);
                        }
                    } else {
                        LOG4CXX_WARN(logger, "iamge not loaded:" << (*list_iter).name)
                    }
                }
            }
        } else {
            LOG4CXX_ERROR(logger, "found images " << files[IMAGEFILE].size() << ":" << path) //TODO
        }
        files.erase(IMAGEFILE);
    }
    return dir_type;
}

void FileParser::create_image_thumb(std::string image, std::string thumb) {
    Imlib_Image imlib_image = imlib_load_image(image.c_str());
    if(imlib_image) {
        imlib_context_set_image(imlib_image);
        Imlib_Image imlib_thumb = imlib_create_cropped_scaled_image(0, 0, imlib_image_get_width(), imlib_image_get_height(), 100, 100);
        imlib_free_image();
        imlib_context_set_image(imlib_thumb);
        imlib_save_image(thumb.c_str());
        imlib_free_image();
    } else {
        LOG4CXX_WARN(logger, "image not loaded:" << image)
    }
}
void FileParser::resize_image(std::string image, std::string thumb) {
    Imlib_Image imlib_image = imlib_load_image(image.c_str());
    if(imlib_image) {
        imlib_context_set_image(imlib_image);
        int x, y;
        if(imlib_image_get_width() > imlib_image_get_height()) {
            double ratio = (double)1000 / (double)imlib_image_get_width();
            x = (double)imlib_image_get_width() * ratio;
            y = (double)imlib_image_get_height() * ratio;
        } else {
            double ratio = (double)1000 / (double)imlib_image_get_height();
            x = (double)imlib_image_get_width() * ratio;
            y = (double)imlib_image_get_height() * ratio;
        }
        Imlib_Image imlib_thumb = imlib_create_cropped_scaled_image(0, 0, imlib_image_get_width(), imlib_image_get_height(), x, y);
        imlib_free_image();
        imlib_context_set_image(imlib_thumb);
        imlib_save_image(thumb.c_str());
        imlib_free_image();
    } else {
        LOG4CXX_WARN(logger, "image not loaded:" << image)
    }
}

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
    } else if( commons::string::ends_with(filename, ".cue", true) ||
               commons::string::ends_with(filename, ".m3u", true) ||
               commons::string::ends_with(filename, ".txt", true) ) {
        return "text/plain";
    } else return std::string("application/octet-stream");
}

}}
