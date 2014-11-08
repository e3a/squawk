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

#include "commons.h"

#include <map>
#include <list>
#include <string>

namespace squawk {  

static const bool DEBUG = true; //make macro

namespace media {

/**
 * \brief The artist entity.
 */
struct Artist {
public:
    Artist() : id_(0), name_(""), letter_(""), clean_name_("") {}
    Artist(std::string name, std::string letter, std::string clean_name) : id_(0), name_(name), letter_(letter), clean_name_(clean_name) {}

    void id( unsigned long id ) { this->id_ = id; }
    unsigned long id() { return id_; }
    std::string letter() {
        if(clean_name().length()>0) {
            return commons::string::to_upper( clean_name().substr(0, 1) );
        } else {
            return std::string("");
        }
    }
    std::string name() { return name_; }
    std::string clean_name() {
        return commons::string::trim(commons::string::to_lower(name_));
    }

private:
    unsigned long id_;
    std::string letter_, name_, clean_name_;
};
/** unsigned long id;
 * \brief The song entity.
 */
struct Song {
    Song() : id(0), title(""), mime_type(""), filename(""), mtime(0), samplerate(0), bitrate(0), size(0), sampleFrequency(0),
        playLength(0), track(0), disc(0), channels(0), bits_per_sample(0) {}
    Song(std::string title, std::string mime_type, std::string filename, int mtime, int samplerate, int bitrate, int size,
         int sampleFrequency, int playLength, int track, int disc, int channels, int bits_per_sample, std::list< Artist > artist) :
        id(0), title(title), mime_type(mime_type), filename(filename), mtime(mtime), samplerate(samplerate), bitrate(bitrate), size(size),
        sampleFrequency(sampleFrequency), playLength(playLength), track(track), disc(disc), channels(channels), bits_per_sample(bits_per_sample), artist(artist) {}
    unsigned long id;
    std::string title, mime_type, filename, album, genre, comment, year;
    int mtime, samplerate, bitrate, size, sampleFrequency, bits_per_sample, playLength, track, disc, channels;
    std::list< Artist > artist;
};
/**
 * \brief The image entity.
 */
struct Image {
    enum IMAGE_TYPE { COVER, OTHER } type;
    unsigned long id;
    std::string filename,  mime_type;
    int width, height;
};
/**
 * \brief The album entity.
 */
struct Album {
    Album() : id(0), name(""), genre(""), year("") {}
    unsigned long id;
    std::string name, genre, year;
    std::list< Artist > artists;
    std::list< Song> songs;
    std::list< Image > images;
};
/**
  * \brief The audiofile entity.
  */
struct Audiofile {
    Audiofile() : track(0), bitrate(0), sample_rate(0), bits_per_sample(0), channels(0), disc(0), sample_frequency(0) {}
    std::string album, title, year, genre, composer, mime_type, comment, performer;
    int track, bitrate, sample_rate, bits_per_sample, channels, length, disc, sample_frequency;
    std::list< std::string > artist;
};
/**
 * \brief The MetadataParser class
 */
class MetadataParser {
public:
    /**
     * @brief parse the audiofile
     * @param audiofile the audiofile data structure
     * @param mime_type the mime-type
     * @param file the file
     * @return return true when file was parsed
     */
    virtual bool parse(struct squawk::media::Audiofile & audiofile, const std::string mime_type, const std::string file) = 0;
};
}}
#endif // SQUAWK_H
