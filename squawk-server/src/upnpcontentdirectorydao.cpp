/*
    Copyright (C) 2016  <etienne> <etienne.knecht@hotmail.com>

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

#include "upnpcontentdirectorydao.h"


namespace squawk {
    log4cxx::LoggerPtr UpnpContentDirectoryDao::logger ( log4cxx::Logger::getLogger ( "squawk.UpnpContentDirectoryDao" ) );

    /* CREATE TABLES */
    const std::list< std::string > UpnpContentDirectoryDao::CREATE_STATEMENTS {
        "CREATE TABLE IF NOT EXISTS tbl_cds_object( cls, parent, size, title, path, mime_type, mtime, rating, year, track, playback_count, object_update_id, contributor, artist, author, publisher, genre, album, series, dlna_profile, last_playback_time, import, timestamp );",
        "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexObjectPath ON tbl_cds_object(path);",
        "CREATE TABLE IF NOT EXISTS tbl_cds_artist( clean_name, name, import );",
        "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexArtistCleanName ON tbl_cds_artist(clean_name);",
        "CREATE TABLE IF NOT EXISTS tbl_cds_resource( ref_obj, protocol_info, uri, bitrate, bits_per_sample, color_depth, dlna_profile, duration, framerate, mime_type, nr_audio_channels, resolution, sample_frequency, size );",
        "CREATE INDEX IF NOT EXISTS UniqueIndexResourceRef ON tbl_cds_resource(ref_obj);",
        "CREATE TABLE IF NOT EXISTS tbl_cds_album_art_uri( ref_obj, path, uri, profile);",
        "CREATE INDEX IF NOT EXISTS UniqueIndexAlbumArtUriRef ON tbl_cds_album_art_uri(ref_obj);",
    };

    /* CONSTRUCTOR */
    UpnpContentDirectoryDao::UpnpContentDirectoryDao( http::HttpServletContext & context ) :
        _db(db::Sqlite3Database::instance().connection( context.parameter( squawk::CONFIG_DATABASE_FILE ) ) ) {

        //create tables if they dont exist
        for ( auto & stmt : CREATE_STATEMENTS ) {
            if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "create table:" << stmt ); }

            try {
                _db->prepareStatement ( stmt )->step();

            } catch ( db::DbException & e ) {
                LOG4CXX_FATAL ( logger, "create table, Exception:" << e.code() << "-> " << e.what() );
                throw upnp::UpnpException( e.code(), "DbException: " +  std::string( e.what() ) );
            }
        }
    }

    /* TRANSACTION */
    void UpnpContentDirectoryDao::startTransaction() {
        try {
            _db->exec ( "BEGIN;" );

        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "create statements, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "DbException: " +  std::string( e.what() ) );
        }
    }
    void UpnpContentDirectoryDao::endTransaction() {
        try {
            _db->exec ( "END;" );

        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "create statements, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "DbException: " +  std::string( e.what() ) );
        }
    }

    /* SWEEP DATABASE */
    void UpnpContentDirectoryDao::sweep ( long mtime ) {
        //TODO
    }

    /* GET OBJECTS */
    size_t UpnpContentDirectoryDao::objectsCount( const didl::DIDL_CLASS cls, const size_t id ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "count objects:" << cls << "::" << id );

        try {
            //Prepare query string
            std::ostringstream query_string_;
            query_string_<< "select count(*) from tbl_cds_object where parent = ?";
            if( cls != didl::object ) query_string_ << " and cls = ? ";

            //Bind and execute query
            db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_.str() );
            stmt_objects_->bind_int ( 1, id );
            if( cls != didl::object ) stmt_objects_->bind_int ( 2, cls );

            if( stmt_objects_->step() ) {
                return stmt_objects_->get_int( 0 );
            } else return 0;
        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "objects counbd, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "objectsCount(" + std::to_string( cls ) + "::" + std::to_string( id ) + ") -> DbException: " +  std::string( e.what() ) );
        }
    }

    std::list< didl::DidlObject > UpnpContentDirectoryDao::objects( const size_t id, const size_t & start_index, const size_t & result_count ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "objects<DidlObject>::" << id << " (" << start_index << ", " << result_count << ")" );

        std::list< didl::DidlObject > object_list_;
        std::string query_string_( "select cls, ROWID, parent, title, path, mtime, object_update_id from tbl_cds_object where parent = ?" );
        if( result_count > 0 ) {
            query_string_ += " limit ?, ? ";
        }

        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "Execute query: " << query_string_ ); //TODO remove
        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_ );
        stmt_objects_->bind_int ( 1, id );
        if( result_count > 0 ) {
            stmt_objects_->bind_int ( 2, start_index );
            stmt_objects_->bind_int ( 3, result_count );
        }
        while( stmt_objects_->step() ) {

            object_list_.push_back( didl::DidlObject(
                static_cast<didl::DIDL_CLASS>( stmt_objects_->get_int(0) ) /*cls*/,
                stmt_objects_->get_int(1) /*ROWID*/,
                stmt_objects_->get_int(2) /*parent_id*/,
                stmt_objects_->get_string(3) /*title*/,
                stmt_objects_->get_string(4) /*path*/,
                stmt_objects_->get_int(5) /*mtime*/,
                stmt_objects_->get_int(6) /*object_update_id*/
                )
            );
        }
        return object_list_;
    }

    std::list< didl::DidlContainer > UpnpContentDirectoryDao::containers( const size_t id, const size_t & start_index, const size_t & result_count ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "objects<DidlContainer>::" << id << " (" << start_index << ", " << result_count << ")" );

        std::list< didl::DidlContainer > object_list_;
        std::string query_string_( "select cls, ROWID from tbl_cds_object where parent = ?" );
        query_string_ += " and cls = " + std::to_string( didl::objectContainer);
        if( result_count > 0 ) {
            query_string_ += " limit ?, ? ";
        }

        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "Execute query: " << query_string_ ); //TODO remove
        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_ );
        stmt_objects_->bind_int ( 1, id );
        if( result_count > 0 ) {
            stmt_objects_->bind_int ( 2, start_index );
            stmt_objects_->bind_int ( 3, result_count );
        }
        while( stmt_objects_->step() ) {
            object_list_.push_back( container( stmt_objects_->get_int(1) ) );
        }
        return object_list_;
    }

    std::list< didl::DidlContainerAlbum > UpnpContentDirectoryDao::albums(
            const size_t id, const size_t & start_index, const size_t & result_count ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "objects<DidlContainerAlbum>::" << id << " (" << start_index << ", " << result_count << ")" );

        std::list< didl::DidlContainerAlbum > object_list_;
        try {
            std::string query_string_( "select cls, ROWID from tbl_cds_object where parent = ?" );
            query_string_ += " and cls = " + std::to_string( didl::objectContainerAlbumMusicAlbum );

            if( result_count > 0 ) {
                query_string_ += " limit ?, ? ";
            }
            if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "Execute query: " << query_string_ ); //TODO remove

            db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_ );
            stmt_objects_->bind_int ( 1, id );
            if( result_count > 0 ) {
                stmt_objects_->bind_int ( 2, start_index );
                stmt_objects_->bind_int ( 3, result_count );
            }
            while( stmt_objects_->step() ) {
                object_list_.push_back( album( stmt_objects_->get_int(1) ) );
            }
        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "get albums, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "albums(" + std::to_string( id ) + ") -> DbException: " +  std::string( e.what() ) );
        }
        return object_list_;
    }

    std::list< didl::DidlMovie > UpnpContentDirectoryDao::movies( const size_t id, const size_t & start_index, const size_t & result_count ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "objects<DidlMovie>::" << id << " (" << start_index << ", " << result_count << ")" );

        std::list< didl::DidlMovie > object_list_;
        std::string query_string_( "select cls, ROWID from tbl_cds_object where parent = ?" );
        query_string_ += " and cls = " + std::to_string( didl::objectItemVideoItemMovie );
        if( result_count > 0 ) {
            query_string_ += " limit ?, ? ";
        }

        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "Execute query: " << query_string_ ); //TODO remove
        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_ );
        stmt_objects_->bind_int ( 1, id );
        if( result_count > 0 ) {
            stmt_objects_->bind_int ( 2, start_index );
            stmt_objects_->bind_int ( 3, result_count );
        }
        while( stmt_objects_->step() ) {
            object_list_.push_back( movie( stmt_objects_->get_int(1) ) );
        }
        return object_list_;
    }

    std::list< didl::DidlPhoto > UpnpContentDirectoryDao::photos( const size_t id, const size_t & start_index, const size_t & result_count ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "objects<DidlPhoto>::" << id << " (" << start_index << ", " << result_count << ")" );

        std::list< didl::DidlPhoto > object_list_;
        std::string query_string_( "select cls, ROWID from tbl_cds_object where parent = ?" );
        query_string_ += " and cls = " + std::to_string( didl::objectItemImageItemPhoto );
        if( result_count > 0 ) {
            query_string_ += " limit ?, ? ";
        }

        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "Execute query: " << query_string_ ); //TODO remove
        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_ );
        stmt_objects_->bind_int ( 1, id );
        if( result_count > 0 ) {
            stmt_objects_->bind_int ( 2, start_index );
            stmt_objects_->bind_int ( 3, result_count );
        }
        while( stmt_objects_->step() ) {
            object_list_.push_back( photo( stmt_objects_->get_int(1) ) );
        }
        return object_list_;
    }

    std::list< didl::DidlMusicTrack > UpnpContentDirectoryDao::tracks( const size_t id, const size_t & start_index, const size_t & result_count ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "objects<DidlMusicTrack>::" << id << " (" << start_index << ", " << result_count << ")" );

        std::list< didl::DidlMusicTrack > object_list_;

        std::ostringstream query_string_;
        query_string_<< "select cls, ROWID from tbl_cds_object where parent = ? and cls = ? order by track asc"; //TODO also order by disc
        if( result_count > 0 ) {
            query_string_ << " limit ?, ? ";
        }

        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "Execute query: " << query_string_.str() ); //TODO remove
        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_.str() );
        stmt_objects_->bind_int ( 1, id );
        stmt_objects_->bind_int ( 2, didl::objectItemAudioItemMusicTrack );
        if( result_count > 0 ) {
            stmt_objects_->bind_int ( 3, start_index );
            stmt_objects_->bind_int ( 4, result_count );
        }
        while( stmt_objects_->step() ) {
            object_list_.push_back( track( stmt_objects_->get_int(1) ) );
        }
        return object_list_;
    }

    /* OBJECT BY ID */
    didl::DidlContainer UpnpContentDirectoryDao::container( const size_t id ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "container:" << id );

        size_t child_count_ = objectsCount( didl::object, id );
        db::db_statement_ptr stmt_container_ = _db->prepareStatement (
            "select ROWID, parent, title, path, mtime, object_update_id, 0 from tbl_cds_object where ROWID = ?" );
        stmt_container_->bind_int ( 1, id );

        if( stmt_container_->step() ) {
            return( didl::DidlContainer( stmt_container_->get_int(0), stmt_container_->get_int(1), stmt_container_->get_string(2),
                                         stmt_container_->get_string(3), stmt_container_->get_int(4), stmt_container_->get_int(5),
                                         child_count_ ) );
        } else {
            return didl::DIDL_CONTAINER_EMPTY;
        }
    }

    didl::DidlContainerAlbum UpnpContentDirectoryDao::album( const size_t id ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "album:" << id );

        db::db_statement_ptr stmt_music_track_ = _db->prepareStatement (
            "select ROWID, parent, title, path, mtime, object_update_id, rating, year, playback_count, contributor, artist, genre from tbl_cds_object where ROWID = ?" );
        stmt_music_track_->bind_int ( 1, id );

        if( stmt_music_track_->step() ) {
            return( didl::DidlContainerAlbum(
                        stmt_music_track_->get_int(0) /*ROWID*/,
                        stmt_music_track_->get_int(1) /*parent*/,
                        stmt_music_track_->get_string(2) /*title*/,
                        stmt_music_track_->get_string(3) /*path*/,
                        stmt_music_track_->get_int(4) /*mtime*/,
                        stmt_music_track_->get_int(5) /*object_update_id*/,
                        objectsCount( didl::objectItemAudioItemMusicTrack, id ), /* child_count */
                        stmt_music_track_->get_int(6) /*rating*/,
                        stmt_music_track_->get_int(7) /*year*/,
                        stmt_music_track_->get_int(8) /*playback_count*/,
                        stmt_music_track_->get_string(9) /*contributor*/,
                        stmt_music_track_->get_string(10) /*artist*/,
                        stmt_music_track_->get_string(11) /*genre*/,
                        albumArtUris( id ) ) );
        } else {
            throw db::DbException(404, "no album object found for id " + id );
        }
    }

    didl::DidlMusicTrack UpnpContentDirectoryDao::track( const size_t id ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "track:" << id );

        db::db_statement_ptr stmt_music_track_ = _db->prepareStatement (
            "select ROWID, parent, title, path, mtime, object_update_id, size, mime_type, rating, year, track, playback_count, contributor, artist, genre, album, last_playback_time, import from tbl_cds_object where ROWID = ?" );
        stmt_music_track_->bind_int ( 1, id );


        if( stmt_music_track_->step() ) {
            return( didl::DidlMusicTrack( stmt_music_track_->get_int(0), stmt_music_track_->get_int(1), stmt_music_track_->get_string(2),
                                          stmt_music_track_->get_string(3), stmt_music_track_->get_int(4), stmt_music_track_->get_int(5),
                                          stmt_music_track_->get_int(6), stmt_music_track_->get_string(7), stmt_music_track_->get_int(8),
                                          stmt_music_track_->get_int(9), stmt_music_track_->get_int(10), stmt_music_track_->get_int(11),
                                          stmt_music_track_->get_string(12), stmt_music_track_->get_string(13), stmt_music_track_->get_string(14),
                                          stmt_music_track_->get_string(15), stmt_music_track_->get_int(16),
                                          _resources( std::list< didl::DidlResource::UPNP_RES_ATTRIBUTES >( { //todo
                                                        didl::DidlResource::bitrate,
                                                        didl::DidlResource::bitsPerSample,
                                                        didl::DidlResource::dlnaProfile,
                                                        didl::DidlResource::duration,
                                                        didl::DidlResource::mimeType,
                                                        didl::DidlResource::nrAudioChannels,
                                                        didl::DidlResource::sampleFrequency } ), id ),
                                          stmt_music_track_->get_int(17)==1 ) );
        } else {
            throw db::DbException(404, "no track object found for id " + id );
        }
    }
    didl::DidlMovie UpnpContentDirectoryDao::movie( const size_t id ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "movie:" << id );

        db::db_statement_ptr stmt_movie_ = _db->prepareStatement (
            "select ROWID, parent, title, path, mtime, object_update_id, size, mime_type, import from tbl_cds_object where ROWID = ?" );
        stmt_movie_->bind_int ( 1, id );

        if( stmt_movie_->step() ) {
            return( didl::DidlMovie( stmt_movie_->get_int( 0 ) /*ROWID*/, stmt_movie_->get_int( 1 ) /*parent*/,
                                     stmt_movie_->get_string( 2 ) /* title,*/,
                                     stmt_movie_->get_string( 3 ) /*path*/,
                                     stmt_movie_->get_int( 4 ) /*mtime*/,
                                     stmt_movie_->get_int( 5 ) /*object_update_id*/,
                                     stmt_movie_->get_int( 6 ) /*size*/,
                                     stmt_movie_->get_string( 7 ) /*mime_type*/,
                                     stmt_movie_->get_int(8)==1 ) );
        } else {
            throw 1; //TODO what to throw
        }
    }
    didl::DidlPhoto UpnpContentDirectoryDao::photo( const size_t id ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "photo:" << id );

        db::db_statement_ptr stmt_photo_ = _db->prepareStatement (
            "select ROWID, parent, title, path, mtime, object_update_id, size, mime_type, import from tbl_cds_object where ROWID = ?" );
        stmt_photo_->bind_int ( 1, id );

        if( stmt_photo_->step() ) {
            return( didl::DidlPhoto( stmt_photo_->get_int( 0 ) /*ROWID*/, stmt_photo_->get_int( 1 ) /*parent*/,
                                     stmt_photo_->get_string( 2 ) /* title,*/,
                                     stmt_photo_->get_string( 3 ) /*path*/,
                                     stmt_photo_->get_int( 4 ) /*mtime*/,
                                     stmt_photo_->get_int( 5 ) /*object_update_id*/,
                                     stmt_photo_->get_int( 6 ) /*size*/,
                                     stmt_photo_->get_string( 7 ) /*mime_type*/,
                                     stmt_photo_->get_int(8)==1 ) );
        } else {
            throw upnp::UpnpException( 404, "can not find photo with id: " + std::to_string( id ) );
        }
    }

    /* STORE OBJECTS */
    didl::DidlObject UpnpContentDirectoryDao::save( const didl::DidlObject o ) {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "save object = id:" << o.id() << ", cls:" << o.cls() << ", title:" << o.title() );

        size_t object_id_  = 0;
        db::db_statement_ptr stmt_object_ = _db->prepareStatement ( "select ROWID, object_update_id from tbl_cds_object where path = ?" );
        stmt_object_->bind_text ( 1, o.path() );
        if ( stmt_object_->step() ) {
            object_id_ = stmt_object_->get_int ( 0 );

            db::db_statement_ptr stmt = _db->prepareStatement (
                "update tbl_cds_object SET " \
                     "cls=?, parent=?, size=?, title=?, path=?, mime_type=?, mtime=?, rating=?, year=?, track=?, playback_count=?, object_update_id=?, "\
                     "contributor=?, artist=?, author=?, publisher=?, genre=?, album=?, series=?, dlna_profile=?, last_playback_time=?, import=?, timestamp=? "\
                        "where ROWID = ?" );

            stmt->bind_int ( 1, o._cls );
            stmt->bind_int ( 2, o._parent_id );
            stmt->bind_int ( 3, o._size );
            stmt->bind_text ( 4, o._title );
            stmt->bind_text ( 5, o._path );
            stmt->bind_text ( 6, o._mime_type );
            stmt->bind_int ( 7, o._mtime );
            stmt->bind_int ( 8, o._rating );
            stmt->bind_int ( 9, o._year );
            stmt->bind_int ( 10, o._track );
            stmt->bind_int ( 11, o._playback_count );
            stmt->bind_int( 12, stmt_object_->get_int ( 1 ) + 1 /*object_update_id*/ );
            stmt->bind_text ( 13, o._contributor );
            stmt->bind_text ( 14, o._artist );
            stmt->bind_text ( 15, o._author );
            stmt->bind_text ( 16, o._publisher );
            stmt->bind_text ( 17, o._genre );
            stmt->bind_text ( 18, o._album );
            stmt->bind_text ( 19, o._series );
            stmt->bind_text ( 20, o._dlna_profile );

            stmt->bind_int ( 21, o._last_playback_time );
            stmt->bind_int ( 22, o._import );
            stmt->bind_int ( 23, std::time ( 0 ) );

            // the id in the sql where clause
            stmt->bind_int ( 24, object_id_ );

            stmt->update();

        } else {
            db::db_statement_ptr stmt = _db->prepareStatement (
                "insert into tbl_cds_object( "\
                    "cls, parent, size, title, path, mime_type, mtime, rating, year, track, playback_count, object_update_id, "\
                    "contributor, artist, author, publisher, genre, album, series, dlna_profile, last_playback_time, import, timestamp) "\
                    "values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)" );

            stmt->bind_int ( 1, o._cls );
            stmt->bind_int ( 2, o._parent_id );
            stmt->bind_int ( 3, o._size );
            stmt->bind_text ( 4, o._title );
            stmt->bind_text ( 5, o._path );
            stmt->bind_text ( 6, o._mime_type );
            stmt->bind_int ( 7, o._mtime );
            stmt->bind_int ( 8, o._rating );
            stmt->bind_int ( 9, o._year );
            stmt->bind_int ( 10, o._track );
            stmt->bind_int ( 11, o._playback_count );
            stmt->bind_int( 12, o._object_update_id );
            stmt->bind_text ( 13, o._contributor );
            stmt->bind_text ( 14, o._artist );
            stmt->bind_text ( 15, o._author );
            stmt->bind_text ( 16, o._publisher );
            stmt->bind_text ( 17, o._genre );
            stmt->bind_text ( 18, o._album );
            stmt->bind_text ( 19, o._series );
            stmt->bind_text ( 20, o._dlna_profile );

            stmt->bind_int ( 21, o._last_playback_time );
            stmt->bind_int ( 22, o._import );
            stmt->bind_int ( 23, std::time ( 0 ) );

            stmt->insert();
            object_id_ =  _db->last_insert_rowid();
        }

        std::list< didl::DidlAlbumArtUri > new_album_art_uris_;
        for( auto & uri_ : o._album_art_uri ) {
            new_album_art_uris_.push_back(
                save( didl::DidlAlbumArtUri( uri_.id(), object_id_, uri_.path(), uri_.uri(), uri_.profile() ) )
            );
        }

        std::list< didl::DidlResource > new_resources_;
        for( auto & resource_ : o._item_resource ) {
            new_resources_.push_back(
                _save( didl::DidlResource( resource_.id(), object_id_, resource_.uri(), resource_.protocolInfo(), resource_.attributes() ) )
            );
        }

        return( didl::DidlObject(o._cls, object_id_, o._parent_id, o._title, o._path, o._mtime, 0, 0 /*get child count */, o._size, o._mime_type,
                                 o._rating, o._year, o._track, o._playback_count, o._contributor, o._artist, o._author, o._publisher, o._genre,
                                 o._album, o._series, o._dlna_profile, o._last_playback_time, new_album_art_uris_, new_resources_, o.import() ) );
    }


    /* ARTIST OBJECT */
    didl::DidlContainerArtist UpnpContentDirectoryDao::artist( const std::string name ) {
        std::string clean_name_ = _clean_name( name );
        try {
            if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "artist:" << clean_name_ );

            db::db_statement_ptr stmt_artist_ = _db->prepareStatement (
                "select ROWID, name, clean_name, import from tbl_cds_artist where ROWID = ?" );
            stmt_artist_->bind_text( 1, clean_name_ );

            if( stmt_artist_->step() ) {
                return( didl::DidlContainerArtist( stmt_artist_->get_int( 0 ) /*ROWID*/, 0 /*parent_id*/,
                                         stmt_artist_->get_string( 1 ) /* title,*/,
                                         "" /*path*/,
                                         0 /*mtime*/, 0 /*object_update_id*/,
                                         0 /*child_count*/, stmt_artist_->get_string( 2 ) /*clean_name*/, stmt_artist_->get_int(3) == 1 ) );
            } else {
                return save( didl::DidlContainerArtist( 0, 0, name, "", 0, 0, 0, clean_name_, false ) );
            }
        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "get artist, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "artist(" + clean_name_ + ") -> DbException: " +  std::string( e.what() ) );
        }
    }
    std::list< didl::DidlContainerArtist > UpnpContentDirectoryDao::artists( const size_t & start_index, const size_t & result_count ) {
        try {
            if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "artists:" << start_index << ", " << result_count << ")" );

            db::db_statement_ptr stmt_artists_ = _db->prepareStatement (
                "select ROWID, name, clean_name, import from tbl_cds_artist order by clean_name asc limit ?, ?" );
            stmt_artists_->bind_int( 1, start_index );
            stmt_artists_->bind_int( 2, result_count );

            std::list< didl::DidlContainerArtist > artist_list_;
            while( stmt_artists_->step() ) {
                artist_list_.push_back(
                    didl::DidlContainerArtist( stmt_artists_->get_int( 0 ) /*ROWID*/, 0 /*parent_id*/,
                                               stmt_artists_->get_string( 1 ) /* title,*/,
                                               stmt_artists_->get_string( 3 ) /*path*/,
                                               0 /*mtime*/, 0 /* TODO stmt_artists_->get_int( 3 ) object_update_id*/,
                                               0 /*child_count*/, stmt_artists_->get_string( 2 ) /*clean_name*/, stmt_artists_->get_int( 3 ) /*import*/ ) );
            }
            return artist_list_;
        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "get artists, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(),
                "artist(" + std::to_string( start_index ) + "/" + std::to_string( result_count ) + ") -> DbException: " +  std::string( e.what() ) );
        }
    }
    size_t UpnpContentDirectoryDao::artistsCount() {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "count artists" );

        try {
            db::db_statement_ptr stmt_artist_ = _db->prepareStatement ( "select count(*) from tbl_cds_artist" );
            if ( stmt_artist_->step() ) {
                return stmt_artist_->get_int(0);
            } else return 0;

        } catch ( db::DbException & e ) {
            LOG4CXX_ERROR ( logger, "get artists count, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(),
                "artists count -> DbException: " +  std::string( e.what() ) );
        }
    }

    int UpnpContentDirectoryDao::albumsCount( std::map< std::string, std::string > filters ) const {
        std::ostringstream query_string_;
        query_string_ << "select count(*) from tbl_cds_object where cls = ?";

        if( filters.size() > 0 ) {
            query_string_ << " AND (";
            bool is_first_ = true;
            for(auto & f : filters ) {
                if(is_first_) is_first_=false; else query_string_ << " or ";
                query_string_ << f.first << "=\'" << f.second + "\' ";
            }
        }

        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "Execute query: " << query_string_ ); //TODO remove
        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_.str() );
        stmt_objects_->bind_int( 1, didl::objectContainerAlbumMusicAlbum );
        if( stmt_objects_->step() ) {
            return stmt_objects_->get_int( 0 );
        } else return 0;
    }

    std::list< didl::DidlContainerAlbum > UpnpContentDirectoryDao:: albums(
            const size_t & start_index, const size_t & result_count,
            std::map< std::string, std::string > filters, std::string order ) const {

        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "albums::" << " (" << start_index << ", " << result_count << ")" );

        std::list< didl::DidlContainerAlbum > object_list_;
        std::string query_string_( "select cls, ROWID from tbl_cds_object where" );
        query_string_ += " cls = " + std::to_string( didl::objectContainerAlbumMusicAlbum );

        if( filters.size() > 0 ) {
            query_string_ += " AND (";
            bool is_first_ = true;
            for(auto & f : filters ) {
                if(is_first_) is_first_=false; else query_string_ += " or ";
                query_string_ += f.first + "= \'" + f.second + "\' ";
            }
            query_string_ += ") ";
        }

        query_string_ += " order by " + order;

        if( result_count > 0 ) {
            query_string_ += " limit ?, ? ";
        }

        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "Execute query: " << query_string_ ); //TODO remove
        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_ );
        if( result_count > 0 ) {
            stmt_objects_->bind_int ( 1, start_index );
            stmt_objects_->bind_int ( 2, result_count );
        }
        while( stmt_objects_->step() ) {
            object_list_.push_back( album( stmt_objects_->get_int(1) ) );
        }
        return object_list_;
    }

    didl::DidlContainerArtist UpnpContentDirectoryDao::save( const didl::DidlContainerArtist artist ) {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "save artist = id:" << artist.id() << ", " << artist.title() );

        try {
            size_t artist_id_  = 0;
            db::db_statement_ptr stmt_artist_ = _db->prepareStatement ( "select ROWID from tbl_cds_artist where clean_name = ?" );
            stmt_artist_->bind_text( 1, artist.cleanName() );
            db::db_statement_ptr stmt_ = nullptr;
            if ( stmt_artist_->step() ) {
                artist_id_ = stmt_artist_->get_int ( 0 );

                stmt_ = _db->prepareStatement (
                    "update tbl_cds_artist SET " \
                        "name=?, clean_name=?, import=? "\
                        "where ROWID = ?" );
            } else {

                stmt_ = _db->prepareStatement (
                    "insert into tbl_cds_artist( "\
                    "name, clean_name, import) "\
                        "values (?,?,?)" );
            }

            stmt_->bind_text ( 1, artist.title() );
            stmt_->bind_text ( 2, artist.cleanName() );
            stmt_->bind_int ( 3, artist.import() );

            // the id in the sql where clause
            if( artist_id_ > 0 )
                stmt_->bind_int ( 3, artist_id_ );

            stmt_->update();
            if( artist_id_ == 0 )
                artist_id_ =  _db->last_insert_rowid();

            return( didl::DidlContainerArtist( artist_id_, 0, artist.title(), "", 0,
                                               0, 0 /* TODO child count */,
                                               artist.cleanName(), artist.import() ) );

        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "save or create artist, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "save(DidlContainerArtist) -> DbException: " +  std::string( e.what() ) );
        }
    }

    /* ALBUM ART URI OBJECT */
    didl::DidlAlbumArtUri UpnpContentDirectoryDao::albumArtUri( const size_t id ) {
        try {
            if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "album_art_uri:" << id );

            db::db_statement_ptr stmt_album_art_uri_ = _db->prepareStatement (
                "select ROWID, ref_obj, path, uri, profile from tbl_cds_album_art_uri where ROWID = ?" );
            stmt_album_art_uri_->bind_int ( 1, id );

            if( stmt_album_art_uri_->step() ) {
                return( didl::DidlAlbumArtUri( stmt_album_art_uri_->get_int( 0 ) /*ROWID*/, stmt_album_art_uri_->get_int( 1 ) /*object_ref*/,
                                         stmt_album_art_uri_->get_string( 2 ) /* path,*/,
                                         stmt_album_art_uri_->get_string( 3 ) /*uri*/,
                                         stmt_album_art_uri_->get_string( 4 ) /*profile*/ ) );
            } else {
                throw upnp::UpnpException( 404, "can not find album_art_uri with id: " + std::to_string( id ) );
            }
        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "get album_art_uri, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "album_art_uri(" + std::to_string( id ) + ") -> DbException: " +  std::string( e.what() ) );
        }
    }

    std::list< didl::DidlAlbumArtUri > UpnpContentDirectoryDao::albumArtUris( const size_t album_id ) const {
        try {
            if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "album_art_uri for album:" << album_id );

            db::db_statement_ptr stmt_album_art_uri_ = _db->prepareStatement (
                "select ROWID, ref_obj, path, uri, profile from tbl_cds_album_art_uri where ref_obj = ?" );
            stmt_album_art_uri_->bind_int ( 1, album_id );

            std::list< didl::DidlAlbumArtUri > album_art_uri_list_;
            if( stmt_album_art_uri_->step() ) {
                album_art_uri_list_.push_back(
                    didl::DidlAlbumArtUri( stmt_album_art_uri_->get_int( 0 ) /*ROWID*/, stmt_album_art_uri_->get_int( 1 ) /*object_ref*/,
                                         stmt_album_art_uri_->get_string( 2 ) /* path,*/,
                                         stmt_album_art_uri_->get_string( 3 ) /*uri*/,
                                         stmt_album_art_uri_->get_string( 4 ) /*profile*/ ) );
            }
            return album_art_uri_list_;
        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "get album_art_uri list, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "album_art_uri by album(" + std::to_string( album_id ) + ") -> DbException: " +  std::string( e.what() ) );
        }
    }

    didl::DidlAlbumArtUri UpnpContentDirectoryDao::save( const didl::DidlAlbumArtUri album_art_uri ) {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "save album_art_uri = id:" << album_art_uri.id() << ", " << album_art_uri.path() );

        try {
            size_t album_art_uri_id_  = 0;
            db::db_statement_ptr stmt_album_art_uri_ = _db->prepareStatement ( "select ROWID from tbl_cds_album_art_uri where ROWID = ?" );
            stmt_album_art_uri_->bind_int ( 1, album_art_uri.id() );
            db::db_statement_ptr stmt_ = nullptr;
            if ( stmt_album_art_uri_->step() ) {
                album_art_uri_id_ = stmt_album_art_uri_->get_int ( 0 );

                stmt_ = _db->prepareStatement (
                    "update tbl_cds_album_art_uri SET " \
                        "ref_obj=?, protocol_info?=, uri=?, bitrate=?, bits_per_sample=?, color_depth=?, dlna_profile=?, duration=?, framerate=?, mime_type=?,"\
                        "nr_audio_channels=?, resolution=?, sample_frequency=?, size=? "\
                        "where ROWID = ?" );
            } else {

                stmt_ = _db->prepareStatement (
                    "insert into tbl_cds_album_art_uri( "\
                    "ref_obj, path, uri, profile) "\
                        "values (?,?,?,?)" );
            }

            stmt_->bind_int ( 1, album_art_uri.objectRef() );
            stmt_->bind_text ( 2, album_art_uri.path() );
            stmt_->bind_text ( 3, album_art_uri.uri() );;
            stmt_->bind_text ( 4, album_art_uri.profile() );

            // the id in the sql where clause
            if( album_art_uri_id_ > 0 )
                stmt_->bind_int ( 5, album_art_uri_id_ );

            stmt_->update();
            if( album_art_uri_id_ == 0 )
                album_art_uri_id_ =  _db->last_insert_rowid();

            return( didl::DidlAlbumArtUri( album_art_uri_id_, album_art_uri.objectRef(), album_art_uri.path(), album_art_uri.uri(), album_art_uri.profile() ) );


        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "save or create DidlAlbumArtUri, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "save(DidlAlbumArtUri) -> DbException: " +  std::string( e.what() ) );
        }
    }

    int UpnpContentDirectoryDao::touch( const std::string path, const unsigned long mtime ) {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "touch record with path: " << path );

        try {
            db::db_statement_ptr stmt_ = _db->prepareStatement (
                "update tbl_cds_object SET timestamp=? where path=? and mtime=?" );

            stmt_->bind_int ( 1, std::time ( 0 ) );
            stmt_->bind_text ( 2, path );
            stmt_->bind_int ( 3, mtime );

            stmt_->update();

            return( _db->last_changes_count() );


        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "touch file with path, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "touch(" + path + ") -> DbException: " +  std::string( e.what() ) );
        }
    }

    /* PRVATE METHODS */

    /* DIDL RESOURCE */
    didl::DidlResource UpnpContentDirectoryDao::_save( const didl::DidlResource res ) {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "save resource = id:" << res.id() << ", " << res.uri() );

        try {
            size_t resource_id_  = 0;
            db::db_statement_ptr stmt_resource_ = _db->prepareStatement ( "select ROWID from tbl_cds_resource where ref_obj = ?" );
            stmt_resource_->bind_int ( 1, res.refObj() );
            db::db_statement_ptr stmt = nullptr;
            if ( stmt_resource_->step() ) {
                resource_id_ = stmt_resource_->get_int ( 0 );

                stmt = _db->prepareStatement (
                    "update tbl_cds_resource SET " \
                        "ref_obj=?, protocol_info=?, uri=?, bitrate=?, bits_per_sample=?, color_depth=?, dlna_profile=?, duration=?, framerate=?, mime_type=?,"\
                        "nr_audio_channels=?, resolution=?, sample_frequency=?, size=? "\
                        "where ROWID = ?" );
            } else {

                stmt = _db->prepareStatement (
                    "insert into tbl_cds_resource( "\
                    "ref_obj, protocol_info, uri, bitrate, bits_per_sample, color_depth, dlna_profile, duration, framerate, mime_type, "\
                    "nr_audio_channels, resolution, sample_frequency, size) "\
                        "values (?,?,?,?,?,?,?,?,?,?,?,?,?,?)" );
            }

            stmt->bind_int ( 1, res.refObj() );
            stmt->bind_text ( 2, res.protocolInfo() );
            stmt->bind_text ( 3, res.uri() );

            stmt->bind_text ( 4, ( res.attributes().find( didl::DidlResource::bitrate ) != res.attributes().end() ?
                                    res.attributes()[ didl::DidlResource::bitrate ] : 0 ) );

            stmt->bind_text ( 5, ( res.attributes().find( didl::DidlResource::bitsPerSample ) != res.attributes().end() ?
                                    res.attributes()[ didl::DidlResource::bitsPerSample ] : 0 ) );

            stmt->bind_text ( 6, ( res.attributes().find( didl::DidlResource::colorDepth ) != res.attributes().end() ?
                                    res.attributes()[ didl::DidlResource::colorDepth ] : 0 ) );

            stmt->bind_text( 7, ( res.attributes().find( didl::DidlResource::dlnaProfile ) != res.attributes().end() ?
                                        res.attributes()[ didl::DidlResource::dlnaProfile ] : "" ) );

            stmt->bind_text ( 8, ( res.attributes().find( didl::DidlResource::duration ) != res.attributes().end() ?
                                    res.attributes()[ didl::DidlResource::duration ] : 0 ) );

            stmt->bind_text ( 9, ( res.attributes().find( didl::DidlResource::framerate ) != res.attributes().end() ?
                                    res.attributes()[ didl::DidlResource::framerate] : 0 ) );

            stmt->bind_text ( 10, ( res.attributes().find( didl::DidlResource::mimeType ) != res.attributes().end() ?
                                        res.attributes()[ didl::DidlResource::mimeType ] : "" ) );

            stmt->bind_text ( 11, ( res.attributes().find( didl::DidlResource::nrAudioChannels ) != res.attributes().end() ?
                                    res.attributes()[ didl::DidlResource::nrAudioChannels ] : 0 ) );

            stmt->bind_text ( 12, ( res.attributes().find( didl::DidlResource::resolution ) != res.attributes().end() ?
                                        res.attributes()[ didl::DidlResource::resolution ] : "" ) );

            stmt->bind_text ( 13, ( res.attributes().find( didl::DidlResource::sampleFrequency ) != res.attributes().end() ?
                                    res.attributes()[ didl::DidlResource::sampleFrequency ] : 0 ) );

            stmt->bind_text ( 14, ( res.attributes().find( didl::DidlResource::size ) != res.attributes().end() ?
                                    res.attributes()[ didl::DidlResource::sampleFrequency ] : 0 ) );


            // the id in the sql where clause
            if( resource_id_ > 0 )
                stmt->bind_int ( 15, resource_id_ );

            stmt->update();
            if( resource_id_ == 0 )
                resource_id_ =  _db->last_insert_rowid();

            return( didl::DidlResource( resource_id_, res.refObj(), res.uri(), res.protocolInfo(), res.attributes() ) );


        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "save or create DidlResource, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "save(DidlResource) -> DbException: " +  std::string( e.what() ) );
        }
    }

    didl::DidlResource UpnpContentDirectoryDao::_resource( const size_t resource_id ) {
        try {
            if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "resource:" << resource_id );

            db::db_statement_ptr stmt_resource = _db->prepareStatement (
                "select ROWID, ref_obj, protocol_info, uri, bitrate, bits_per_sample, color_depth, dlna_profile, duration, framerate, mime_type, "\
                "       nr_audio_channels, resolution, sample_frequency, size from tbl_cds_resource where ROWID = ?" );
            stmt_resource->bind_int ( 1, resource_id );

            if( stmt_resource->step() ) {
                return( didl::DidlResource(
                            stmt_resource->get_int(0) /*ROWID*/,
                            stmt_resource->get_int(1) /*ref_id*/,
                            stmt_resource->get_string(2) /*protocol_info*/,
                            stmt_resource->get_string(3) /*uri*/,
                            std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string >( {
                                { didl::DidlResource::bitrate, stmt_resource->get_string(4) /*bitrate*/ },
                                { didl::DidlResource::bitsPerSample, stmt_resource->get_string(5) /*bits_per_sample*/ },
                                { didl::DidlResource::colorDepth, stmt_resource->get_string(6) /*color_depth*/ },
                                { didl::DidlResource::dlnaProfile, stmt_resource->get_string(7) /*dlna_profile*/ },
                                { didl::DidlResource::duration, stmt_resource->get_string(8) /*duration*/ },
                                { didl::DidlResource::framerate, stmt_resource->get_string(9) /*framerate*/ },
                                { didl::DidlResource::mimeType, stmt_resource->get_string(10) /*mime_type*/ },
                                { didl::DidlResource::nrAudioChannels, stmt_resource->get_string(11) /*nr_audio_channels*/ },
                                { didl::DidlResource::resolution, stmt_resource->get_string(12) /*resolution*/ },
                                { didl::DidlResource::sampleFrequency, stmt_resource->get_string(13) /*sample_frequency*/ },
                                { didl::DidlResource::size, stmt_resource->get_string(14) /*size*/ }
                            } ) ) );
            } else {
                throw db::DbException(404, "no resource object found for id " + resource_id );
            }
        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "get or create DidlResources, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException( e.code(), "get(DidlResources) -> DbException: " +  std::string( e.what() ) );
        }
    }

    std::list< didl::DidlResource > UpnpContentDirectoryDao::_resources( const std::list< didl::DidlResource::UPNP_RES_ATTRIBUTES> & names, const size_t item_id ) const {
        if( squawk::DEBUG ) LOG4CXX_TRACE( logger, "resources:" << item_id );

        db::db_statement_ptr stmt_resource_ = _db->prepareStatement (
            "select ROWID, ref_obj, protocol_info, uri, bitrate, bits_per_sample, color_depth, dlna_profile, duration, framerate, mime_type, "\
            "       nr_audio_channels, resolution, sample_frequency, size "\
            "from tbl_cds_resource where ref_obj = ?" );


        stmt_resource_->bind_int ( 1, item_id );


        std::list< didl::DidlResource > resources_;
        while( stmt_resource_->step() ) {
            std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string > attrs_map_;
            for( auto name : names ) {
                switch( name ) {
                    case didl::DidlResource::bitrate: attrs_map_[name] = stmt_resource_->get_string(4); break; /*bitrate*/
                    case didl::DidlResource::bitsPerSample: attrs_map_[name] = stmt_resource_->get_string(5); break; /*bits_per_sample*/
                    case didl::DidlResource::colorDepth: attrs_map_[name] = stmt_resource_->get_string(6); break; /*color_depth*/
                    case didl::DidlResource::dlnaProfile: attrs_map_[name] = stmt_resource_->get_string(7); break; /*dlna_profile*/
                    case didl::DidlResource::duration: attrs_map_[name] = stmt_resource_->get_string(8); break; /*duration*/
                    case didl::DidlResource::framerate: attrs_map_[name] = stmt_resource_->get_string(9); break; /*framerate*/
                    case didl::DidlResource::mimeType: attrs_map_[name] = stmt_resource_->get_string(10); break; /*mime_type*/
                    case didl::DidlResource::nrAudioChannels: attrs_map_[name] = stmt_resource_->get_string(11); break; /*nr_audio_channels*/
                    case didl::DidlResource::resolution: attrs_map_[name] = stmt_resource_->get_string(12); break; /*resolution*/
                    case didl::DidlResource::sampleFrequency: attrs_map_[name] = stmt_resource_->get_string(13); break; /*sample_frequency*/
                    case didl::DidlResource::size: attrs_map_[name] = stmt_resource_->get_string(14); break; /*size*/
                }
            }
            resources_.push_back(
                    didl::DidlResource(
                        stmt_resource_->get_int(0) /*ROWID*/,
                        stmt_resource_->get_int(1) /*ref_id*/,
                        stmt_resource_->get_string(2) /*protocol_info*/,
                        stmt_resource_->get_string(3) /*uri*/, attrs_map_ ) );
        }

        return resources_;
    }

    std::string UpnpContentDirectoryDao::_clean_name( const std::string & name ) {

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
}//namepsace squawk
