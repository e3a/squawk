#include "mediadao.h"

#include <ctime>
#include <iostream>

#include <squawk.h>

#define SQL_TABEL_NAME_AUDIO "tbl_cds_audiofiles"
#define SQL_TABEL_NAME_MOVIES "tbl_cds_movies"
#define SQL_TABEL_NAME_ARTISTS_ALBUMS "tbl_cds_artists"
#define SQL_TABEL_NAME_ARTISTS "tbl_cds_artists"
#define SQL_TABEL_NAME_ALBUMS "tbl_cds_albums"
#define SQL_TABEL_NAME_IMAGES "tbl_cds_images"

namespace squawk {
namespace media {

log4cxx::LoggerPtr MediaDao::logger(log4cxx::Logger::getLogger("squawk.media.MediaDao"));

MediaDao::MediaDao(squawk::db::db_connection_ptr db) : db(db) {
    //create tables if they dont exist
    for( auto & stmt : squawk::sql::CREATE_STATEMENTS ) {
        if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "create table:" << stmt );
        try {
            db->prepareStatement(stmt)->step();

        } catch( squawk::db::DbException & e) {
            LOG4CXX_FATAL(logger, "create table, Exception:" << e.code() << "-> " << e.what());
            throw;
        }
    }
}
MediaDao::~MediaDao() {}

void MediaDao::start_transaction() {
    try {
        db->exec("BEGIN;");

    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "create statements, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
}
void MediaDao::end_transaction() {
    db->exec("END;");
}
bool MediaDao::exist_audiofile(std::string filename, long mtime, long size, bool update) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "exist audiofile:" << filename );
    return exist( squawk::sql::TBL_AUDIO, filename, mtime, size, update);
}
bool MediaDao::exist_videofile(std::string filename, long mtime, long size, bool update) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "exist Videofile:" << filename );
    return exist(squawk::sql::TBL_MOVIES , filename, mtime, size, update);
}
bool MediaDao::exist_imagefile(std::string filename, long mtime, long size, bool update) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "exist Imagefile:" << filename );
    return exist(squawk::sql::TBL_IMAGES, filename, mtime, size, update);
}
bool MediaDao::exist(const squawk::sql::TBL_NAMES & table_name, const std::string & filename, const long & mtime, const long & size, const bool & update) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "exist file:" << filename );
    bool found = false;
    try {
        squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::QUERY_EXIST[table_name] );
        stmt->bind_text(1, filename);
        stmt->bind_int(2, size);
        stmt->bind_int(3, mtime);
        while( stmt->step() ) {
            int result = stmt->get_int(0);
            found = true;
            if(update) {
                squawk::db::db_statement_ptr stmtUpdate = db->prepareStatement( squawk::sql::UPDATE_EXIST[table_name] );
                stmtUpdate->bind_int(1, time(0));
                stmtUpdate->bind_int(2, result);
                stmtUpdate->step();
            }
        }

    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not save audiofile, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
    return found;
}
unsigned long MediaDao::getOrCreateDirectory(const std::string & path, const std::string & name, const unsigned long & parent, const int & type ) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "getOrCreateDirectory:" << path );
    int directory_id = 0;
    try {
        squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::QUERY_DIRECTORY );
        stmt->bind_text(1, path);
        if( stmt->step() ) {
            directory_id = stmt->get_int( 0 );
        } else {
            squawk::db::db_statement_ptr stmtCreate = db->prepareStatement( squawk::sql::INSERT_DIRECTORY );
            stmtCreate->bind_text(1, name);
            stmtCreate->bind_int(2, parent);
            stmtCreate->bind_int(3, DIRECTORY );
            stmtCreate->bind_text(4, path);

            stmtCreate->insert();

            directory_id = db->last_insert_rowid();
        }

    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not get album by path, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
    return directory_id;
}
unsigned long MediaDao::saveFile(const file_item & file, const unsigned long & parent, commons::image::Image * imagefile) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "save imagefile:" << file.name );
    try {
       squawk::db::db_statement_ptr stmt_get_image = db->prepareStatement( squawk::sql::QUERY_FILE );
        stmt_get_image->bind_text(1, file.name);
        if( stmt_get_image->step() ) {
            int image_id = stmt_get_image->get_int( 0 );

            squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::UPDATE_FILE_IMAGE );

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
            return image_id;

        } else {
            squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::INSERT_FILE_IMAGE );
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
            return db->last_insert_rowid();
        }

    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not save imagefile, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
}
unsigned long MediaDao::saveVideo(const file_item & file, const unsigned long & parent, commons::media::MediaFile & media_file ) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "save video:" << file.name )
    try {
       squawk::db::db_statement_ptr stmt_get_audio = db->prepareStatement( squawk::sql::QUERY_FILE );
        stmt_get_audio->bind_text(1, file.name);
        if( stmt_get_audio->step() ) {
            int video_id = stmt_get_audio->get_int( 0 );

            squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::UPDATE_FILE_VIDEO );

            stmt->bind_int(1, parent);
            stmt->bind_int(2, file.mtime);
            stmt->bind_int(3, std::time(0));
            stmt->bind_int(4, file.size);
            stmt->bind_int(5, VIDEO );
            stmt->bind_text(6, file.mime_type);
            stmt->bind_text(7, media_file.name());
            stmt->bind_int(13, media_file.duration());
            if( media_file.getVideoStreams().size() > 0 ) {
                stmt->bind_int(8, media_file.getVideoStreams()[0].width());
                stmt->bind_int(9, media_file.getVideoStreams()[0].height());
                stmt->bind_int(14, (int)media_file.getVideoStreams()[0].codec());
                stmt->bind_int(10, media_file.getVideoStreams()[0].bitrate());
            } else {
                stmt->bind_int(8, 0);
                stmt->bind_int(9, 0);
                stmt->bind_int(10, 0);
                stmt->bind_int(14, 0);
            }
            if( media_file.getAudioStreams().size() > 0 ) {
                stmt->bind_int(11, media_file.getAudioStreams()[0].sampleFrequency());
                stmt->bind_int(12, media_file.getAudioStreams()[0].channels());
            } else {
                stmt->bind_int(11, 0);
                stmt->bind_int(12, 0);
            }
            stmt->bind_int(15, video_id );

            stmt->update();
            return video_id;

        } else {
            squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::INSERT_FILE_VIDEO );
            stmt->bind_int(1, parent);
            stmt->bind_text(2, file.name);
            stmt->bind_int(3, file.mtime);
            stmt->bind_int(4, std::time(0));
            stmt->bind_int(5, file.size);
            stmt->bind_int(6, VIDEO );
            stmt->bind_text(7, file.mime_type);
            stmt->bind_text(8, media_file.name());
            stmt->bind_int(14, media_file.duration());
            if( media_file.getVideoStreams().size() > 0 ) {
                stmt->bind_int(9, media_file.getVideoStreams()[0].width());
                stmt->bind_int(10, media_file.getVideoStreams()[0].height());
                stmt->bind_int(15, (int)media_file.getVideoStreams()[0].codec());
                stmt->bind_int(11, media_file.getVideoStreams()[0].bitrate());
            } else {
                stmt->bind_int(9, 0);
                stmt->bind_int(10, 0);
                stmt->bind_int(11, 0);
                stmt->bind_int(15, 0);
            }
            if( media_file.getAudioStreams().size() > 0 ) {
                stmt->bind_int(12, media_file.getAudioStreams()[0].sampleFrequency());
                stmt->bind_int(13, media_file.getAudioStreams()[0].channels());
            } else {
                stmt->bind_int(12, 0);
                stmt->bind_int(13, 0);
            }

            stmt->insert();
            return db->last_insert_rowid();
        }
    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not save videofile, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
}


squawk::media::Album MediaDao::get_album(std::string path) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "get AlbumByPath:" << path );
    squawk::media::Album album;
    try {
        squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::QUERY_GET_ALBUM_BY_PATH );
        stmt->bind_text(1, path);
        while( stmt->step() ) {
            album.name( stmt->get_string(0) );
            album.genre( stmt->get_string(1) );
            album.year( stmt->get_string(2) );
            album.id = stmt->get_int(3);
        }

    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not get album by path, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
    return album;
}
unsigned long MediaDao::save_album( std::string path, squawk::media::Album * album ) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "save Album:" << path )

    long album_id = 0;
    try {

        squawk::db::db_statement_ptr stmt_album_id = db->prepareStatement( squawk::sql::QUERY_GET_ALBUM_BY_PATH );
        stmt_album_id->bind_text(1, path);
        if( stmt_album_id->step() ) {
            album_id = stmt_album_id->get_int(3);
        }
        stmt_album_id->reset();

        if(album_id == 0) {

            squawk::db::db_statement_ptr stmt_insert_album = db->prepareStatement( squawk::sql::QUERY_INSERT_ALBUM );
            stmt_insert_album->bind_text(1, path);
            stmt_insert_album->bind_text(2, album->name());
            stmt_insert_album->bind_text(3, album->genre());
            stmt_insert_album->bind_text(4, album->year());
            stmt_insert_album->bind_text(5, album->cleanName());
            stmt_insert_album->bind_text(6, album->letter());
            stmt_insert_album->insert();

            album_id = db->last_insert_rowid();

            //and save the artist mappings
            for( auto artist : album->artists ) {
                //squawk::media::Artist artist = *list_iter;
                if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "save artists: album_id:" << album_id << " artist_id:" << artist->id() )

                squawk::db::db_statement_ptr stmt_insert_album_mapping = db->prepareStatement( squawk::sql::QUERY_INSERT_ALBUM_ARTIST_MAPPING );
                stmt_insert_album_mapping->bind_int( 1, album_id );
                stmt_insert_album_mapping->bind_int( 2, artist->id() );
                stmt_insert_album_mapping->insert();
            }
        }
    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not save album, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
    return album_id;
}
unsigned long MediaDao::save_artist(squawk::media::Artist * artist) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "save artist, or get artist id: \"" << artist->name() << "\", clean_name: \"" << artist->clean_name() << "\"" )
    unsigned long artist_id = 0;
    try {
        squawk::db::db_statement_ptr stmt_artist_id = db->prepareStatement( squawk::sql::QUERY_ARTIST_BY_CLEAN_NAME );
        stmt_artist_id->bind_text( 1, artist->clean_name() );
        if( stmt_artist_id->step() ) {
            artist_id = stmt_artist_id->get_int(0);
        }

        if(artist_id == 0) {
            if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "save artist: \"" << artist->name() << "\", clean_name: \"" << artist->clean_name() << "\"" )
            squawk::db::db_statement_ptr stmt_insert_artist = db->prepareStatement( squawk::sql::INSERT_ARTIST );

            stmt_insert_artist->bind_text(1, artist->name() );
            stmt_insert_artist->bind_text(2, artist->clean_name() );
            stmt_insert_artist->bind_text(3, artist->letter() );
            stmt_insert_artist->insert();
            artist_id = db->last_insert_rowid();
        }
    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not save artist, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
    return artist_id;
}
void MediaDao::save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::media::Song * audiofile) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "save audiofile:" << filename );
    try {
        squawk::db::db_statement_ptr stmt_audiofile = db->prepareStatement( squawk::sql::QUERY_AUDIOFILE_BY_FILENAME );
        stmt_audiofile->bind_text( 1, filename );
        if( stmt_audiofile->step() ) {

            squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::UPDATE_AUDIOFILE );
            stmt->bind_int(1, size);
            stmt->bind_int(2, mtime);
            stmt->bind_int(3, std::time(0));
            stmt->bind_int(4, album_id);
            stmt->bind_text(5, audiofile->title);
            stmt->bind_int(6, audiofile->bitrate);
            stmt->bind_int(7, audiofile->bits_per_sample);
            stmt->bind_int(8, audiofile->sampleFrequency);
            stmt->bind_int(9, audiofile->channels);
            stmt->bind_int(10, audiofile->track);
            stmt->bind_text(11, audiofile->mime_type);
            stmt->bind_int(12, audiofile->playLength);
            stmt->bind_int(13, audiofile->disc);
            stmt->bind_text(14, filename);
            stmt->insert();

        } else {

            squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::INSERT_AUDIOFILE );
            stmt->bind_text(1, filename);
            stmt->bind_int(2, size);
            stmt->bind_int(3, mtime);
            stmt->bind_int(4, std::time(0));
            stmt->bind_int(5, album_id);
            stmt->bind_text(6, audiofile->title);
            stmt->bind_int(7, audiofile->bitrate);
            stmt->bind_int(8, audiofile->bits_per_sample);
            stmt->bind_int(9, audiofile->sampleFrequency);
            stmt->bind_int(10, audiofile->channels);
            stmt->bind_int(11, audiofile->track);
            stmt->bind_text(12, audiofile->mime_type);
            stmt->bind_int(13, audiofile->playLength);
            stmt->bind_int(14, audiofile->disc);
            stmt->insert();
        }

    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not save audiofile, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
}
unsigned long MediaDao::createDirectory( const std::string path ) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "create and get directory:" << path );
    std::list<std::string> listPath = commons::filesystem::getPathTokens( path );
    std::string relative_path;
    int parent = 0;
    for( auto & token : listPath ) {
        relative_path += "/" + token;
        parent = getOrCreateDirectory(relative_path, token, parent, 1);
    }
    return parent;
}
unsigned long MediaDao::save_imagefile(const file_item & file, const unsigned long & album, commons::image::Image * imagefile) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "save imagefile:" << file.name );
    try {

        squawk::db::db_statement_ptr stmt_get_image = db->prepareStatement( squawk::sql::QUERY_IMAGE_BY_FILENAME );
        stmt_get_image->bind_text(1, file.name);
        if( stmt_get_image->step() ) {
            int image_id = stmt_get_image->get_int( 0 );

            squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::UPDATE_IMAGE );
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
            return image_id;

        } else {

            squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::INSERT_IMAGE );
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
            return db->last_insert_rowid();
        }
    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not save imagefile, Exception:" << e.code() << "-> " << e.what());
        throw;
    }
}
void MediaDao::sweep( long mtime ) {
    if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "sweep:" );
    try {
        squawk::db::db_statement_ptr stmt = db->prepareStatement( squawk::sql::SWEEP_AUDIOFILES );
        squawk::db::db_statement_ptr stmt_delete_audiofile = db->prepareStatement( squawk::sql::DELETE_AUDIOFILE );
        squawk::db::db_statement_ptr stmt_delete_album = db->prepareStatement( squawk::sql::DELETE_ALBUM );
        squawk::db::db_statement_ptr stmt_delete_image = db->prepareStatement( squawk::sql::DELETE_IMAGE );
        stmt->bind_int(1, mtime);
        while( stmt->step() ) {
            int audiofile_id = stmt->get_int( 0 );
            int album_id = stmt->get_int( 1 );
            if( squawk::DEBUG ) LOG4CXX_TRACE(logger, "sweep aufiodile: " << audiofile_id << " : " << album_id )

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

        squawk::db::db_statement_ptr stmt_sweep_images = db->prepareStatement( squawk::sql::SWEEP_IMAGES );
        stmt_sweep_images->bind_int( 1, mtime );
        stmt_sweep_images->step();

        //TODO delete empty artist and mapping

        squawk::db::db_statement_ptr stmt_sweep_videos = db->prepareStatement( squawk::sql::SWEEP_VIDEOS );
        stmt_sweep_videos->bind_int( 1, mtime );
        stmt_sweep_videos->step();

    } catch( squawk::db::DbException & e ) {
        LOG4CXX_FATAL(logger, "Can not sweep files, Exception:" << e.code() << "-> " << e.what());
        throw;
    } catch( ... ) {
        LOG4CXX_FATAL(logger, "Can not sweep files, other Exception.");
        throw;
    }
}
} // media
} // squawk
