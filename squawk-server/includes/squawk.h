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

#include <boost/algorithm/string.hpp>

namespace squawk {
namespace sql {

enum TBL_NAMES {TBL_AUDIO, TBL_IMAGES, TBL_MOVIES };

/* CREATE TABLES */
static std::vector< std::string > CREATE_STATEMENTS {
    "create table if not exists tbl_cds_audiofiles(album_id, filename NOT NULL, filesize, mtime, title, track, timestamp, mime_type, bitrate, sample_rate, bits_per_sample, channels, length, disc);",
    "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexFilename ON tbl_cds_audiofiles (filename)",
    "create table if not exists tbl_cds_artists_albums(album_id, artist_id, role);",
    "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexMapping ON tbl_cds_artists_albums (album_id, artist_id)",
    "create table if not exists tbl_cds_artists(name, clean_name, letter);",
    "create table if not exists tbl_cds_albums(path, name, genre, year, clean_name, letter);",
    "create table if not exists tbl_cds_images(album, filename, mtime, timestamp, filesize, type, mime_type, width, height);",
    "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexImagesFilename ON tbl_cds_images (filename)",
    "create table if not exists tbl_cds_movies(name, parent, type, filename, mtime, timestamp, filesize, mime_type);",
    "create table if not exists tbl_cds_files(name, parent, type, filename, mtime, timestamp, filesize, mime_type, width, height, color, bitrate, sampleFrequency, channels, duration, codecId);"
};

/* QUERY AND UPDATE RECORDS */
static std::map< TBL_NAMES, std::string > QUERY_EXIST = {
    {TBL_AUDIO, "select ROWID from tbl_cds_audiofiles where filename=? and filesize=? and mtime=?"},
    {TBL_IMAGES, "select ROWID from tbl_cds_images where filename=? and filesize=? and mtime=?"},
    {TBL_MOVIES, "select ROWID from tbl_cds_movies where filename=? and filesize=? and mtime=?"}
};
static std::map< TBL_NAMES, std::string > UPDATE_EXIST = {
    {TBL_AUDIO, "update tbl_cds_audiofiles set timestamp=? where ROWID=?"},
    {TBL_IMAGES, "update tbl_cds_images set timestamp=? where ROWID=?"},
    {TBL_MOVIES, "update tbl_cds_movies set timestamp=? where ROWID=?"}
};

/* QUERY ARTIST FOR ALBUM */
static const std::string QUERY_ARTIST_BY_ALBUM = "select artist.ROWID, artist.name from tbl_cds_artists artist " \
                                                  "JOIN tbl_cds_artists_albums m ON artist.ROWID = m.artist_id " \
                                                  "where m.album_id=?";
static const std::string QUERY_ARTIST_BY_CLEAN_NAME = "select ROWID from tbl_cds_artists where clean_name = ?";

/* QUERY ALBUM AND ALL ITEMS */
static const std::string QUERY_GET_ALBUM_BY_PATH = "select album.name, album.genre, album.year, album.ROWID from tbl_cds_albums album where album.path = ?";
static const std::string QUERY_ALBUM = "select album.name, album.genre, album.year, album.ROWID from tbl_cds_albums album where album.ROWID = ?";
static const std::string QUERY_SONGS = "select songs.ROWID, songs.title, songs.track, songs.filename, songs.length, songs.bitrate, songs.sample_rate, " \
                                       "songs.bits_per_sample, songs.channels, songs.mime_type, songs.disc from tbl_cds_audiofiles songs, tbl_cds_albums album " \
                                       "where album.ROWID = ? and album.ROWID = songs.album_id order by songs.track, songs.disc";
static const std::string QUERY_AUDIOFILE_BY_FILENAME = "select audiofile.ROWID from tbl_cds_audiofiles audiofile where audiofile.filename = ?";
static const std::string QUERY_IMAGES = "select ROWID from tbl_cds_images where album = ? order by type asc";

/* INSERT ALBUM */
static const std::string QUERY_INSERT_ALBUM = "insert into tbl_cds_albums(path, name, genre, year, clean_name, letter) values (?,?,?,?,?,?)";
static const std::string INSERT_ARTIST = "insert into tbl_cds_artists(name, clean_name, letter) values (?,?,?)";
static const std::string QUERY_INSERT_ALBUM_ARTIST_MAPPING = "insert into tbl_cds_artists_albums(album_id, artist_id) values (?,?)";
static const std::string UPDATE_AUDIOFILE = "update tbl_cds_audiofiles SET " \
                                            "filesize=?, mtime=?, timestamp=?, album_id=?, title=?, bitrate=?, bits_per_sample=?, sample_rate=?," \
                                            "channels=?, track=?, mime_type=?, length=?, disc=? where filename = ?";
static const std::string INSERT_AUDIOFILE = "insert into tbl_cds_audiofiles(" \
                                            "filename, filesize, mtime, timestamp, album_id, title, bitrate, bits_per_sample, sample_rate, " \
                                            "channels, track, mime_type, length, disc) values (?,?,?,?,?,?,?,?,?,?,?,?,?,?)";


static const std::string QUERY_IMAGE_BY_FILENAME = "select image.ROWID from tbl_cds_images image where image.filename = ?";
static const std::string UPDATE_IMAGE = "update tbl_cds_images SET " \
                                        "album=?, mtime=?, timestamp=?, filesize=?, type=?, mime_type=?, width=?, height=? where ROWID = ?";
static const std::string INSERT_IMAGE = "insert into tbl_cds_images(album, filename, mtime, timestamp, filesize, type, mime_type, width, height) " \
                                        "values (?,?,?,?,?,?,?,?,?)";

/* QUERY VIDEO LIST */
static const std::string QUERY_VIDEOS = "select ROWID, name, filename, mime_type, duration, filesize, sampleFrequency, width, height, bitrate, channels from tbl_cds_files where type=2 order by name";

/* DIRECTORY */
static const std::string QUERY_DIRECTORY = "select ROWID from tbl_cds_files where filename = ?";
static const std::string INSERT_DIRECTORY ="insert into tbl_cds_files(name, parent, type, filename) values(?,?,?,?)";

/* FILE */
static const std::string QUERY_FILE = "select ROWID from tbl_cds_files where filename = ?";

/* IMAGE */
static const std::string UPDATE_FILE_IMAGE = "update tbl_cds_files SET " \
                                             "parent=?, mtime=?, timestamp=?, filesize=?, type=?, mime_type=?, width=?, height=? where ROWID = ?";
static const std::string INSERT_FILE_IMAGE = "insert into tbl_cds_files(parent, filename, mtime, timestamp, filesize, type, mime_type, width, height) " \
                                             "values (?,?,?,?,?,?,?,?,?)";

/** VIDEO */
static const std::string UPDATE_FILE_VIDEO = "update tbl_cds_files SET " \
                                             "parent=?, mtime=?, timestamp=?, filesize=?, type=?, mime_type=?, name=?, width=?, height=?, bitrate=?, sampleFrequency=?, channels=?, duration=?, codecId=? where ROWID = ?";
static const std::string INSERT_FILE_VIDEO = "insert into tbl_cds_files(parent, filename, mtime, timestamp, filesize, type, mime_type, name, width, height, bitrate, sampleFrequency, channels, duration, codecId) " \
                                             "values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

/* SWEEP AND DELETE */
static const std::string SWEEP_AUDIOFILES = "select ROWID, album_id from tbl_cds_audiofiles where timestamp < ?";
static const std::string SWEEP_IMAGES = "delete from tbl_cds_IMAGES where timestamp < ?";
static const std::string SWEEP_VIDEOS = "delete from tbl_cds_movies where timestamp < ?";
static const std::string DELETE_AUDIOFILE = "delete from tbl_cds_audiofiles where ROWID = ?";
static const std::string DELETE_ALBUM = "delete from tbl_cds_albums where ROWID = ?";
static const std::string DELETE_IMAGE = "delete from tbl_cds_images where ROWID = ?";

} // sql
} // squawk

namespace squawk {  

static const bool DEBUG = true; //make macro

static const std::string CONFIG_LOGGER_PROPERTIES = "logger";
static const std::string CONFIG_MULTICAST_ADDRESS = "multicast-address";
static const std::string CONFIG_MULTICAST_PORT = "multicast-port";
static const std::string CONFIG_HTTP_IP = "http-ip";
static const std::string CONFIG_HTTP_PORT = "http-port";
static const std::string CONFIG_DATABASE_FILE = "database-file";
static const std::string CONFIG_TMP_DIRECTORY = "tmp-directory";
static const std::string CONFIG_LOCAL_LISTEN_ADDRESS = "local-address";
static const std::string CONFIG_UUID = "uuid";
static const std::string CONFIG_FILE = "config-file";
static const std::string CONFIG_MEDIA_DIRECTORY = "media-directory";
static const std::string CONFIG_HTTP_DOCROOT = "http-docroot";


enum FILE_TYPE { DIRECTORY = 0, IMAGE = 1, VIDEO = 2 };

namespace media {

inline std::string clean_name( const std::string & name ) {
    std::string res = boost::algorithm::trim_copy( name );
    boost::algorithm::to_lower( res );

    if( res.rfind("the ", 0) == 0)
        res.erase(0, 4);
    if( res.rfind("die ", 0) == 0)
        res.erase(0, 4);
    if( res.rfind("das ", 0) == 0)
        res.erase(0, 4);
    if( res.rfind("der ", 0) == 0)
        res.erase(0, 4);
    return res;
}
inline std::string get_letter( const std::string & clean_name ) {
    if(clean_name.length()>0) {
        return commons::string::to_upper( clean_name.substr(0, 1) );
    } else {
        return std::string("");
    }
}

/**
 * \brief The artist entity.
 */
struct Artist {
public:
    /**
     * @brief Artist Class.
     * @param name The artist Name.
     */
    Artist( const std::string & name ) :
        id_(0), name_(name),
        clean_name_(::squawk::media::clean_name( name_ )),
        letter_(::squawk::media::get_letter(clean_name_)) {}


    void id( const unsigned long & id ) { this->id_ = id; }
    unsigned long id() const { return id_; }
    const std::string letter() const {
        return letter_;
    }
    const std::string name() const { return name_; }
    const std::string clean_name() const {
        return clean_name_;
    }

private:
    unsigned long id_;
    std::string name_;
    std::string clean_name_;
    std::string letter_;
};
/**
 * \brief The song entity.
 */
struct Song {
    Song() : id(0), title(""), mime_type(""), filename(""), mtime(0), bitrate(0), size(0), sampleFrequency(0),
        playLength(0), track(0), disc(0), channels(0), bits_per_sample(0) {}
    Song(std::string title, std::string mime_type, std::string filename, int mtime, int bitrate, int size,
         int sampleFrequency, int playLength, int track, int disc, int channels, int bits_per_sample, std::list< Artist *> artist) :
        id(0), title(title), mime_type(mime_type), filename(filename), mtime(mtime), bitrate(bitrate), size(size),
        sampleFrequency(sampleFrequency), playLength(playLength), track(track), disc(disc), channels(channels), bits_per_sample(bits_per_sample), artist(artist) {}
    unsigned long id;
    std::string title, mime_type, filename, album, genre, comment, year;
    int mtime, bitrate, size, sampleFrequency, playLength, track, disc, channels, bits_per_sample;
    std::list< Artist *> artist;
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
public:
    Album() : id(0), name_(""), genre_(""), year_("") {}
    Album(const std::string & name, const std::string & genre, const std::string & year ) :
        id(0), name_(commons::string::trim(name)), genre_(commons::string::trim(genre)), year_(commons::string::trim(year)),
        clean_name_(::squawk::media::clean_name( name_ )),
        letter_(::squawk::media::get_letter(clean_name_)) {}
    ~Album() {
        for(auto a : artists) delete a;
    }

    unsigned long id;
    std::list< Artist *> artists;
    std::list< Song> songs;
    std::list< Image > images;

    std::string name() {return name_; }
    void name(const std::string & name) {name_ = commons::string::trim(name); }
    std::string genre() {return genre_; }
    void genre(const std::string & genre) {genre_ = commons::string::trim(genre); }
    std::string year() {return year_; }
    void year(const std::string & year) {name_ = commons::string::trim(year); }

    std::string cleanName() {
        return clean_name_;
    }
    std::string letter() {
        return letter_;
    }

    bool add( squawk::media::Artist * artist ) {
        for( auto _artist : artists ) {
            if( _artist->clean_name() == artist->clean_name() ) {
                return false;
            }
        }
        artists.insert(artists.end(), artist);
        return true;
    }

    bool equals( const std::string name ) {
        return name_ == commons::string::trim( name );
    }

private:
    std::string name_, genre_, year_, clean_name_, letter_;
};
/**
  * \brief The audiofile entity.
  */
struct Audiofile {
public:
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
