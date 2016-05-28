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
#include "upnpcontentdirectoryparser.h"
#include "squawkserver.h"

namespace squawk {
log4cxx::LoggerPtr UpnpContentDirectoryDao::logger ( log4cxx::Logger::getLogger ( "squawk.UpnpContentDirectoryDao" ) );

/* CONSTRUCTOR */
UpnpContentDirectoryDao::UpnpContentDirectoryDao() : _db ( SquawkServer::instance()->db() ) {

    //create tables if they dont exist
    for ( auto & stmt : CREATE_STATEMENTS ) {
        if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "create table:" << stmt ); }

        try {
            _db->prepareStatement ( stmt )->step();

        } catch ( db::DbException & e ) {
            LOG4CXX_FATAL ( logger, "create table, Exception:" << e.code() << "-> " << e.what() );
            throw upnp::UpnpException ( e.code(), "DbException: " +  std::string ( e.what() ) );
        }
    }
}

/* TRANSACTION */
void UpnpContentDirectoryDao::startTransaction() {
    try {
        _db->exec ( "BEGIN;" );

    } catch ( db::DbException & e ) {
        LOG4CXX_FATAL ( logger, "create statements, Exception:" << e.code() << "-> " << e.what() );
        throw upnp::UpnpException ( e.code(), "DbException: " +  std::string ( e.what() ) );
    }
}
void UpnpContentDirectoryDao::endTransaction() {
    try {
        _db->exec ( "END;" );

    } catch ( db::DbException & e ) {
        LOG4CXX_FATAL ( logger, "create statements, Exception:" << e.code() << "-> " << e.what() );
        throw upnp::UpnpException ( e.code(), "DbException: " +  std::string ( e.what() ) );
    }
}

/* SWEEP DATABASE */
void UpnpContentDirectoryDao::sweep ( long mtime ) {
    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_DEBUG ( logger, "sweepDatabase:" << mtime ); }

    db::db_statement_ptr stmt = _db->prepareStatement ( "delete from tbl_cds_object where timestamp < ?" );
    stmt->bind_int ( 1, mtime );
    stmt->update();

//        squawk::db::db_statement_ptr stmt_delete_album = _db->prepareStatement ( squawk::sql::DELETE_ALBUM );
//        stmt_delete_album->update();

//        squawk::db::db_statement_ptr stmt_orphan_artist_mappings = _db->prepareStatement ( squawk::sql::DELETE_ORPHAN_ARTIST_MAPPINGS );
//        stmt_orphan_artist_mappings->update();

//        squawk::db::db_statement_ptr stmt_orphan_artist = _db->prepareStatement ( squawk::sql::DELETE_ORPHAN_ARTIST );
//        stmt_orphan_artist->update();
}

size_t UpnpContentDirectoryDao::objectsCount ( didl::DIDL_CLASS cls, std::map< std::string, std::string > filters ) {
    if ( squawk::SUAWK_SERVER_DEBUG )
    { LOG4CXX_DEBUG ( logger, "objectsCount<" << didl::className ( cls ) ); }

    size_t result_count = 0;
    std::stringstream query_string_;
    query_string_ << "select count(*) from tbl_cds_object where cls = ?";

    if ( ! filters.empty() ) {
        query_string_ << " AND (";
        parse_attributes ( query_string_, filters );
        query_string_ << ")";
    }

    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "Execute query (objectsCount): " << query_string_.str() ); }

    db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_.str() );
    stmt_objects_->bind_int ( 1, cls );

    while ( stmt_objects_->step() ) {
        result_count = stmt_objects_->get_int ( 0 );
    }

    return result_count;
}

size_t UpnpContentDirectoryDao::childrenCount ( didl::DIDL_CLASS cls, const size_t & parent, std::map< std::string, std::string > filters ) {
    if ( squawk::SUAWK_SERVER_DEBUG )
    { LOG4CXX_DEBUG ( logger, "childrenCount<" << didl::className ( cls ) << ">::parent=" << parent ); }

    size_t result_count = 0;
    std::stringstream query_string_;
    query_string_ << "select count(*) from tbl_cds_object where parent_id = ? ";
    parse_class ( cls, " and ", query_string_, filters );
    parse_filters ( query_string_, filters );

    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "Execute query (childrenCount): " << query_string_.str() ); }

    db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_.str() );
    stmt_objects_->bind_int ( 1, parent );

    while ( stmt_objects_->step() ) {
        result_count = stmt_objects_->get_int ( 0 );
    }

    return result_count;
}

didl::DidlStatistics UpnpContentDirectoryDao::statistics() {

    int albums_count_, artist_count_;
    std::map< std::string, int> audiofile_types_;
    std::map< std::string, int > file_types_;

    try {
        //Get the Albums Count
        db::db_statement_ptr stmt_albums = _db->prepareStatement ( "select count(*) from tbl_cds_object where cls=2" );

        while ( stmt_albums->step() ) {
            albums_count_ = stmt_albums->get_int ( 0 );
        }

        //Get the Artist Count
        db::db_statement_ptr stmt_artists = _db->prepareStatement ( "select count(*) from tbl_cds_artist" );

        while ( stmt_artists->step() ) {
            artist_count_ = stmt_artists->get_int ( 0 );
        }

        //Get the Files Count
        db::db_statement_ptr stmt_audiofiles = _db->prepareStatement ( "select mime_type, count(mime_type) from tbl_cds_resource GROUP BY mime_type" );

        while ( stmt_audiofiles->step() ) {
            audiofile_types_[ stmt_audiofiles->get_string ( 0 ) ] = stmt_audiofiles->get_int ( 1 );
        }

        //Get the Files Count
        db::db_statement_ptr stmt_type = _db->prepareStatement ( "select cls, count(cls) from tbl_cds_object where cls > 0 GROUP BY cls" );

        while ( stmt_type->step() ) {
            std::string type_ = "TEXT";

            switch ( stmt_type->get_int ( 0 ) ) {
            case didl::objectContainer:
                type_ = "FOLDER";
                break;

            case didl::objectContainerAlbumMusicAlbum:
                type_ = "ALBUM";
                continue;

            case didl::objectContainerPersonMusicArtist:
                type_ = "ARTIST";
                continue;

            case didl::objectItem:
                type_ = "ITEM";
                break;

            case didl::objectItemAudioItemMusicTrack:
                type_ = "AUDIO_TRACK";
                break;

            case didl::objectItemImageItemPhoto:
                type_ = "IMAGE";
                break;

            case didl::objectItemVideoItemMovie:
                type_ = "MOVIE";
                break;
            }

            file_types_[ type_ ] = stmt_type->get_int ( 1 );
        }

    } catch ( db::DbException & e ) {
        LOG4CXX_FATAL ( logger, "Can not get statistic, Exception:" << e.code() << "-> " << e.what() );
        throw;

    } catch ( ... ) {
        LOG4CXX_FATAL ( logger, "Other Excpeption in get_statistic." );
        throw;
    }

    return didl::DidlStatistics ( albums_count_, artist_count_, audiofile_types_, file_types_ );
}

/* ARTIST OBJECT */
didl::DidlContainerArtist UpnpContentDirectoryDao::artist ( const std::string & name ) {
    std::string clean_name_ = UpnpContentDirectoryParser::_clean_name ( name );

    try {
        if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "artist:" << clean_name_ ); }

        db::db_statement_ptr stmt_artist_ = _db->prepareStatement (
                                                "select ROWID, name, clean_name, import from tbl_cds_artist where ROWID = ?" );
        stmt_artist_->bind_text ( 1, clean_name_ );

        if ( stmt_artist_->step() ) {
            return ( didl::DidlContainerArtist ( stmt_artist_->get_int ( 0 ) /*ROWID*/, 0 /*parent_id*/,
                                                 stmt_artist_->get_string ( 1 ) /* title,*/,
                                                 "" /*path*/,
                                                 0 /*mtime*/, 0 /*object_update_id*/,
                                                 0 /*child_count*/, stmt_artist_->get_string ( 2 ) /*clean_name*/, stmt_artist_->get_int ( 3 ) == 1 ) );

        } else {
            return save ( didl::DidlContainerArtist ( 0, 0, name, "", 0, 0, 0, clean_name_, false ) );
        }

    } catch ( db::DbException & e ) {
        LOG4CXX_FATAL ( logger, "get artist, Exception:" << e.code() << "-> " << e.what() );
        throw upnp::UpnpException ( e.code(), "artist(" + clean_name_ + ") -> DbException: " +  std::string ( e.what() ) );
    }
}
std::list< didl::DidlContainerArtist > UpnpContentDirectoryDao::artists ( const size_t & start_index, const size_t & result_count,
        std::map< std::string, std::string > filters, std::pair< std::string, std::string > sort ) const {

    try {
        if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "artists:" << start_index << ", " << result_count << ")" ); }

        std::stringstream query_string_;
        query_string_ << "select ROWID, name, clean_name, import from tbl_cds_artist";

        if ( ! filters.empty() ) {
            query_string_ << " WHERE ";
            parse_attributes ( query_string_, filters );
        }

        query_string_ << " order by " << sort.first << " " << sort.second;

        if ( result_count > 0 ) {
            query_string_ << " limit ?, ? ";
        }

        db::db_statement_ptr stmt_artists_ = _db->prepareStatement ( query_string_.str() );

        if ( result_count > 0 ) {
            stmt_artists_->bind_int ( 1, start_index );
            stmt_artists_->bind_int ( 2, result_count );
        }

        std::list< didl::DidlContainerArtist > artist_list_;

        while ( stmt_artists_->step() ) {
            db::db_statement_ptr stmt_albums_artists_ = _db->prepareStatement (
                        "select count(*) from tbl_cds_object where cls=? and (artist = ? or publisher=?)" );
            stmt_albums_artists_->bind_int ( 1, didl::objectContainerAlbumMusicAlbum );
            stmt_albums_artists_->bind_text ( 2, stmt_artists_->get_string ( 2 ) );
            stmt_albums_artists_->bind_text ( 3, stmt_artists_->get_string ( 2 ) );
            int albums_count_ = 0;

            if ( stmt_albums_artists_->step() ) {
                albums_count_ = stmt_albums_artists_->get_int ( 0 );
            }

            artist_list_.push_back (
                didl::DidlContainerArtist ( stmt_artists_->get_int ( 0 ) /*ROWID*/, 0 /*parent_id*/,
                                            stmt_artists_->get_string ( 1 ) /* title,*/,
                                            "" /*path*/,
                                            0 /*mtime*/, 0 /*object_update_id*/,
                                            albums_count_ /*child_count*/, stmt_artists_->get_string ( 2 ) /*clean_name*/,
                                            stmt_artists_->get_int ( 3 ) /*import*/ ) );
        }

        return artist_list_;

    } catch ( db::DbException & e ) {
        LOG4CXX_FATAL ( logger, "get artists, Exception:" << e.code() << "-> " << e.what() );
        throw upnp::UpnpException ( e.code(),
                                    "artist(" + std::to_string ( start_index ) + "/" + std::to_string ( result_count ) + ") -> DbException: " +  std::string ( e.what() ) );
    }
}

size_t UpnpContentDirectoryDao::artistsCount ( std::map< std::string, std::string > filters ) {
    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "count artists" ); }

    try {
        std::stringstream query_string_;
        query_string_ << "select count(*) from tbl_cds_artist";

        if ( ! filters.empty() ) {
            query_string_ << " WHERE ";
            parse_attributes ( query_string_, filters );
        }

        db::db_statement_ptr stmt_artist_ = _db->prepareStatement ( query_string_.str() );

        if ( stmt_artist_->step() ) {
            return stmt_artist_->get_int ( 0 );

        } else { return 0; }

    } catch ( db::DbException & e ) {
        LOG4CXX_ERROR ( logger, "get artists count, Exception:" << e.code() << "-> " << e.what() );
        throw upnp::UpnpException ( e.code(),
                                    "artists count -> DbException: " +  std::string ( e.what() ) );
    }
}

didl::DidlContainerArtist UpnpContentDirectoryDao::save ( const didl::DidlContainerArtist artist ) {
    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "save artist = id:" << artist.id() << ", " << artist.title() ); }

    try {
        size_t artist_id_  = 0;
        db::db_statement_ptr stmt_artist_ = _db->prepareStatement ( "select ROWID from tbl_cds_artist where clean_name = ?" );
        stmt_artist_->bind_text ( 1, artist.cleanName() );
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
        if ( artist_id_ > 0 )
        { stmt_->bind_int ( 4, artist_id_ ); }

        stmt_->update();

        if ( artist_id_ == 0 )
        { artist_id_ =  _db->last_insert_rowid(); }

        return ( didl::DidlContainerArtist ( artist_id_, 0, artist.title(), "", 0,
                                             0, 0, artist.cleanName(), artist.import() ) );

    } catch ( db::DbException & e ) {
        LOG4CXX_FATAL ( logger, "save or create artist, Exception:" << e.code() << "-> " << e.what() );
        throw upnp::UpnpException ( e.code(), "save(DidlContainerArtist) -> DbException: " +  std::string ( e.what() ) );
    }
}

didl::DidlResource UpnpContentDirectoryDao::save ( const didl::DidlResource resource ) {
    db::db_connection_ptr db_ = SquawkServer::instance()->db();

    size_t resource_id_  = 0;
    db::db_statement_ptr stmt_resource_ = db_->prepareStatement ( "select ROWID from tbl_cds_resource where ref_obj = ?" );
    stmt_resource_->bind_int ( 1, resource.refObj() );
    db::db_statement_ptr stmt = nullptr;

    if ( stmt_resource_->step() ) {
        resource_id_ = stmt_resource_->get_int ( 0 );

        stmt = db_->prepareStatement (
                   "update tbl_cds_resource SET " \
                   "ref_obj=?, protocol_info=?, path=?, bitrate=?, bits_per_sample=?, color_depth=?, dlna_profile=?, duration=?, framerate=?, mime_type=?,"\
                   "nr_audio_channels=?, resolution=?, sample_frequency=?, size=? "\
                   "where ROWID = ?" );

    } else {

        stmt = db_->prepareStatement (
                   "insert into tbl_cds_resource( "\
                   "ref_obj, protocol_info, path, bitrate, bits_per_sample, color_depth, dlna_profile, duration, framerate, mime_type, "\
                   "nr_audio_channels, resolution, sample_frequency, size) "\
                   "values (?,?,?,?,?,?,?,?,?,?,?,?,?,?)" );
    }

    stmt->bind_int ( 1, resource.refObj() );
    stmt->bind_text ( 2, resource.protocolInfo() );
    stmt->bind_text ( 3, resource.path() );

    stmt->bind_text ( 4, ( resource.attributes().find ( didl::DidlResource::bitrate ) != resource.attributes().end() ?
                           resource.attributes() [ didl::DidlResource::bitrate ] : 0 ) );

    stmt->bind_text ( 5, ( resource.attributes().find ( didl::DidlResource::bitsPerSample ) != resource.attributes().end() ?
                           resource.attributes() [ didl::DidlResource::bitsPerSample ] : 0 ) );

    stmt->bind_text ( 6, ( resource.attributes().find ( didl::DidlResource::colorDepth ) != resource.attributes().end() ?
                           resource.attributes() [ didl::DidlResource::colorDepth ] : 0 ) );

    stmt->bind_text ( 7, resource.dlnaProfile() );

    stmt->bind_text ( 8, ( resource.attributes().find ( didl::DidlResource::duration ) != resource.attributes().end() ?
                           resource.attributes() [ didl::DidlResource::duration ] : 0 ) );

    stmt->bind_text ( 9, ( resource.attributes().find ( didl::DidlResource::framerate ) != resource.attributes().end() ?
                           resource.attributes() [ didl::DidlResource::framerate] : 0 ) );

    stmt->bind_text ( 10, resource.mimeType() );

    stmt->bind_text ( 11, ( resource.attributes().find ( didl::DidlResource::nrAudioChannels ) != resource.attributes().end() ?
                            resource.attributes() [ didl::DidlResource::nrAudioChannels ] : 0 ) );

    stmt->bind_text ( 12, ( resource.attributes().find ( didl::DidlResource::resolution ) != resource.attributes().end() ?
                            resource.attributes() [ didl::DidlResource::resolution ] : "" ) );

    stmt->bind_text ( 13, ( resource.attributes().find ( didl::DidlResource::sampleFrequency ) != resource.attributes().end() ?
                            resource.attributes() [ didl::DidlResource::sampleFrequency ] : 0 ) );

    stmt->bind_int ( 14, resource.size() );

    // the id in the sql where clause
    if ( resource_id_ > 0 )
    { stmt->bind_int ( 15, resource_id_ ); }

    stmt->update();

    if ( resource_id_ == 0 )
    { resource_id_ = SquawkServer::instance()->db()->last_insert_rowid(); }

    return didl::DidlResource ( resource_id_, resource.refObj(), resource.size(), resource.uri(),
                                resource.path(), resource.protocolInfo(), resource.dlnaProfile(),
                                resource.mimeType(), resource.attributes() );
}

std::list< didl::DidlContainer > UpnpContentDirectoryDao::series ( const size_t & start_index, const size_t & result_count,
        std::map< std::string, std::string > filters, std::pair< std::string, std::string > sort ) const {

    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "series:" << start_index << ", " << result_count << ")" ); }

    std::stringstream query_string_;
    query_string_ << "select distinct series_title from tbl_cds_object where series_title!='' ";

    if ( ! filters.empty() ) {
        query_string_ << " AND( ";
        parse_attributes ( query_string_, filters );
        query_string_ << " ) ";
    }

    query_string_ << " order by " << sort.first << " " << sort.second;

    if ( result_count > 0 ) {
        query_string_ << " limit ?, ? ";
    }

    db::db_statement_ptr stmt_series_ = _db->prepareStatement ( query_string_.str() );

    if ( result_count > 0 ) {
        stmt_series_->bind_int ( 1, start_index );
        stmt_series_->bind_int ( 2, result_count );
    }

    std::list< didl::DidlContainer > series_list_;

    while ( stmt_series_->step() ) {
        db::db_statement_ptr stmt_series_count_ = _db->prepareStatement (
                    "select count(*) from tbl_cds_object where series_title = ?" );
        stmt_series_count_->bind_text ( 1, stmt_series_->get_string( 0 ) );
        int series_count_ = 0;

        if ( stmt_series_count_->step() ) {
            series_count_ = stmt_series_count_->get_int ( 0 );
        }

        series_list_.push_back (
            didl::DidlContainer ( 0 /*ROWID*/, 0 /*parent_id*/,
                                  stmt_series_->get_string ( 0 ) /* title,*/,
                                        "" /*path*/,
                                        0 /*mtime*/, 0 /*object_update_id*/,
                                        series_count_ /*child_count*/, true ) );
    }

    return series_list_;
}

size_t UpnpContentDirectoryDao::seriesCount ( std::map< std::string, std::string > filters ) const {

    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "seriesCount:" ); }

    std::stringstream query_string_;
    if( filters.empty() ) {
        query_string_ << "select count(distinct series_title) from tbl_cds_object";
    } else {
        query_string_ << "select count(series_title) from tbl_cds_object";
        query_string_ << " WHERE ";
        parse_attributes ( query_string_, filters );
    }

    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "Execute query (seriesCount): " << query_string_.str() ); }

    db::db_statement_ptr stmt_series_ = _db->prepareStatement ( query_string_.str() );

    int result_ = 0;
    if ( stmt_series_->step() ) {
        result_ = stmt_series_->get_int( 0 );
    }
    return result_;

}

int UpnpContentDirectoryDao::touch ( const std::string & path, const unsigned long mtime ) {
    if ( squawk::SUAWK_SERVER_DEBUG ) { LOG4CXX_TRACE ( logger, "touch record with path: " << path ); }

    try {
        db::db_statement_ptr stmt_ = _db->prepareStatement (
                                         "update tbl_cds_object SET timestamp=? where path=? and mtime=?" );

        stmt_->bind_int ( 1, std::time ( 0 ) );
        stmt_->bind_text ( 2, path );
        stmt_->bind_int ( 3, mtime );

        stmt_->update();
        int last_changes_count_ = _db->last_changes_count();
        return ( last_changes_count_ );

    } catch ( db::DbException & e ) {
        LOG4CXX_FATAL ( logger, "touch file with path, Exception:" << e.code() << "-> " << e.what() );
        throw upnp::UpnpException ( e.code(), "touch(" + path + ") -> DbException: " +  std::string ( e.what() ) );
    }
}
}//namepsace squawk
