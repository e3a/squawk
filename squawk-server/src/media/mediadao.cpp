#include "mediadao.h"

#include <ctime>
#include <iostream>

#define SQL_TABEL_NAME_AUDIO "tbl_cds_audiofiles"
#define SQL_TABEL_NAME_ARTISTS_ALBUMS "tbl_cds_artists"
#define SQL_TABEL_NAME_ARTISTS "tbl_cds_artists"
#define SQL_TABEL_NAME_ALBUMS "tbl_cds_albums"
#define SQL_TABEL_NAME_IMAGES "tbl_cds_images"

namespace squawk {
namespace media {

log4cxx::LoggerPtr MediaDao::logger(log4cxx::Logger::getLogger("squawk.media.MediaDao"));

const char *SQL_TABLE_CREATE_TABLE_AUDIO = "create table tbl_cds_audiofiles(album_id, filename NOT NULL, filesize, mtime, title, track, timestamp, mime_type, bitrate, sample_rate, bits_per_sample, channels, length, disc);";
const char *SQL_TABLE_CREATE_TABLE_AUDIO_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexFilename ON tbl_cds_audiofiles (filename)";
const char *SQL_TABLE_CREATE_TABLE_ARTISTS_ALBUMS = "create table tbl_cds_artists_albums(album_id, artist_id, role);";
const char *SQL_TABLE_CREATE_TABLE_ARTISTS_ALBUMS_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexMapping ON tbl_cds_artists_albums (album_id, artist_id)";
const char *SQL_TABLE_CREATE_TABLE_ARTISTS = "create table tbl_cds_artists(name, clean_name, letter);";
const char *SQL_TABLE_CREATE_TABLE_ALBUMS = "create table tbl_cds_albums(path, name, genre, year);";
const char *SQL_TABLE_CREATE_TABLE_IMAGES = "create table tbl_cds_images(album, filename, mtime, timestamp, filesize, type, mime_type, width, height);";
const char *SQL_TABLE_CREATE_TABLE_IMAGES_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexImagesFilename ON tbl_cds_images (filename)";


const char *SQL_GET_AUDIOFILE = "select audiofile.ROWID from tbl_cds_audiofiles audiofile where audiofile.filename = ?";
const char *SQL_GET_ALBUM = "select album.name, album.genre, album.year, album.ROWID from tbl_cds_albums album where album.path = ?";
const char *SQL_GET_ALBUM_ID = "select ROWID from tbl_cds_albums where path = ?";
const char *SQL_INSERT_ALBUM = "insert into tbl_cds_albums(path, name, genre, year) values (?,?,?,?)";
const char *SQL_INSERT_ALBUM_ARTIST_MAPPING = "insert into tbl_cds_artists_albums(album_id, artist_id) values (?,?)";
const char *SQL_GET_ARTIST_ID = "select ROWID from tbl_cds_artists where clean_name = ?";
const char *SQL_INSERT_ARTIST = "insert into tbl_cds_artists(name, clean_name, letter) values (?,?,?)";
const char *SQL_INSERT_AUDIOFILE = "insert into tbl_cds_audiofiles(" \
                                   "filename, filesize, mtime, timestamp, album_id, title, bitrate, sample_rate, bits_per_sample, " \
                                   "channels, track, mime_type, length, disc) values (?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
const char *SQL_UPDATE_AUDIOFILE = "update tbl_cds_audiofiles SET " \
                                   "filesize=?, mtime=?, timestamp=?, album_id=?, title=?, bitrate=?, sample_rate=?, bits_per_sample=?, " \
                                   "channels=?, track=?, mime_type=?, length=?, disc=? where filename = ?";
const char *SQL_INSERT_IMAGE = "insert into tbl_cds_images(album, filename, mtime, timestamp, filesize, type, mime_type, width, height) " \
                               "values (?,?,?,?,?,?,?,?,?)";
const char *SQL_SWEEP_AUDIOFILES = "select ROWID, album_id from tbl_cds_audiofiles where timestamp < ?";
const char *SQL_SWEEP_IMAGES = "delete from tbl_cds_IMAGES where timestamp < ?";
const char *SQL_DELETE_AUDIOFILE = "delete from tbl_cds_audiofiles where ROWID = ?";
const char *SQL_DELETE_ALBUM = "delete from tbl_cds_albums where ROWID = ?";
const char *SQL_DELETE_IMAGE = "delete from tbl_cds_images where ROWID = ?";

MediaDao::MediaDao(squawk::db::Sqlite3Database * db) : db(db) {
    //create tables if they dont exist
    if(exist_table(SQL_TABEL_NAME_AUDIO)==false) {
        create_table(SQL_TABLE_CREATE_TABLE_AUDIO);
    }
    create_table(SQL_TABLE_CREATE_TABLE_AUDIO_INDEX);
    if(exist_table(SQL_TABEL_NAME_ARTISTS_ALBUMS)==false) {
        create_table(SQL_TABLE_CREATE_TABLE_ARTISTS_ALBUMS);
    }
    create_table(SQL_TABLE_CREATE_TABLE_ARTISTS_ALBUMS_INDEX);
    if(exist_table(SQL_TABEL_NAME_ARTISTS)==false) {
        create_table(SQL_TABLE_CREATE_TABLE_ARTISTS);
    }
    if(exist_table(SQL_TABEL_NAME_ALBUMS)==false) {
        create_table(SQL_TABLE_CREATE_TABLE_ALBUMS);
    }
    if(exist_table(SQL_TABEL_NAME_IMAGES)==false) {
        create_table(SQL_TABLE_CREATE_TABLE_IMAGES);
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
        stmtMap["GET_AUDIOFILE"] = db->prepare_statement( SQL_GET_AUDIOFILE );
        stmtMap["GET_ALBUM"] = db->prepare_statement( SQL_GET_ALBUM );
        stmtMap["GET_ALBUM_ID"] = db->prepare_statement( SQL_GET_ALBUM_ID );
        stmtMap["INSERT_ALBUM"] = db->prepare_statement( SQL_INSERT_ALBUM );
        stmtMap["GET_ARTIST_ID"] = db->prepare_statement( SQL_GET_ARTIST_ID );
        stmtMap["INSERT_ARTIST"] = db->prepare_statement( SQL_INSERT_ARTIST );
        stmtMap["INSERT_ALBUM_ARTIST_MAPPING"] = db->prepare_statement( SQL_INSERT_ALBUM_ARTIST_MAPPING );
        stmtMap["INSERT_AUDIOFILE"] = db->prepare_statement( SQL_INSERT_AUDIOFILE );
        stmtMap["UPDATE_AUDIOFILE"] = db->prepare_statement( SQL_UPDATE_AUDIOFILE );
        stmtMap["INSERT_IMAGE"] = db->prepare_statement( SQL_INSERT_IMAGE );
        stmtMap["SWEEP_AUDIOFILES"] = db->prepare_statement( SQL_SWEEP_AUDIOFILES );
        stmtMap["SWEEP_IMAGES"] = db->prepare_statement( SQL_SWEEP_IMAGES );
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
bool MediaDao::exist_table(std::string table_name) {
    LOG4CXX_TRACE(logger, "exist Table:" << table_name );
    bool exist = false;
    squawk::db::Sqlite3Statement * statement;
    try {
        statement = db->prepare_statement("SELECT count(*) FROM sqlite_master WHERE name='"+table_name+"'");
        while( statement->step() ){
            int result = statement->get_int(0);
            if(result == 1) {
                exist = true;
            } else if(result > 1) {
                LOG4CXX_WARN(logger, "found to many rows:" << result)
            }
        }
    } catch( ::db::DbException * e) {
        LOG4CXX_FATAL(logger, "Can not test table, Exception:" << e->code() << "-> " << e->what());
        if(statement != NULL) db->release_statement(statement);
        throw;
    }
    statement->reset();
    db->release_statement(statement);
    return exist;
}
void MediaDao::create_table(std::string query) {
    LOG4CXX_TRACE(logger, "creat Table:" << query );
    squawk::db::Sqlite3Statement * stmt_table_create;
    try {
        stmt_table_create = db->prepare_statement(query);
        stmt_table_create->step();

    } catch( ::db::DbException * e) {
        LOG4CXX_FATAL(logger, "create table, Exception:" << e->code() << "-> " << e->what());
        if(stmt_table_create != NULL) db->release_statement(stmt_table_create);
        throw;
    }
    stmt_table_create->reset();
    db->release_statement(stmt_table_create);
}
bool MediaDao::exist_audiofile(std::string filename, long mtime, long size, bool update) {
    LOG4CXX_TRACE(logger, "exist audiofile:" << filename );
    return exist(SQL_TABEL_NAME_AUDIO, filename, mtime, size, update);
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
squawk::media::Album MediaDao::get_album(std::string path) {
    LOG4CXX_TRACE(logger, "get Album:" << path );
    squawk::media::Album album;
    try {
        squawk::db::Sqlite3Statement * stmt = stmtMap[ "GET_ALBUM" ];
        stmt->bind_text(1, path);
        while( stmt->step() ) {
            album.name = stmt->get_string(0);
            album.genre = stmt->get_string(1);
            album.year = stmt->get_string(2);
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
            stmt_insert_album->bind_text(2, album->name);
            stmt_insert_album->bind_text(3, album->genre);
            stmt_insert_album->bind_text(4, album->year);
            stmt_insert_album->insert();
            stmt_insert_album->reset();

            album_id = db->last_insert_rowid();

            //and save the artist mappings
            for(std::list< squawk::media::Artist >::iterator list_iter = album->artists.begin(); list_iter != album->artists.end(); list_iter++) {
                squawk::media::Artist artist = *list_iter;

                squawk::db::Sqlite3Statement * stmt_insert_album_mapping = stmtMap["INSERT_ALBUM_ARTIST_MAPPING"];
                stmt_insert_album_mapping->bind_int( 1, album_id );
                stmt_insert_album_mapping->bind_int( 2, artist.id() );
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
unsigned long MediaDao::save_artist(squawk::media::Artist & artist) {
    LOG4CXX_TRACE(logger, "save artist:" << artist.name() );
    unsigned long artist_id = 0;
    try {
        squawk::db::Sqlite3Statement * stmt_artist_id = stmtMap["GET_ARTIST_ID"];
        stmt_artist_id->bind_text( 1, artist.clean_name() );
        if( stmt_artist_id->step() )
            artist_id = stmt_artist_id->get_int(0);
        stmt_artist_id->reset();

        if(artist_id == 0) {
            squawk::db::Sqlite3Statement * stmt_insert_artist = stmtMap["INSERT_ARTIST"];

            stmt_insert_artist->bind_text(1, artist.name() );
            stmt_insert_artist->bind_text(2, artist.clean_name() );
            stmt_insert_artist->bind_text(3, artist.letter() );
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
            stmt->bind_int(7, audiofile->samplerate);
            stmt->bind_int(8, audiofile->bits_per_sample);
            stmt->bind_int(9, audiofile->channels);
            stmt->bind_int(10, audiofile->track);
            stmt->bind_text(11, audiofile->mime_type);
            stmt->bind_int(12, audiofile->playLength);
            stmt->bind_int(13, audiofile->disc);
            stmt->bind_text(14, filename);
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
            stmt->bind_int(8, audiofile->samplerate);
            stmt->bind_int(9, audiofile->bits_per_sample);
            stmt->bind_int(10, audiofile->channels);
            stmt->bind_int(11, audiofile->track);
            stmt->bind_text(12, audiofile->mime_type);
            stmt->bind_int(13, audiofile->playLength);
            stmt->bind_int(14, audiofile->disc);
            stmt->insert();
            stmt->reset();
        }
        stmt_audiofile->reset();

    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save audiofile, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
}
unsigned long MediaDao::save_imagefile(std::string filename, long mtime, long size, unsigned long album, squawk::media::Image * imagefile) {
    LOG4CXX_TRACE(logger, "save imagefile:" << filename );
    try {
        squawk::db::Sqlite3Statement * stmt = stmtMap["INSERT_IMAGE"];
        stmt->bind_int(1, album);
        stmt->bind_text(2, filename);
        stmt->bind_int(3, mtime);
        stmt->bind_int(4, std::time(0));
        stmt->bind_int(5, size);
        stmt->bind_int(6, imagefile->type);
        stmt->bind_text(7, imagefile->mime_type);
        stmt->bind_int(8, imagefile->width);
        stmt->bind_int(9, imagefile->height);
        stmt->insert();
        stmt->reset();
        return db->last_insert_rowid();

    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save imagefile, Exception:" << e->code() << "-> " << e->what());
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

    } catch( ::db::DbException * e ) {
        LOG4CXX_FATAL(logger, "Can not save imagefile, Exception:" << e->code() << "-> " << e->what());
        throw;
    }
}
}}
