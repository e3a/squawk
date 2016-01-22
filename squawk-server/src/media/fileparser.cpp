/*
    file parser implementaion.
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

log4cxx::LoggerPtr FileParser::logger ( log4cxx::Logger::getLogger ( "squawk.media.FileParser" ) );

void FileParser::parse ( std::vector< std::string > paths ) {

	long start_time = std::time ( 0 );
	statistic.clear();
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	mediaDao->startTransaction();

	for ( auto & path : paths ) {
		if ( commons::filesystem::is_directory ( path ) ) {
			if ( squawk::DEBUG ) LOG4CXX_DEBUG ( logger, "import files:" << path )
				_parse ( 0, path,path );

		} else {
			LOG4CXX_WARN ( logger, path << "is not a directory." )
		}
	}

	//output statistic
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;

	mediaDao->sweep ( start_time );

	std::stringstream ss;
	int sum = 0;
	ss << std::endl << "***************************************************" << std::endl;

	for ( std::map< std::string, int >::iterator iter = statistic.begin(); iter != statistic.end(); ++iter ) {
		ss << iter->first << "\t" << iter->second << std::endl;
		sum += iter->second;
	}

	ss << "Total:\t" << sum << std::endl;
	ss << "Time:\t" << elapsed_seconds.count() << std::endl;
	LOG4CXX_DEBUG ( logger, ss.str() )
	mediaDao->endTransaction();

}

FileParser::DIRECTORY_TYPE FileParser::_parse ( const unsigned long & path_id, const std::string & basepath, const std::string & path ) {

	std::map< FILE_TYPE, std::list< FileItem > > files;
	DIRECTORY_TYPE dir_type = NONE;

	//search for the files and directories
	struct dirent *entry;
	DIR *dp;

	dp = opendir ( path.c_str() );

	if ( dp == NULL ) {
		LOG4CXX_ERROR ( logger, "opendir: Path does not exist or could not be read." + std::string ( path ) )
		return FileParser::NONE;
	}

	while ( ( entry = readdir ( dp ) ) ) {
		if ( std::string ( entry->d_name ) != std::string ( "." ) && std::string ( entry->d_name ) != std::string ( ".." ) ) {
			std::string name = path + std::string ( "/" ) + std::string ( entry->d_name );
			struct stat s;

			if ( stat ( name.c_str(), &s ) == 0 ) {
				if ( s.st_mode & S_IFREG ) {

					//check if the file exists
					if ( ! mediaDao->exist ( name, s.st_mtim.tv_sec, s.st_size, true ) ) {

						std::string type = get_mime_type ( name );

						int count = 0;

						if ( statistic.find ( type ) != statistic.end() )
						{ count = statistic.find ( type )->second; }

						statistic[type] = ++count;

						//store file in the list
						if ( commons::string::starts_with ( type, "audio/" ) ) {
							files[AUDIOFILE].insert ( files[AUDIOFILE].end(), FileItem ( name, type, s.st_mtim.tv_sec, s.st_size ) );

						} else if ( commons::string::starts_with ( type, "image/" ) ) {
							files[IMAGEFILE].insert ( files[IMAGEFILE].end(), FileItem ( name, type, s.st_mtim.tv_sec, s.st_size ) );

						} else if ( commons::string::starts_with ( type, "video/" ) ) {
							files[VIDEOFILE].insert ( files[VIDEOFILE].end(), FileItem ( name, type, s.st_mtim.tv_sec, s.st_size ) );

						} else if ( type == "application/pdf" ) {
							files[EBOOK].insert ( files[EBOOK].end(), FileItem ( name, type, s.st_mtim.tv_sec, s.st_size ) );

						} else {
							LOG4CXX_WARN ( logger, "unknown file type:" << type << ":" << name )
						}
					}

				} else if ( s.st_mode & S_IFDIR ) {
					std::string relative_path = name.substr ( basepath.size() );
					unsigned long path_id_ = mediaDao->save ( path_id, entry->d_name, relative_path );
					dir_type = _parse ( path_id_, basepath, name );

				} else {
					LOG4CXX_ERROR ( logger, "path is neither a reqular file nor a directory:" << path )
				}
			}
		}
	}

	closedir ( dp );

	squawk::media::Album album;

	if ( files.find ( AUDIOFILE ) != files.end() ) {
		dir_type = MUSIC;
		for ( auto & audiofile : files[ AUDIOFILE ] ) {

			//parse the mediafile
			commons::media::MediaFile media_file = commons::media::MediaParser::parseFile ( audiofile.name );

			//create the album if we dont have it.
			if ( ! album.equals ( media_file.getTag ( commons::media::MediaFile::ALBUM ) ) ) {

				album = squawk::media::Album ( media_file.getTag ( commons::media::MediaFile::ALBUM ),
											   media_file.getTag ( commons::media::MediaFile::GENRE ),
											   media_file.getTag ( commons::media::MediaFile::YEAR ),
											   path );

				if ( media_file.hasTag ( commons::media::MediaFile::ARTIST ) ) {
					LOG4CXX_WARN ( logger, "artist: " << media_file.getTag ( commons::media::MediaFile::ARTIST ) )
					squawk::media::Artist artist ( media_file.getTag ( commons::media::MediaFile::ARTIST ) );

                    if ( ! album.contains ( artist ) ) {
						unsigned long new_artist_id = mediaDao->save ( artist );
						artist.id ( new_artist_id );
						album.add ( std::move ( artist ) );
					}
				}

				if ( media_file.hasTag ( commons::media::MediaFile::COMPOSER ) ) {
					LOG4CXX_WARN ( logger, "artist: " << media_file.getTag ( commons::media::MediaFile::COMPOSER ) )
					squawk::media::Artist artist ( media_file.getTag ( commons::media::MediaFile::COMPOSER ) );

                    if ( ! album.contains ( artist ) ) {
						unsigned long new_artist_id = mediaDao->save ( artist );
						artist.id ( new_artist_id );
						album.add ( std::move ( artist ) );
					}
				}

				album.id ( mediaDao->save ( album ) );
			}

			//create and save the song
			squawk::media::Song song ( media_file.getTag ( commons::media::MediaFile::TITLE ),
									   audiofile.mime_type, audiofile.name,
									   media_file.getAudioStreams() [0].bitrate(),
									   media_file.getAudioStreams() [0].sampleFrequency(),
									   media_file.duration(),
									   commons::string::parse_string<int> ( media_file.getTag ( commons::media::MediaFile::TRACK ) ),
									   commons::string::parse_string<int> ( media_file.getTag ( commons::media::MediaFile::DISC ) ),
									   media_file.getAudioStreams() [0].channels(),
									   media_file.getAudioStreams() [0].bitsPerSample() );
			mediaDao->saveFile ( path_id, audiofile.name, audiofile.mtime, audiofile.size, album.id(), song );
		}

		files.erase ( AUDIOFILE );
	}

	if ( files.find ( IMAGEFILE ) != files.end() ) {
		if ( dir_type == MUSIC ) {
			if ( squawk::DEBUG ) LOG4CXX_TRACE ( logger, "found covers " << files[IMAGEFILE].size() << ":" << path )
				for ( FileItem & file : files[IMAGEFILE] ) {

					// get the album if we dont have it.
					std::string cleanPath = squawk::media::get_album_clean_path ( path );

					if ( album.cleanPath() != cleanPath ) {
						album = mediaDao->getAlbum ( cleanPath );
					}

					// is it a front cover
                    file.type =
                            ( file.name.find ( "front" ) == std::string::npos && file.name.find ( "cover" ) == std::string::npos ? AUDIO_IMAGE : AUDIO_COVER );

					Image image ( file.name );
					int image_id = mediaDao->saveFile ( path_id, file, album.id(), image );
                    parseImage( image, tmp_directory_, image_id );
				}

		} else {
			if ( squawk::DEBUG ) LOG4CXX_TRACE ( logger, "found images " << files[IMAGEFILE].size() << ":" << path )

				//save image file
				for ( auto & image_file : files[IMAGEFILE] ) {
					Image image ( image_file.name );
					unsigned long id = mediaDao->saveFile ( path_id, image_file, image );

					//create tumbs
					image.scale ( 150, 150, tmp_directory_ + "/images" + "/cover" + commons::string::to_string<unsigned long> ( id ) + ".jpg" );
					// image.scale(1000, 1000, tmp_directory_ + "/images" + "/" + commons::string::to_string<unsigned long>(id) + ".jpg");
				}
		}

		files.erase ( IMAGEFILE );
	}

	if ( files.find ( VIDEOFILE ) != files.end() ) {
		for ( auto & video : files[VIDEOFILE] ) {

			commons::media::MediaFile media_file = commons::media::MediaParser::parseFile ( video.name );

			if ( squawk::DEBUG ) LOG4CXX_DEBUG ( logger, "save video" << video.name )

            mediaDao->save_video ( path_id, video, media_file );
		}

		files.erase ( VIDEOFILE );
	}

	if ( files.find ( EBOOK ) != files.end() ) {
		//save book file
		for ( auto & book : files[EBOOK] ) {
			std::string isbn = PdfParser::parsePdf ( book.name );

			if ( squawk::DEBUG ) LOG4CXX_TRACE ( logger, "found book:" << book.name << ", ISBN:" << isbn )
            mediaDao->saveFile( path_id, book, isbn );
        }

		files.erase ( EBOOK );
	}

	return dir_type;
}
std::string FileParser::get_mime_type ( const std::string & filename ) {
	if ( commons::string::ends_with ( filename, ".flac", true ) ) {
		return "audio/x-flac";

	} else if ( commons::string::ends_with ( filename, ".mp3", true ) ) {
		return "audio/mpeg";

	} else if ( commons::string::ends_with ( filename, ".ogg", true ) ) {
		return "audio/vorbis";

	} else if ( commons::string::ends_with ( filename, ".mpc", true ) ) {
		return "audio/x-musepack";

	} else if ( commons::string::ends_with ( filename, ".aac", true ) ) {
		return "audio/aac";

	} else if ( commons::string::ends_with ( filename, ".ape", true ) ) {
		return "audio/x-ape";

    } else if ( commons::string::ends_with ( filename, ".dsf", true ) ) {
        return "audio/dsd";

    } else if ( commons::string::ends_with ( filename, ".jpg", true ) || commons::string::ends_with ( filename, ".jpeg", true ) ) {
		return "image/jpeg";

	} else if ( commons::string::ends_with ( filename, ".gif", true ) ) {
		return "image/gif";

	} else if ( commons::string::ends_with ( filename, ".png", true ) ) {
		return "image/png";

	} else if ( commons::string::ends_with ( filename, ".pdf", true ) ) {
		return "application/pdf";

	} else if ( commons::string::ends_with ( filename, ".mp4", true ) ) {
		return "video/mp4";

    } else if ( commons::string::ends_with ( filename, ".mkv", true ) ) {
        return "video/x-matroska";

    } else if ( commons::string::ends_with ( filename, ".mpeg", true ) ) {
        return "video/mpeg";

	} else if ( commons::string::ends_with ( filename, ".avi", true ) ) {
		return "video/x-msvideo";

    } else if ( commons::string::ends_with ( filename, ".mov", true ) ) {
        return "video/quicktime";

    } else if ( commons::string::ends_with ( filename, ".wmv", true ) ) {
        return "video/x-ms-wmv";

    } else if ( commons::string::ends_with ( filename, ".cue", true ) ||
				commons::string::ends_with ( filename, ".m3u", true ) ||
				commons::string::ends_with ( filename, ".txt", true ) ) {
		return "text/plain";

	} else { return std::string ( "application/octet-stream" ); }
}
void FileParser::parseImage( Image & image, const std::string & prefix, int image_id ) {
    //create the thumbnails
    std::stringstream image_stream;
    image_stream << prefix << "/tn-" << image_id << ".jpg";
    image.scale ( 160, 160, image_stream.str() );

    {
    std::stringstream cover_stream;
    cover_stream << prefix << "/sm-" << image_id << ".jpg";
    std::string cover_filename = cover_stream.str();
    image.scale ( 480, 480, cover_filename );
    }
    {
    std::stringstream cover_stream;
    cover_stream << prefix << "/lrg-" << image_id << ".jpg";
    std::string cover_filename = cover_stream.str();
    image.scale ( 768, 768, cover_filename );
    }
}
} // media
} // squawk
