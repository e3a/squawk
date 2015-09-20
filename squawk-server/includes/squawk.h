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

#include "pcrecpp.h"

#include <boost/algorithm/string.hpp>

namespace squawk {
namespace sql {

/* CREATE TABLES */
static std::vector< std::string > CREATE_STATEMENTS {
    "create table if not exists tbl_cds_files(name, parent, type, filename, mtime, timestamp, filesize, mime_type, width, height, color, bitrate, sampleFrequency, channels, duration, codecId, album_id, track, disc, bits_per_sample, isbn);",
    "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexFiles ON tbl_cds_files (filename)",
    "create table if not exists tbl_cds_artists_albums(album_id, artist_id, role);",
    "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexMapping ON tbl_cds_artists_albums (album_id, artist_id)",
    "create table if not exists tbl_cds_artists(name, clean_name, letter);",
    "create table if not exists tbl_cds_albums(path, name, genre, year, clean_name, letter);",
};

/* COUNT FOR STATISTICS */
static const std::string QUERY_ALBUMS_COUNT = "select count(*) from tbl_cds_albums";
static const std::string QUERY_ARTISTS_COUNT = "select count(*) from tbl_cds_artists";
static const std::string QUERY_AUDIOFILES_COUNT = "select mime_type, count(mime_type) from tbl_cds_files where type > 0 GROUP BY mime_type";
static const std::string QUERY_TYPES_COUNT = "select type, count(type) from tbl_cds_files where type > 0 GROUP BY type";

/* QUERY AND UPDATE RECORDS */
static const std::string QUERY_EXIST_FILE = "select ROWID from tbl_cds_files where filename=? and filesize=? and mtime=?";
static const std::string UPDATE_FILE = "update tbl_cds_files set timestamp=? where ROWID=?";

/* QUERY ARTIST FOR ALBUM */
static const std::string QUERY_ARTIST_BY_ALBUM = "select artist.ROWID, artist.name from tbl_cds_artists artist " \
                                                  "JOIN tbl_cds_artists_albums m ON artist.ROWID = m.artist_id " \
                                                  "where m.album_id=?";
static const std::string QUERY_ARTIST_BY_CLEAN_NAME = "select ROWID from tbl_cds_artists where clean_name = ?";
static const std::string QUERY_COUNT_ARTISTS = "select count(*) from tbl_cds_artists";
static const std::string QUERY_ARTIST = "select ROWID, name from tbl_cds_artists order by clean_name LIMIT ?, ?";

/* get albums by artists TODO remove */
static const std::string SQL_ARTIST_ALBUM = "select album.name, album.year, album.ROWID " \
        "from tbl_cds_albums album, tbl_cds_artists_albums m, tbl_cds_artists artist " \
        "where artist.ROWID = ? and album.ROWID = m.album_id and artist.ROWID = m.artist_id LIMIT ?,?";

/* get new albums TODO */
static const std::string SQL_ALBUM_NEW = "select DISTINCT album.name, album.year, album.ROWID from tbl_cds_albums album, tbl_cds_files song where album.ROWID = song.album_id ORDER BY song.mtime desc LIMIT 0, 100";
static const std::string QUERY_COUNT_ALBUMS = "select count(*) from tbl_cds_albums";
static const std::string QUERY_COUNT_ALBUMS_ARTIST =
    "select count(*) from tbl_cds_albums album, tbl_cds_artists_albums m, tbl_cds_artists artist " \
        "where artist.ROWID = ? and album.ROWID = m.album_id and artist.ROWID = m.artist_id";
static const std::string QUERY_COUNT_SONGS_ALBUM =
    "select count(*) from tbl_cds_files songs, tbl_cds_albums album where album.ROWID = ? and album.ROWID = songs.album_id";
static const std::string SQL_ALBUM_ID = "select album.name, album.genre, album.year, album.ROWID " \
        "from tbl_cds_albums album where album.ROWID = ?";
static const std::string SQL_ALBUM_ARTIST = "select artist.ROWID, artist.name from tbl_cds_artists artist " \
        "JOIN tbl_cds_artists_albums m ON artist.ROWID = m.artist_id where m.album_id=?";
static const std::string SQL_ALBUM_SONG = "select songs.ROWID, songs.name, songs.track, songs.filename, " \
        "songs.duration, songs.bitrate, songs.sampleFrequency, songs.bits_per_sample, songs.channels, " \
        "songs.mime_type, songs.disc, songs.mtime, songs.filesize " \
        "from tbl_cds_files songs, tbl_cds_albums album where songs.type = 3 and album.ROWID = ? and " \
        "album.ROWID = songs.album_id order by songs.disc, songs.track";

/* QUERY BOOKS AND ALL ITEMS */
static const std::string QUERY_BOOKS = "select ROWID, name, filename, mime_type from tbl_cds_files where type=4 order by name";

/* QUERY ALBUM AND ALL ITEMS */
static const std::string QUERY_GET_ALBUM_BY_PATH = "select album.name, album.genre, album.year, album.ROWID from tbl_cds_albums album where album.path = ?";
static const std::string QUERY_ALBUM = "select album.name, album.genre, album.year, album.ROWID from tbl_cds_albums album where album.ROWID = ?";
static const std::string QUERY_SONGS = "select songs.ROWID, songs.name, songs.track, songs.filename, songs.duration, songs.bitrate, songs.sampleFrequency, " \
                                       "songs.bits_per_sample, songs.channels, songs.mime_type, songs.disc from tbl_cds_files songs, tbl_cds_albums album " \
                                       "where album.ROWID = ? and album.ROWID = songs.album_id and songs.type = 3 order by songs.track, songs.disc";
static const std::string QUERY_AUDIOFILE_BY_FILENAME = "select audiofile.ROWID from tbl_cds_files audiofile where audiofile.filename = ?";
static const std::string QUERY_IMAGES = "select ROWID from tbl_cds_files where album_id = ? and type in (5, 6) order by type asc";

/* get albums TODO remove */
static const std::string SQL_ALBUM = "select name, year, ROWID from tbl_cds_albums ORDER BY name LIMIT ?, ?";

/* INSERT ALBUM */
static const std::string QUERY_INSERT_ALBUM = "insert into tbl_cds_albums(path, name, genre, year, clean_name, letter) values (?,?,?,?,?,?)";
static const std::string INSERT_ARTIST = "insert into tbl_cds_artists(name, clean_name, letter) values (?,?,?)";
static const std::string QUERY_INSERT_ALBUM_ARTIST_MAPPING = "insert into tbl_cds_artists_albums(album_id, artist_id) values (?,?)";
static const std::string UPDATE_AUDIOFILE = "update tbl_cds_files SET " \
                                            "parent=?, filesize=?, mtime=?, timestamp=?, album_id=?, name=?, bitrate=?, bits_per_sample=?, sampleFrequency=?," \
                                            "channels=?, track=?, mime_type=?, duration=?, disc=?, type=? where filename = ?";
static const std::string INSERT_AUDIOFILE = "insert into tbl_cds_files(" \
                                            "parent, filename, filesize, mtime, timestamp, album_id, name, bitrate, bits_per_sample, sampleFrequency, " \
                                            "channels, track, mime_type, duration, disc, type) values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";


static const std::string QUERY_IMAGE_BY_FILENAME = "select image.ROWID from tbl_cds_files image where image.filename = ?";
static const std::string UPDATE_IMAGE = "update tbl_cds_files SET " \
                                        "parent=?, album_id=?, mtime=?, timestamp=?, filesize=?, type=?, mime_type=?, width=?, height=? where ROWID = ?";
static const std::string INSERT_IMAGE = "insert into tbl_cds_files(parent, album_id, filename, mtime, timestamp, filesize, type, mime_type, width, height) " \
                                        "values (?,?,?,?,?,?,?,?,?,?)";

/* QUERY VIDEO LIST */
static const std::string QUERY_VIDEOS = "select ROWID, name, filename, mime_type, duration, filesize, sampleFrequency, width, height, bitrate, channels from tbl_cds_files where type=2 order by name";
static const std::string QUERY_VIDEO = "select name, filename, mime_type, duration, filesize, sampleFrequency, width, height, bitrate, channels from tbl_cds_files where ROWID = ?";

/* DIRECTORY */
static const std::string QUERY_DIRECTORY = "select ROWID from tbl_cds_files where type = 0 and parent = ? and filename = ?";
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

/* IMAGE */
static const std::string UPDATE_BOOK = "update tbl_cds_files SET " \
                                             "parent=?, mtime=?, timestamp=?, filesize=?, type=?, mime_type=?, isbn=?, name=? where ROWID = ?";
static const std::string INSERT_BOOK = "insert into tbl_cds_files(parent, filename, mtime, timestamp, filesize, type, mime_type, isbn, name) " \
                                             "values (?,?,?,?,?,?,?,?,?)";

/* SWEEP AND DELETE */
static const std::string SWEEP_FILES = "delete from tbl_cds_files where timestamp < ?";
static const std::string DELETE_ALBUM = "delete from tbl_cds_albums " \
                                        "where ROWID not in ( select album_id from tbl_cds_files where type in (3, 5, 6))";
static const std::string DELETE_ORPHAN_ARTIST_MAPPINGS = "delete from tbl_cds_artists_albums " \
                                                         "where album_id not in ( select ROWID from tbl_cds_albums )";
static const std::string DELETE_ORPHAN_ARTIST = "delete  from tbl_cds_artists " \
                                                "where ROWID not in ( select artist_id from tbl_cds_artists_albums )";
} // sql
} // squawk

namespace squawk {  

static const bool DEBUG = true; //TODO make macro

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

enum FILE_TYPE { DIRECTORY = 0, IMAGE = 1, VIDEO = 2, AUDIO = 3, BOOK = 4, AUDIO_COVER = 5, AUDIO_IMAGE = 6, OTHER = 20 };

namespace media {

/**
 * @brief get the clean name
 * @param name
 * @return
 */
inline std::string clean_name( const std::string & name ) {

    std::string s = name;

    std::string allowed = "+-()";
    s.erase(remove_if(s.begin(), s.end(), [&allowed](const char& c) {
            return allowed.find(c) != string::npos;
    }), s.end());

    boost::algorithm::trim( s );
    boost::algorithm::to_lower( s );

    if( s.rfind("the ", 0) == 0)
        s.erase(0, 4);
    if( s.rfind("die ", 0) == 0)
        s.erase(0, 4);
    if( s.rfind("das ", 0) == 0)
        s.erase(0, 4);
    if( s.rfind("der ", 0) == 0)
        s.erase(0, 4);
    return s;
}
inline std::string get_letter( const std::string & clean_name ) {
    if(clean_name.length()>0) {
        return commons::string::to_upper( clean_name.substr(0, 1) );
    } else {
        return std::string("");
    }
}
static pcrecpp::RE re_clean_path("(.*)/CD[\\d+]");
inline std::string get_album_clean_path(const std::string & path) {
    string clean_path;
    if(re_clean_path.PartialMatch(path, &clean_path)) {
        return boost::algorithm::trim_copy(clean_path);
    } else {
        return boost::algorithm::trim_copy(path);
    }
}
}}
#endif // SQUAWK_H
