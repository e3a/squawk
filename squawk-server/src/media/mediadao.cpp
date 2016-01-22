/*
    media dao implementation.
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

#include "mediadao.h"

#include <ctime>
#include <iostream>

#include <squawk.h>

namespace squawk {
namespace media {

log4cxx::LoggerPtr MediaDao::logger ( log4cxx::Logger::getLogger ( "squawk.media.MediaDao" ) );

MediaDao::MediaDao ( const std::string & database_file ) : db_ ( db::Sqlite3Database::instance().connection ( database_file ) ) {

	//create tables if they dont exist
	for ( auto & stmt : squawk::sql::CREATE_STATEMENTS ) {
		if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "create table:" << stmt ); }

		try {
			db_->prepareStatement ( stmt )->step();

        } catch ( db::DbException & e ) {
			LOG4CXX_FATAL ( logger, "create table, Exception:" << e.code() << "-> " << e.what() );
			throw;
		}
	}
}

void MediaDao::startTransaction() {
	try {
		db_->exec ( "BEGIN;" );

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "create statements, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}
}
void MediaDao::endTransaction() {
	db_->exec ( "END;" );
}

std::list< didl::DidlContainer > search( const int & parent, const int & start_index, const int & result_count ) {

}

bool MediaDao::exist ( const std::string & filename, const long & mtime, long const & size, const bool & update ) {
	bool found = false;

	try {
        db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::QUERY_EXIST_FILE );
		stmt->bind_text ( 1, filename );
		stmt->bind_int ( 2, size );
		stmt->bind_int ( 3, mtime );

		while ( stmt->step() ) {
			int result = stmt->get_int ( 0 );
			found = true;

			if ( update ) {
                db::db_statement_ptr stmtUpdate = db_->prepareStatement ( squawk::sql::UPDATE_FILE );
				stmtUpdate->bind_int ( 1, time ( 0 ) );
				stmtUpdate->bind_int ( 2, result );
				stmtUpdate->update();
			}
		}

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not update file, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}

	return found;
}
unsigned long MediaDao::saveFile ( const unsigned long & parent, const FileItem & file, Image & imagefile ) {
	if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "save imagefile:" << file.name ); }

	try {
        db::db_statement_ptr stmt_get_image = db_->prepareStatement ( squawk::sql::QUERY_FILE );
		stmt_get_image->bind_text ( 1, file.name );

		if ( stmt_get_image->step() ) {
			int image_id = stmt_get_image->get_int ( 0 );

            db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::UPDATE_FILE_IMAGE );

			stmt->bind_int ( 1, parent );
			stmt->bind_int ( 2, file.mtime );
			stmt->bind_int ( 3, std::time ( 0 ) );
			stmt->bind_int ( 4, file.size );
			stmt->bind_int ( 5, IMAGE );
			stmt->bind_text ( 6, file.mime_type );
			stmt->bind_int ( 7, imagefile.width() );
			stmt->bind_int ( 8, imagefile.height() );
			stmt->bind_int ( 9, image_id );
			stmt->update();
			return image_id;

		} else {
            db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::INSERT_FILE_IMAGE );
			stmt->bind_int ( 1, parent );
			stmt->bind_text ( 2, file.name );
			stmt->bind_int ( 3, file.mtime );
			stmt->bind_int ( 4, std::time ( 0 ) );
			stmt->bind_int ( 5, file.size );
			stmt->bind_int ( 6, IMAGE );
			stmt->bind_text ( 7, file.mime_type );
			stmt->bind_int ( 8, imagefile.width() );
			stmt->bind_int ( 9, imagefile.height() );
			stmt->insert();
			return db_->last_insert_rowid();
		}

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not save imagefile, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}
}
size_t MediaDao::save_video( const size_t & parent, const FileItem & file, commons::media::MediaFile & media_file ) {
	if ( squawk::DEBUG ) LOG4CXX_TRACE ( logger, "save video:" << file.name )
		try {
            db::db_statement_ptr stmt_get_audio = db_->prepareStatement ( squawk::sql::QUERY_FILE );
			stmt_get_audio->bind_text ( 1, file.name );

			if ( stmt_get_audio->step() ) {
				int video_id = stmt_get_audio->get_int ( 0 );

                db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::UPDATE_FILE_VIDEO );

				stmt->bind_int ( 1, parent );
				stmt->bind_int ( 2, file.mtime );
				stmt->bind_int ( 3, std::time ( 0 ) );
				stmt->bind_int ( 4, file.size );
				stmt->bind_int ( 5, VIDEO );
				stmt->bind_text ( 6, file.mime_type );
				stmt->bind_text ( 7, media_file.name() );
				stmt->bind_int ( 13, media_file.duration() );

				if ( media_file.getVideoStreams().size() > 0 ) {
					stmt->bind_int ( 8, media_file.getVideoStreams() [0].width() );
					stmt->bind_int ( 9, media_file.getVideoStreams() [0].height() );
					stmt->bind_int ( 14, ( int ) media_file.getVideoStreams() [0].codec() );
					stmt->bind_int ( 10, media_file.getVideoStreams() [0].bitrate() );

				} else {
					stmt->bind_int ( 8, 0 );
					stmt->bind_int ( 9, 0 );
					stmt->bind_int ( 10, 0 );
					stmt->bind_int ( 14, 0 );
				}

				if ( media_file.getAudioStreams().size() > 0 ) {
					stmt->bind_int ( 11, media_file.getAudioStreams() [0].sampleFrequency() );
					stmt->bind_int ( 12, media_file.getAudioStreams() [0].channels() );

				} else {
					stmt->bind_int ( 11, 0 );
					stmt->bind_int ( 12, 0 );
				}

				stmt->bind_int ( 15, video_id );

				stmt->update();
				return video_id;

			} else {
                db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::INSERT_FILE_VIDEO );
				stmt->bind_int ( 1, parent );
				stmt->bind_text ( 2, file.name );
				stmt->bind_int ( 3, file.mtime );
				stmt->bind_int ( 4, std::time ( 0 ) );
				stmt->bind_int ( 5, file.size );
				stmt->bind_int ( 6, VIDEO );
				stmt->bind_text ( 7, file.mime_type );
				stmt->bind_text ( 8, media_file.name() );
				stmt->bind_int ( 14, media_file.duration() );

				if ( media_file.getVideoStreams().size() > 0 ) {
					stmt->bind_int ( 9, media_file.getVideoStreams() [0].width() );
					stmt->bind_int ( 10, media_file.getVideoStreams() [0].height() );
					stmt->bind_int ( 15, ( int ) media_file.getVideoStreams() [0].codec() );
					stmt->bind_int ( 11, media_file.getVideoStreams() [0].bitrate() );

				} else {
					stmt->bind_int ( 9, 0 );
					stmt->bind_int ( 10, 0 );
					stmt->bind_int ( 11, 0 );
					stmt->bind_int ( 15, 0 );
				}

				if ( media_file.getAudioStreams().size() > 0 ) {
					stmt->bind_int ( 12, media_file.getAudioStreams() [0].sampleFrequency() );
					stmt->bind_int ( 13, media_file.getAudioStreams() [0].channels() );

				} else {
					stmt->bind_int ( 12, 0 );
					stmt->bind_int ( 13, 0 );
				}

				stmt->insert();
				return db_->last_insert_rowid();
			}

        } catch ( db::DbException & e ) {
			LOG4CXX_FATAL ( logger, "Can not save videofile, Exception:" << e.code() << "-> " << e.what() );
			throw;
		}
}


squawk::media::Album MediaDao::getAlbum ( const std::string & path ) {
	if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "get AlbumByPath:" << path ); }

	squawk::media::Album album;

	try {
        db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::QUERY_GET_ALBUM_BY_PATH );
		stmt->bind_text ( 1, path );

		while ( stmt->step() ) {
			album.name ( stmt->get_string ( 0 ) );
			album.genre ( stmt->get_string ( 1 ) );
			album.year ( stmt->get_string ( 2 ) );
			album.id ( stmt->get_int ( 3 ) );
		}

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not get album by path, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}

	return album;
}
unsigned long MediaDao::save ( squawk::media::Album & album ) {
	if ( squawk::DEBUG ) LOG4CXX_TRACE ( logger, "save Album:" << album.cleanPath() )

		long album_id = 0;

	try {

        db::db_statement_ptr stmt_album_id = db_->prepareStatement ( squawk::sql::QUERY_GET_ALBUM_BY_PATH );
		stmt_album_id->bind_text ( 1, album.cleanPath() );

		if ( stmt_album_id->step() ) {
			album_id = stmt_album_id->get_int ( 3 );
		}

		stmt_album_id->reset();

		if ( album_id == 0 ) {

            db::db_statement_ptr stmt_insert_album = db_->prepareStatement ( squawk::sql::QUERY_INSERT_ALBUM );
			stmt_insert_album->bind_text ( 1, album.cleanPath() );
			stmt_insert_album->bind_text ( 2, album.name() );
			stmt_insert_album->bind_text ( 3, album.genre() );
			stmt_insert_album->bind_text ( 4, album.year() );
			stmt_insert_album->bind_text ( 5, album.cleanName() );
			stmt_insert_album->bind_text ( 6, album.letter() );
			stmt_insert_album->insert();

			album_id = db_->last_insert_rowid();

			//and save the artist mappings
			for ( auto & artist : album.artists() ) {
				if ( squawk::DEBUG ) LOG4CXX_TRACE ( logger, "save artists: album_id:" << album_id << " artist_id:" << artist.id() )
                    db::db_statement_ptr stmt_insert_album_mapping = db_->prepareStatement ( squawk::sql::QUERY_INSERT_ALBUM_ARTIST_MAPPING );

				stmt_insert_album_mapping->bind_int ( 1, album_id );
				stmt_insert_album_mapping->bind_int ( 2, artist.id() );
				stmt_insert_album_mapping->insert();
			}
		}

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not save album, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}

	return album_id;
}
unsigned long MediaDao::save ( Artist & artist ) {
	if ( squawk::DEBUG ) LOG4CXX_TRACE ( logger, "save artist, or get artist id: \"" << artist.name() << "\", clean_name: \"" << artist.cleanName() << "\"" )
		unsigned long artist_id = 0;

	try {
        db::db_statement_ptr stmt_artist_id = db_->prepareStatement ( squawk::sql::QUERY_ARTIST_BY_CLEAN_NAME );
		stmt_artist_id->bind_text ( 1, artist.cleanName() );

		if ( stmt_artist_id->step() ) {
			artist_id = stmt_artist_id->get_int ( 0 );
		}

		if ( artist_id == 0 ) {
			if ( squawk::DEBUG ) LOG4CXX_TRACE ( logger, "save artist: \"" << artist.name() << "\", clean_name: \"" << artist.cleanName() << "\"" )
                db::db_statement_ptr stmt_insert_artist = db_->prepareStatement ( squawk::sql::INSERT_ARTIST );

			stmt_insert_artist->bind_text ( 1, artist.name() );
			stmt_insert_artist->bind_text ( 2, artist.cleanName() );
			stmt_insert_artist->bind_text ( 3, artist.letter() );
			stmt_insert_artist->insert();
			artist_id = db_->last_insert_rowid();
		}

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not save artist, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}

	return artist_id;
}




void MediaDao::saveFile ( const size_t & parent_id, const std::string & filename, const long & mtime, const long & size,
						  const unsigned long & album_id, const squawk::media::Song & song ) {

	if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "save audiofile:" << filename ); }

	try {
        db::db_statement_ptr stmt_audiofile = db_->prepareStatement ( squawk::sql::QUERY_AUDIOFILE_BY_FILENAME );
		stmt_audiofile->bind_text ( 1, filename );

		if ( stmt_audiofile->step() ) {

            db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::UPDATE_AUDIOFILE );
            stmt->bind_int ( 1, parent_id );
			stmt->bind_int ( 2, size );
			stmt->bind_int ( 3, mtime );
			stmt->bind_int ( 4, std::time ( 0 ) );
			stmt->bind_int ( 5, album_id );
			stmt->bind_text ( 6, song.title() );
			stmt->bind_int ( 7, song.bitrate() );
			stmt->bind_int ( 8, song.bitsPerSample() );
			stmt->bind_int ( 9, song.sampleFrequency() );
			stmt->bind_int ( 10, song.channels() );
			stmt->bind_int ( 11, song.track() );
			stmt->bind_text ( 12, song.mime_type() );
			stmt->bind_int ( 13, song.playLength() );
			stmt->bind_int ( 14, song.disc() );
			stmt->bind_int ( 15, AUDIO );
			stmt->bind_text ( 16, filename );
			stmt->insert();

		} else {

            db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::INSERT_AUDIOFILE );
            stmt->bind_int ( 1, parent_id );
			stmt->bind_text ( 2, filename );
			stmt->bind_int ( 3, size );
			stmt->bind_int ( 4, mtime );
			stmt->bind_int ( 5, std::time ( 0 ) );
			stmt->bind_int ( 6, album_id );
			stmt->bind_text ( 7, song.title() );
			stmt->bind_int ( 8, song.bitrate() );
			stmt->bind_int ( 9, song.bitsPerSample() );
			stmt->bind_int ( 10, song.sampleFrequency() );
			stmt->bind_int ( 11, song.channels() );
			stmt->bind_int ( 12, song.track() );
			stmt->bind_text ( 13, song.mime_type() );
			stmt->bind_int ( 14, song.playLength() );
			stmt->bind_int ( 15, song.disc() );
			stmt->bind_int ( 16, AUDIO );
			stmt->insert();
		}

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not save audiofile, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}
}
unsigned long MediaDao::save ( const unsigned long parent, const std::string & name, const std::string & path ) {
	if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "create and get directory, parent: " << parent << ", name:" << name <<  ", path:" << path ); }

	int directory_id = 0;

	try {
        db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::QUERY_DIRECTORY );
		stmt->bind_int ( 1, parent );
		stmt->bind_text ( 2, path );

		if ( stmt->step() ) {
			directory_id = stmt->get_int ( 0 );

		} else {
            db::db_statement_ptr stmtCreate = db_->prepareStatement ( squawk::sql::INSERT_DIRECTORY );
			stmtCreate->bind_text ( 1, name );
			stmtCreate->bind_int ( 2, parent );
			stmtCreate->bind_int ( 3, DIRECTORY );
			stmtCreate->bind_text ( 4, path );

			stmtCreate->insert();

			directory_id = db_->last_insert_rowid();
		}

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not get or create path, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}

	return directory_id;
}
unsigned long MediaDao::saveFile ( const unsigned long & path_id, const FileItem & file, const unsigned long & album, Image & imagefile ) {
	if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "save imagefile:" << file.name ); }

	try {

        db::db_statement_ptr stmt_get_image = db_->prepareStatement ( squawk::sql::QUERY_IMAGE_BY_FILENAME );
		stmt_get_image->bind_text ( 1, file.name );

		if ( stmt_get_image->step() ) {
			int image_id = stmt_get_image->get_int ( 0 );

            db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::UPDATE_IMAGE );
			stmt->bind_int ( 1, path_id );
			stmt->bind_int ( 2, album );
			stmt->bind_int ( 3, file.mtime );
			stmt->bind_int ( 4, std::time ( 0 ) );
			stmt->bind_int ( 5, file.size );
			stmt->bind_int ( 6, file.type );
			stmt->bind_text ( 7, file.mime_type );
			stmt->bind_int ( 8, imagefile.width() );
			stmt->bind_int ( 9, imagefile.height() );
			stmt->bind_int ( 10, image_id );
			stmt->update();
			return image_id;

		} else {

            db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::INSERT_IMAGE );
			stmt->bind_int ( 1, path_id );
			stmt->bind_int ( 2, album );
			stmt->bind_text ( 3, file.name );
			stmt->bind_int ( 4, file.mtime );
			stmt->bind_int ( 5, std::time ( 0 ) );
			stmt->bind_int ( 6, file.size );
			stmt->bind_int ( 7, file.type );
			stmt->bind_text ( 8, file.mime_type );
			stmt->bind_int ( 9, imagefile.width() );
			stmt->bind_int ( 10, imagefile.height() );
			stmt->insert();
			return db_->last_insert_rowid();
		}

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not save imagefile, Exception:" << e.code() << "-> " << e.what() );
		throw;
	}
}
void MediaDao::saveFile ( const unsigned long & parent, const FileItem & file, const std::string & isbn ) {
    if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "save book:" << file.name ); }

    try {

        db::db_statement_ptr stmt_get_book = db_->prepareStatement ( squawk::sql::QUERY_FILE );
        stmt_get_book->bind_text ( 1, file.name );

        if ( stmt_get_book->step() ) {
            int image_id = stmt_get_book->get_int ( 0 );

            db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::UPDATE_BOOK );

            stmt->bind_int ( 1, parent );
            stmt->bind_text ( 2, file.name );
            stmt->bind_int ( 3, file.mtime );
            stmt->bind_int ( 4, std::time ( 0 ) );
            stmt->bind_int ( 5, file.size );
            stmt->bind_int ( 6, BOOK );
            stmt->bind_text ( 7, file.mime_type );
            stmt->bind_text ( 8, isbn );
            stmt->bind_text ( 9, file.name );
            stmt->bind_int ( 10, image_id );
            stmt->update();
            // TODO return image_id;

        } else {

            db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::INSERT_BOOK );
            stmt->bind_int ( 1, parent );
            stmt->bind_text ( 2, file.name );
            stmt->bind_int ( 3, file.mtime );
            stmt->bind_int ( 4, std::time ( 0 ) );
            stmt->bind_int ( 5, file.size );
            stmt->bind_int ( 6, BOOK );
            stmt->bind_text ( 7, file.mime_type );
            stmt->bind_text ( 8, isbn );
            stmt->bind_text ( 9, file.name );
            stmt->insert();
            // TODO return db_->last_insert_rowid();
        }

    } catch ( db::DbException & e ) {
        LOG4CXX_FATAL ( logger, "Can not save book, Exception:" << e.code() << "-> " << e.what() );
        throw;
    }
}
void MediaDao::sweep ( long mtime ) {
	if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "sweep:" ); }

	try {
        db::db_statement_ptr stmt = db_->prepareStatement ( squawk::sql::SWEEP_FILES );
        stmt->bind_int( 1, mtime );
        stmt->update();

        db::db_statement_ptr stmt_delete_album = db_->prepareStatement ( squawk::sql::DELETE_ALBUM );
        stmt_delete_album->update();

        db::db_statement_ptr stmt_orphan_artist_mappings = db_->prepareStatement ( squawk::sql::DELETE_ORPHAN_ARTIST_MAPPINGS );
        stmt_orphan_artist_mappings->update();

        db::db_statement_ptr stmt_orphan_artist = db_->prepareStatement ( squawk::sql::DELETE_ORPHAN_ARTIST );
        stmt_orphan_artist->update();

    } catch ( db::DbException & e ) {
		LOG4CXX_FATAL ( logger, "Can not sweep files, Exception:" << e.code() << "-> " << e.what() );
		throw;

	} catch ( ... ) {
		LOG4CXX_FATAL ( logger, "Can not sweep files, other Exception." );
		throw;
	}
}
} // media
} // squawk
