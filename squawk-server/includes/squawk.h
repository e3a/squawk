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

#ifndef SQuAWK_H
#define SQuAWK_H

#include <map>
#include <list>
#include <string>

#include <ostream>
#include <iostream> 

/* #include "squawk-utils.h" */

#define SQUAWK_VERSION 1

namespace squawk {  
  namespace model {
   /**
    * The artist entity.
    */
    struct Artist {
      Artist() : id(0), name(""), letter(""), clean_name("") {};
      Artist(std::string name, std::string letter, std::string clean_name) : id(0), name(name), letter(letter), clean_name(clean_name) {};
      unsigned long id;
      std::string letter, name, clean_name;
      /**
	* create the json stream.
      friend std::ostream& operator<<(std::ostream& out, const squawk::model::Artist & artist) {
        out << "{\"id\":" << artist.id << ",\"name\":\"" << squawk::utils::string::escape_json(artist.name) << "\",\"letter\":\"" << artist.letter << "\",\"clean_name\":\"" << squawk::utils::string::escape_json(artist.clean_name) << "\"}";
        return out;
      };
    */
    };
   /**
    * The song entity.
    */
    struct Song {
        Song() : id(0), title(""), mime_type(""), filename(""), mtime(0), samplerate(0), bitrate(0), size(0), sampleFrequency(0),
            playLength(0), track(0), disc(0), channels(0), bits_per_sample(0) {}
        Song(std::string title, std::string mime_type, std::string filename, int mtime, int samplerate, int bitrate, int size,
             int sampleFrequency, int playLength, int track, int disc, int channels, int bits_per_sample, std::list< squawk::model::Artist > artist) :
            id(0), title(title), mime_type(mime_type), filename(filename), mtime(mtime), samplerate(samplerate), bitrate(bitrate), size(size),
            sampleFrequency(sampleFrequency), playLength(playLength), track(track), disc(disc), channels(channels), bits_per_sample(bits_per_sample), artist(artist) {}
      unsigned long id;
      std::string title, mime_type, filename, album, genre, comment, year;
      int mtime, samplerate, bitrate, size, sampleFrequency, bits_per_sample, playLength, track, disc, channels;
      std::list< Artist > artist;

      /**
	* Create the json stream.
      friend std::ostream& operator<<(std::ostream& out, const squawk::model::Song & song) {
        out << "{\"id\":" << song.id << ",\"title\":\"" << squawk::utils::string::escape_json(song.title) << "\",\"mimetype\":\"" << squawk::utils::string::escape_json(song.mime_type) << "\",\"samplerate\":" << song.samplerate << "," <<
	       "\"bitrate\":" << song.bitrate << ",\"samplefrequency\":" << song.sampleFrequency << ",\"playlength\":" << song.playLength << ",\"track\":" << song.track<< "," <<
	       "\"disc\":" << song.disc << ",\"channels\":" << song.channels <<  ", \"bitspersample\":" << song.bits_per_sample << "}";
        return out;
      };
    */
    };
   /**
    * The image entity.
    */
    struct Image {
      enum IMAGE_TYPE { COVER, OTHER } type;
      unsigned long id;
      std::string filename,  mime_type;
      int width, height;
      /**
	* create the json stream.
      friend std::ostream& operator<<(std::ostream& out, const squawk::model::Image & image) {
        out << "{\"id\":" << image.id << ",\"mime-type\":\"" << squawk::utils::string::escape_json(image.mime_type) << "\",\"width\":" << image.width << ",\"height\":" << image.height << "}";
        return out;
      };
    */
    };
   /**
    * The album entity.
    */
    struct Album {
      Album() : id(0), name(""), genre(""), year("") {};
      unsigned long id;
      std::string name, genre, year;
      std::list< Artist > artists;
      std::list< Song> songs;
      std::list< Image > images;
      /**
	* create the json stream.
	* the stream includes the artist list but not the songs and images.
      friend std::ostream& operator<<(std::ostream& out, squawk::model::Album & album) {
        out << "{\"id\":" << album.id << ",\"name\":\"" << squawk::utils::string::escape_json(album.name) << "\",\"genre\":\"" << squawk::utils::string::escape_json(album.genre) << 
	       "\",\"year\":\"" << squawk::utils::string::escape_json(album.year) << "\",\"artists\":[";        
	bool first = true;
	for(std::list< squawk::model::Artist >::iterator artist_iter = album.artists.begin(); artist_iter != album.artists.end(); artist_iter++) {
	  if(first) first=false;
	  else out << ",";
	  out << (*artist_iter);
	}
        out << "]}";
        return out;
      };
    */
    };
/*    struct Imagefile { //TODO REMOVE
      Imagefile() : width(0), height(0) {}
      std::string mime_type;
      int width, height;
      Image::IMAGE_TYPE type;
    }; */
  }
  namespace db {
    class DaoException : public std::exception { 
    public: 
      explicit DaoException (int _code, std::string _what) throw() : _code(_code), _what(_what) {};
      virtual ~DaoException() throw() {};
      virtual const char* what() const throw() {
	return _what.c_str();
      };
      int code() throw() {
	return _code;
      }
    private: 
      int _code;
      std::string _what;
    };
    class SquawkDaoInterface {
     /**
      * Test if a audio record already exists.
      * \param filename the filename 
      * \param mtime the last access time
      * \param size the filesize
      * \param touch update the timestamp
      */
     virtual bool exist_audiofile(std::string filename, long mtime, long size, bool update) = 0;      
     /**
      * Test if a image record already exists.
      * \param filename the filename 
      * \param mtime the last access time
      * \param size the filesize
      * \param touch update the timestamp
      */
     virtual bool exist_imagefile(std::string filename, long mtime, long size, bool update) = 0;
     /**
      * List all albums
      * \return list of Album items
      */
     virtual std::list< squawk::model::Album > get_albums() = 0;
     /**
      * Get the albums by artist id.
      */
     virtual std::list< squawk::model::Album > getAlbumsByArtist(unsigned long id) = 0;
     /**
      * Get the album by id.
      */
     virtual squawk::model::Album get_album(unsigned long id) = 0;
     /**
      * Get the album by path.
      */
     virtual squawk::model::Album get_album(std::string path) = 0;
    /**
     * Get the Images by album id.
     */
     std::list< squawk::model::Image > get_images_by_album(unsigned long id);
 
    };
  }
  /** TODO is this still used? */
  class SquawkService {
    public:
      SquawkService() {};
      virtual ~SquawkService() {};

      virtual void start_transaction() = 0;
      virtual void end_transaction() = 0;
      
      virtual bool exist_audiofile(std::string path, unsigned long mtime, unsigned long size) = 0;
      virtual bool exist_imagefile(std::string path, unsigned long mtime, unsigned long size) = 0;

      virtual std::list< squawk::model::Album > get_albums() = 0;
      virtual std::list< squawk::model::Album > get_albums_by_artist(unsigned long artist_id) = 0;
      virtual std::list< squawk::model::Artist > get_artists() = 0;
      virtual squawk::model::Album get_album(unsigned long id, bool recursive = false) = 0;
      virtual squawk::model::Album get_album(std::string path) = 0;
      
      virtual unsigned long save_artist(squawk::model::Artist & artist) = 0;
      virtual unsigned long save_album(std::string path, squawk::model::Album * album) = 0;
      virtual unsigned long save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::model::Song * song) = 0;
      virtual unsigned long save_imagefile(std::string filename, long mtime, long size, unsigned long album_id, squawk::model::Image * imagefile) = 0;
  };

namespace media {
    struct Audiofile {
        Audiofile() : track(0), bitrate(0), sample_rate(0), bits_per_sample(0), channels(0), disc(0), sample_frequency(0) {};
        std::string album, title, year, genre, composer, mime_type, comment, performer;
        int track, bitrate, sample_rate, bits_per_sample, channels, length, disc, sample_frequency;
        std::list< std::string > artist;
    };
    class MetadataParser {
    public:
        virtual bool parse(struct squawk::media::Audiofile & audiofile, const std::string mime_type, const std::string file) = 0;

    };
}}









#endif // SQUAWK_H
