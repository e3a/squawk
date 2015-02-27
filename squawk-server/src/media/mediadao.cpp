#include "mediadao.h"

#include <ctime>
#include <iostream>

#define SQL_TABEL_NAME_AUDIO "tbl_cds_audiofiles"
#define SQL_TABEL_NAME_MOVIES "tbl_cds_movies"
#define SQL_TABEL_NAME_ARTISTS_ALBUMS "tbl_cds_artists"
#define SQL_TABEL_NAME_ARTISTS "tbl_cds_artists"
#define SQL_TABEL_NAME_ALBUMS "tbl_cds_albums"
#define SQL_TABEL_NAME_IMAGES "tbl_cds_images"

namespace squawk {
namespace media {

log4cxx::LoggerPtr MediaDao::logger(log4cxx::Logger::getLogger("squawk.media.MediaDao"));

const char *SQL_GET_DIRECTORY = "select ROWID from tbl_cds_files where filename = ?";
const char *SQL_INSERT_DIRECTORY ="insert into tbl_cds_files(name, parent, type, filename) values(?,?,?,?)";
const char *SQL_GET_FILE = "select ROWID from tbl_cds_files where filename = ?";
const char *SQL_INSERT_FILE_IMAGE = "insert into tbl_cds_files(parent, filename, mtime, timestamp, filesize, type, mime_type, width, height) " \
                               "values (?,?,?,?,?,?,?,?,?)";
const char *SQL_UPDATE_FILE_IMAGE = "update tbl_cds_files SET " \
                                   "parent=?, mtime=?, timestamp=?, filesize=?, type=?, mime_type=?, width=?, height=? where ROWID = ?";
const char *SQL_INSERT_FILE_VIDEO = "insert into tbl_cds_files(parent, filename, mtime, timestamp, filesize, type, mime_type, name, width, height) " \
                               "values (?,?,?,?,?,?,?,?,?,?)";
const char *SQL_UPDATE_FILE_VIDEO = "update tbl_cds_files SET " \
                                   "parent=?, mtime=?, timestamp=?, filesize=?, type=?, mime_type=?, name=?, width=?, height=? where ROWID = ?";


const char *SQL_GET_AUDIOFILE = "select audiofile.ROWID from tbl_cds_audiofiles audiofile where audiofile.filename = ?";
const char *SQL_GET_ALBUM = "select album.name, album.genre, album.year, album.ROWID from tbl_cds_albums album where album.path = ?";
const char *SQL_GET_IMAGE = "select image.ROWID from tbl_cds_images image where image.filename = ?";
const char *SQL_GET_VIDEO = "select video.ROWID from tbl_cds_movies video where video.filename = ?";
const char *SQL_GET_ALBUM_ID = "select ROWID from tbl_cds_albums where path = ?";
const char *SQL_INSERT_ALBUM = "insert into tbl_cds_albums(path, name, genre, year, clean_name, letter) values (?,?,?,?,?,?)";
const char *SQL_INSERT_ALBUM_ARTIST_MAPPING = "insert into tbl_cds_artists_albums(album_id, artist_id) values (?,?)";
const char *SQL_GET_ARTIST_ID = "select ROWID from tbl_cds_artists where clean_name = ?";
const char *SQL_INSERT_ARTIST = "insert into tbl_cds_artists(name, clean_name, letter) values (?,?,?)";
const char *SQL_INSERT_AUDIOFILE = "insert into tbl_cds_audiofiles(" \
                                   "filename, filesize, mtime, timestamp, album_id, title, bitrate, bits_per_sample, " \
                                   "channels, track, mime_type, length, disc) values (?,?,?,?,?,?,?,?,?,?,?,?,?)";
const char *SQL_UPDATE_AUDIOFILE = "update tbl_cds_audiofiles SET " \
                                   "filesize=?, mtime=?, timestamp=?, album_id=?, title=?, bitrate=?, bits_per_sample=?, " \
                                   "channels=?, track=?, mime_type=?, length=?, disc=? where filename = ?";
const char *SQL_INSERT_IMAGE = "insert into tbl_cds_images(album, filename, mtime, timestamp, filesize, type, mime_type, width, height) " \
                               "values (?,?,?,?,?,?,?,?,?)";
const char *SQL_UPDATE_IMAGE = "update tbl_cds_images SET " \
                                   "album=?, mtime=?, timestamp=?, filesize=?, type=?, mime_type=?, width=?, height=? where ROWID = ?";
const char *SQL_INSERT_VIDEO = "insert into tbl_cds_files(filename, mtime, timestamp, filesize, mime_type, width, height) " \
                               "values (?,?,?,?,?,?,?)";
const char *SQL_UPDATE_VIDEO = "update tbl_cds_files SET " \
                                   "mtime=?, timestamp=?, filesize=?, mime_type=?, width=?, height=? where ROWID = ?";
const char *SQL_SWEEP_AUDIOFILES = "select ROWID, album_id from tbl_cds_audiofiles where timestamp < ?";
const char *SQL_SWEEP_IMAGES = "delete from tbl_cds_IMAGES where timestamp < ?";
const char *SQL_SWEEP_VIDEOS = "delete from tbl_cds_movies where timestamp < ?";
const char *SQL_DELETE_AUDIOFILE = "delete from tbl_cds_audiofiles where ROWID = ?";
const char *SQL_DELETE_ALBUM = "delete from tbl_cds_albums where ROWID = ?";
const char *SQL_DELETE_IMAGE = "delete from tbl_cds_images where ROWID = ?";

MediaDao::MediaDao(squawk::db::Sqlite3Database * db) : db(db) {
    //create tables if they dont exist
    for( auto stmt : create_statements ) {
        if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "create table:" << stmt );
        squawk::db::Sqlite3Statement * stmt_table_create;
        try {
            stmt_table_create = db->prepare_statement(stmt);
            stmt_table_create->step();

        } catch( ::db::DbException * e) {
            LOG4CXX_FATAL(logger, "create table, Exception:" << e->code() << "-> " << e->what());
            if(stmt_table_create != NULL) db->release_statement(stmt_table_create);
            throw;
        }
        stmt_table_create->reset();
        db->release_statement(stmt_table_create);
    }
}
MediaDao::~MediaDao() {}
void MediaDao::start_transaction() {
    try {
        db->exec("BEGIN;");
        stmtMap[SQL_TABEL_NAME_AUDIO] = db->prepare_statement("select ROWID from tbl_cds_audiofiles where filename=? and filesize=? and mtime=?");
        stmtMap["update_tbl_cds_audiofiles"] = db->prepare_statement("update tbl_cds_audiofiles set timestamp=? where ROWID=?");
        stmtMap[SQL_TABEL_NAME_IMAGES] = db->prepare_statement("select ROWID from tbl_cds_images where filename=? and filesize=? and mtime=?");
        stmtMap["update_tbl_cds_images"] = db->prepare_statement("update tbl_cds_images set timestamp=? where ROWID=?");
        stmtMap[SQL_TABEL_NAME_MOVIES] = db->prepare_statement("select ROWID from tbl_cds_movies where filename=? and filesize=? and mtime=?");
        stmtMap["update_tbl_cds_movies"] = db->prepare_statement("update tbl_cds_movies set timestamp=? where ROWID=?");

        stmtMap["GET_DIRECTORY"] = db->prepare_statement( SQL_GET_DIRECTORY );
        stmtMap["INSERT_DIRECTORY"] = db->prepare_statement( SQL_INSERT_DIRECTORY );
        stmtMap["GET_FILE"] = db->prepare_statement( SQL_GET_FILE );
        stmtMap["INSERT_FILE_IMAGE"] = db->prepare_statement( SQL_INSERT_FILE_IMAGE );
        stmtMap["UPDATE_FILE_IMAGE"] = db->prepare_statement( SQL_UPDATE_FILE_IMAGE );
        stmtMap["INSERT_FILE_VIDEO"] = db->prepare_statement( SQL_INSERT_FILE_VIDEO );
        stmtMap["UPDATE_FILE_VIDEO"] = db->prepare_statement( SQL_UPDATE_FILE_VIDEO );

        stmtMap["GET_AUDIOFILE"] = db->prepare_statement( SQL_GET_AUDIOFILE );
        stmtMap["GET_ALBUM"] = db->prepare_statement( SQL_GET_ALBUM );
        stmtMap["GET_IMAGE"] = db->prepare_statement( SQL_GET_IMAGE );
        stmtMap["GET_VIDEO"] = db->prepare_statement( SQL_GET_VIDEO );
        stmtMap["GET_ALBUM_ID"] = db->prepare_statement( SQL_GET_ALBUM_ID );
        stmtMap["INSERT_ALBUM"] = db->prepare_statement( SQL_INSERT_ALBUM );
        stmtMap["GET_ARTIST_ID"] = db->prepare_statement( SQL_GET_ARTIST_ID );
        stmtMap["INSERT_ARTIST"] = db->prepare_statement( SQL_INSERT_ARTIST );
        stmtMap["INSERT_ALBUM_ARTIST_MAPPING"] = db->prepare_statement( SQL_INSERT_ALBUM_ARTIST_MAPPING );
        stmtMap["INSERT_AUDIOFILE"] = db->prepare_statement( SQL_INSERT_AUDIOFILE );
        stmtMap["UPDATE_AUDIOFILE"] = db->prepare_statement( SQL_UPDATE_AUDIOFILE );
        stmtMap["INSERT_IMAGE"] = db->prepare_statement( SQL_INSERT_IMAGE );
        stmtMap["UPDATE_IMAGE"] = db->prepare_statement( SQL_UPDATE_IMAGE );
        stmtMap["INSERT_VIDEO"] = db->prepare_statement( SQL_INSERT_VIDEO );
        stmtMap["UPDATE_VIDEO"] = db->prepare_statement( SQL_UPDATE_VIDEO );
        stmtMap["SWEEP_AUDIOFILES"] = db->prepare_statement( SQL_SWEEP_AUDIOFILES );
        stmtMap["SWEEP_IMAGES"] = db->prepare_statement( SQL_SWEEP_IMAGES );
        stmtMap["SWEEP_VIDEOS"] = db->prepare_statement( SQL_SWEEP_VIDEOS );
        stmtMap["DELETE_AUDIOFILE"] = db->prepare_statement( SQL_DELETE_AUDIOFILE );
        stmtMap["DELETE_ALBUM"] = db->prepare_statement( SQL_DELETE_ALBUM );
        stmtMap["DELETE_IMAGE"] = db->prepare_statement( SQL_DELETE_IMAGE );
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "create statements, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
}
void MediaDao::end_transaction() {
    LOG4CXX_TRACE(logger, "destroy" );
    for (auto & kv : stmtMap ) {
        db->release_statement( kv.second );
    }
    db->exec("END;");
}
bool MediaDao::exist_audiofile(std::string filename, long mtime, long size, bool update) {
    LOG4CXX_TRACE(logger, "exist audiofile:" << filename );
    return exist(SQL_TABEL_NAME_AUDIO, filename, mtime, size, update);
}
bool MediaDao::exist_videofile(std::string filename, long mtime, long size, bool update) {
    LOG4CXX_TRACE(logger, "exist Videofile:" << filename );
    return exist(SQL_TABEL_NAME_MOVIES, filename, mtime, size, update);
}
bool MediaDao::exist_imagefile(std::string filename, long mtime, long size, bool update) {
    LOG4CXX_TRACE(logger, "exist Imagefile:" << filename );
    return exist(SQL_TABEL_NAME_IMAGES, filename, mtime, size, update);
}
bool MediaDao::exist(std::string table, std::string filename, long mtime, long size, bool update) {
    LOG4CXX_TRACE(logger, "exist file:" << filename );
    bool found = false;
    squawk::db::Sqlite3Statement * stmt = NULL;
    squawk::db::Sqlite3Statement * stmtUpdate = NULL;
    try {
        stmt = stmtMap[table];
        stmt->bind_text(1, filename);
        stmt->bind_int(2, size);
        stmt->bind_int(3, mtime);
        while( stmt->step() ) {
            int result = stmt->get_int(0);
            found = true;
            if(update) {
                stmtUpdate = stmtMap[std::string("update_") + table];
                stmtUpdate->bind_int(1, time(0));
                stmtUpdate->bind_int(2, result);
                stmtUpdate->step();
                stmtUpdate->reset();
            }
        }
        stmt->reset();
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save audiofile, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
    return found;
}
unsigned long MediaDao::getOrCreateDirectory(const std::string & path, const std::string & name, const unsigned long & parent, const int & type ) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "getOrCreateDirectory:" << path );
    int directory_id = 0;
    try {
        squawk::db::Sqlite3Statement * stmt = stmtMap[ "GET_DIRECTORY" ];
        stmt->bind_text(1, path);
        if( stmt->step() ) {
            directory_id = stmt->get_int( 0 );
        } else {
            squawk::db::Sqlite3Statement * stmtCreate = stmtMap[ "INSERT_DIRECTORY" ];
            stmtCreate->bind_text(1, name);
            stmtCreate->bind_int(2, parent);
            stmtCreate->bind_int(3, DIRECTORY );
            stmtCreate->bind_text(4, path);

            stmtCreate->insert();
            stmtCreate->reset();

            directory_id = db->last_insert_rowid();
        }
        stmt->reset();
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not get album by path, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
    return directory_id;
}
unsigned long MediaDao::saveFile(const file_item & file, const unsigned long & parent, commons::image::Image * imagefile) {
    LOG4CXX_TRACE(logger, "save imagefile:" << file.name );
    try {
       squawk::db::Sqlite3Statement * stmt_get_image = stmtMap["GET_FILE"];
        stmt_get_image->bind_text(1, file.name);
        if( stmt_get_image->step() ) {
            int image_id = stmt_get_image->get_int( 0 );

            squawk::db::Sqlite3Statement * stmt = stmtMap["UPDATE_FILE_IMAGE"];

            stmt->bind_int(1, parent);
            stmt->bind_int(2, file.mtime);
            stmt->bind_int(3, std::time(0));
            stmt->bind_int(4, file.size);
            stmt->bind_int(5, IMAGE );
            stmt->bind_text(6, file.mime_type);
            stmt->bind_int(7, imagefile->width());
            stmt->bind_int(8, imagefile->height());
            stmt->bind_int(9, image_id );
            stmt->update();
            stmt_get_image->reset();
            stmt->reset();
            return image_id;

        } else {
            squawk::db::Sqlite3Statement * stmt = stmtMap["INSERT_FILE_IMAGE"];
            stmt->bind_int(1, parent);
            stmt->bind_text(2, file.name);
            stmt->bind_int(3, file.mtime);
            stmt->bind_int(4, std::time(0));
            stmt->bind_int(5, file.size);
            stmt->bind_int(6, IMAGE );
            stmt->bind_text(7, file.mime_type);
            stmt->bind_int(8, imagefile->width());
            stmt->bind_int(9, imagefile->height());
            stmt->insert();
            stmt_get_image->reset();
            stmt->reset();
            return db->last_insert_rowid();
        }
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save imagefile, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
}
unsigned long MediaDao::saveVideo(const file_item & file, const unsigned long & parent, commons::media::MediaFile & media_file ) {
    LOG4CXX_TRACE(logger, "save video:" << file.name );
    try {
       squawk::db::Sqlite3Statement * stmt_get_audio = stmtMap["GET_FILE"];
        stmt_get_audio->bind_text(1, file.name);
        if( stmt_get_audio->step() ) {
            int video_id = stmt_get_audio->get_int( 0 );

            squawk::db::Sqlite3Statement * stmt = stmtMap["UPDATE_FILE_VIDEO"];

            stmt->bind_int(1, parent);
            stmt->bind_int(2, file.mtime);
            stmt->bind_int(3, std::time(0));
            stmt->bind_int(4, file.size);
            stmt->bind_int(5, VIDEO );
            stmt->bind_text(6, file.mime_type);
            stmt->bind_text(7, media_file.name());
            stmt->bind_int(8, media_file.getVideoStreams()[0].width());
            stmt->bind_int(9, media_file.getVideoStreams()[0].height());
            stmt->bind_int(10, video_id );
            stmt->update();
            stmt_get_audio->reset();
            stmt->reset();
            return video_id;

        } else {
            squawk::db::Sqlite3Statement * stmt = stmtMap["INSERT_FILE_VIDEO"];
            stmt->bind_int(1, parent);
            stmt->bind_text(2, file.name);
            stmt->bind_int(3, file.mtime);
            stmt->bind_int(4, std::time(0));
            stmt->bind_int(5, file.size);
            stmt->bind_int(6, VIDEO );
            stmt->bind_text(7, file.mime_type);
            stmt->bind_text(8, media_file.name());
            stmt->bind_int(9, media_file.getVideoStreams()[0].width());
            stmt->bind_int(10, media_file.getVideoStreams()[0].height());
            stmt->insert();
            stmt_get_audio->reset();
            stmt->reset();
            return db->last_insert_rowid();
        }
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save videofile, Exception:" << e->code() << "-> " << e->what());
        //TODO release statements
        throw;
    }
}


squawk::media::Album MediaDao::get_album(std::string path) {
    LOG4CXX_TRACE(logger, "get Album:" << path );
    squawk::media::Album album;
    try {
        squawk::db::Sqlite3Statement * stmt = stmtMap[ "GET_ALBUM" ];
        stmt->bind_text(1, path);
        while( stmt->step() ) {
            album.name( stmt->get_string(0) );
            album.genre( stmt->get_string(1) );
            album.year( stmt->get_string(2) );
            album.id = stmt->get_int(3);
        }
        stmt->reset();
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not get album by path, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
    return album;
}
unsigned long MediaDao::save_album( std::string path, squawk::media::Album * album ) {
    LOG4CXX_TRACE(logger, "save Album:" << path );

    long album_id = 0;
    try {

        squawk::db::Sqlite3Statement * stmt_album_id = stmtMap["GET_ALBUM_ID"];
        stmt_album_id->bind_text(1, path);
        if( stmt_album_id->step() ) {
            album_id = stmt_album_id->get_int(0);
        }
        stmt_album_id->reset();

        if(album_id == 0) {

            squawk::db::Sqlite3Statement * stmt_insert_album = stmtMap["INSERT_ALBUM"];
            stmt_insert_album->bind_text(1, path);
            stmt_insert_album->bind_text(2, album->name());
            stmt_insert_album->bind_text(3, album->genre());
            stmt_insert_album->bind_text(4, album->year());
            stmt_insert_album->bind_text(5, album->cleanName());
            stmt_insert_album->bind_text(6, album->letter());
            stmt_insert_album->insert();
            stmt_insert_album->reset();

            album_id = db->last_insert_rowid();

            //and save the artist mappings
            for( auto artist : album->artists ) {
                //squawk::media::Artist artist = *list_iter;
                LOG4CXX_TRACE(logger, "save artists: album_id:" << album_id << " artist_id:" << artist->id() );

                squawk::db::Sqlite3Statement * stmt_insert_album_mapping = stmtMap["INSERT_ALBUM_ARTIST_MAPPING"];
                stmt_insert_album_mapping->bind_int( 1, album_id );
                stmt_insert_album_mapping->bind_int( 2, artist->id() );
                stmt_insert_album_mapping->insert();
                stmt_insert_album_mapping->reset();
            }
        }
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save album, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
    return album_id;
}
unsigned long MediaDao::save_artist(squawk::media::Artist * artist) {
    LOG4CXX_TRACE(logger, "save artist: \"" << artist->name() << "\", clean_name: \"" << artist->clean_name() << "\"" );
    unsigned long artist_id = 0;
    try {
        squawk::db::Sqlite3Statement * stmt_artist_id = stmtMap["GET_ARTIST_ID"];
        stmt_artist_id->bind_text( 1, artist->clean_name() );
        if( stmt_artist_id->step() ) {
            artist_id = stmt_artist_id->get_int(0);
        }
        stmt_artist_id->reset();

        LOG4CXX_TRACE(logger, "save artist id: \"" << artist_id );
        if(artist_id == 0) {
            squawk::db::Sqlite3Statement * stmt_insert_artist = stmtMap["INSERT_ARTIST"];

            stmt_insert_artist->bind_text(1, artist->name() );
            stmt_insert_artist->bind_text(2, artist->clean_name() );
            stmt_insert_artist->bind_text(3, artist->letter() );
            stmt_insert_artist->insert();
            stmt_insert_artist->reset();
            artist_id = db->last_insert_rowid();
        }
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save artist, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
    return artist_id;
}
void MediaDao::save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::media::Song * audiofile) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "save audiofile:" << filename );
    try {
        squawk::db::Sqlite3Statement * stmt_audiofile = stmtMap["GET_AUDIOFILE"];
        stmt_audiofile->bind_text( 1, filename );
        if( stmt_audiofile->step() ) {

            squawk::db::Sqlite3Statement * stmt = stmtMap["UPDATE_AUDIOFILE"];
            stmt->bind_int(1, size);
            stmt->bind_int(2, mtime);
            stmt->bind_int(3, std::time(0));
            stmt->bind_int(4, album_id);
            stmt->bind_text(5, audiofile->title);
            stmt->bind_int(6, audiofile->bitrate);
            stmt->bind_int(7, audiofile->bits_per_sample);
            stmt->bind_int(8, audiofile->channels);
            stmt->bind_int(9, audiofile->track);
            stmt->bind_text(10, audiofile->mime_type);
            stmt->bind_int(11, audiofile->playLength);
            stmt->bind_int(12, audiofile->disc);
            stmt->bind_text(13, filename);
            stmt->insert();
            stmt->reset();

        } else {

            squawk::db::Sqlite3Statement * stmt = stmtMap["INSERT_AUDIOFILE"];
            stmt->bind_text(1, filename);
            stmt->bind_int(2, size);
            stmt->bind_int(3, mtime);
            stmt->bind_int(4, std::time(0));
            stmt->bind_int(5, album_id);
            stmt->bind_text(6, audiofile->title);
            stmt->bind_int(7, audiofile->bitrate);
            stmt->bind_int(8, audiofile->bits_per_sample);
            stmt->bind_int(9, audiofile->channels);
            stmt->bind_int(10, audiofile->track);
            stmt->bind_text(11, audiofile->mime_type);
            stmt->bind_int(12, audiofile->playLength);
            stmt->bind_int(13, audiofile->disc);
            stmt->insert();
            stmt->reset();
        }
        stmt_audiofile->reset();

    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save audiofile, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
}
unsigned long MediaDao::createDirectory( const std::string path ) {
    LOG4CXX_TRACE(logger, "create and get directory:" << path );
    std::list<std::string> listPath = commons::filesystem::getPathTokens( path );
    std::cout << "-- Path" << std::endl;
    std::string relative_path;
    int parent = 0;
    for( auto & token : listPath ) {
        relative_path += "/" + token;
        std::cout << " >" << relative_path << std::endl;
        parent = getOrCreateDirectory(relative_path, token, parent, 1);
    }
    return parent;
}
unsigned long MediaDao::save_imagefile(const file_item & file, const unsigned long & album, commons::image::Image * imagefile) {
    LOG4CXX_TRACE(logger, "save imagefile:" << file.name );
    try {

        squawk::db::Sqlite3Statement * stmt_get_image = stmtMap["GET_IMAGE"];
        stmt_get_image->bind_text(1, file.name);
        if( stmt_get_image->step() ) {
            int image_id = stmt_get_image->get_int( 0 );

            squawk::db::Sqlite3Statement * stmt = stmtMap["UPDATE_IMAGE"];

            stmt->bind_int(1, album);
            stmt->bind_int(2, file.mtime);
            stmt->bind_int(3, std::time(0));
            stmt->bind_int(4, file.size);
            stmt->bind_int(5, file.type);
            stmt->bind_text(6, file.mime_type);
            stmt->bind_int(7, imagefile->width());
            stmt->bind_int(8, imagefile->height());
            stmt->bind_int(9, image_id );
            stmt->update();
            stmt_get_image->reset();
            stmt->reset();
            return image_id;

        } else {
            squawk::db::Sqlite3Statement * stmt = stmtMap["INSERT_IMAGE"];
            stmt->bind_int(1, album);
            stmt->bind_text(2, file.name);
            stmt->bind_int(3, file.mtime);
            stmt->bind_int(4, std::time(0));
            stmt->bind_int(5, file.size);
            stmt->bind_int(6, file.type);
            stmt->bind_text(7, file.mime_type);
            stmt->bind_int(8, imagefile->width());
            stmt->bind_int(9, imagefile->height());
            stmt->insert();
            stmt_get_image->reset();
            stmt->reset();
            return db->last_insert_rowid();
        }
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save imagefile, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
}
unsigned long MediaDao::save_videofile(std::string filename, long mtime, long size, std::string mime_type) {
    LOG4CXX_TRACE(logger, "save videofile:" << filename );
    try {

        squawk::db::Sqlite3Statement * stmt_get_video = stmtMap["GET_VIDEO"];
        stmt_get_video->bind_text(1, filename);
        if( stmt_get_video->step() ) {
            int video_id = stmt_get_video->get_int( 0 );

            squawk::db::Sqlite3Statement * stmt = stmtMap["UPDATE_VIDEO"];

            stmt->bind_int(1, mtime);
            stmt->bind_int(2, std::time(0));
            stmt->bind_int(3, size);
            stmt->bind_text(4, mime_type);
            stmt->bind_int(5, video_id );
            stmt->update();
            stmt_get_video->reset();
            stmt->reset();
            return video_id;

        } else {
            squawk::db::Sqlite3Statement * stmt = stmtMap["INSERT_VIDEO"];
            stmt->bind_text(1, filename);
            stmt->bind_int(2, mtime);
            stmt->bind_int(3, std::time(0));
            stmt->bind_int(4, size);
            stmt->bind_text(5, mime_type);
            stmt->insert();
            stmt_get_video->reset();
            stmt->reset();
            return db->last_insert_rowid();
        }
    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save videofile, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
}
void MediaDao::sweep( long mtime ) {
    LOG4CXX_TRACE(logger, "sweep:" );
    try {
        squawk::db::Sqlite3Statement * stmt = stmtMap["SWEEP_AUDIOFILES"];
        squawk::db::Sqlite3Statement * stmt_delete_audiofile = stmtMap["DELETE_AUDIOFILE"];
        squawk::db::Sqlite3Statement * stmt_delete_album = stmtMap["DELETE_ALBUM"];
        squawk::db::Sqlite3Statement * stmt_delete_image = stmtMap["DELETE_IMAGE"];
        stmt->bind_int(1, mtime);
        while( stmt->step() ) {
            int audiofile_id = stmt->get_int( 0 );
            int album_id = stmt->get_int( 1 );
            std::cout << "sweep aufiodile: " << audiofile_id << " : " << album_id << std::endl;

            stmt_delete_audiofile->bind_int( 1, audiofile_id );
            stmt_delete_audiofile->step();
            stmt_delete_audiofile->reset();

            stmt_delete_album->bind_int( 1, album_id );
            stmt_delete_album->step();
            stmt_delete_album->reset();

            stmt_delete_image->bind_int( 1, album_id );
            stmt_delete_image->step();
            stmt_delete_image->reset();
        }
        stmt->reset();

        squawk::db::Sqlite3Statement * stmt_sweep_images = stmtMap["SWEEP_IMAGES"];
        stmt_sweep_images->bind_int( 1, mtime );
        stmt_sweep_images->step();
        stmt_sweep_images->reset();

        //TODO delete empty artist and mapping

        squawk::db::Sqlite3Statement * stmt_sweep_videos = stmtMap["SWEEP_VIDEOS"];
        stmt_sweep_videos->bind_int( 1, mtime );
        stmt_sweep_videos->step();
        stmt_sweep_videos->reset();


    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not sweep files, Exception:" << e->code() << "-> " << e->what());
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Can not sweep files, other Exception.");
        throw;
    }
}
}}
