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
#ifndef UPNPCONTENTDIRECTORYDAO_H
#define UPNPCONTENTDIRECTORYDAO_H

#include "squawkserver.h"

namespace squawk {

/* create the sql string */
template < typename S, typename N >
struct SqlParametersImpl {

    typedef typename boost::fusion::result_of::value_at< S, N >::type current_t;
    typedef typename boost::mpl::next< N >::type next_t;
    typedef boost::fusion::extension::struct_member_name< S, N::value > name_t;

    template < typename Ostream >
    static inline void serialize ( Ostream& os, const bool include_id, const std::string & suffix ) {
        std::string name_ = name_t::call();

        if ( name_ != "_child_count" && name_ != "_item_resource" && name_ != "_album_art_uri" ) {
            if ( name_ == "_id" ) {
                if ( include_id ) {
                    os << "ROWID" << suffix;
                    Comma< S, N >::comma ( os );
                }

            } else {
                os << name_.substr ( 1 ) << suffix;
                Comma< S, N >::comma ( os );
            }
        }

        SqlParametersImpl< S, next_t >::serialize ( os, include_id, suffix );
    }

    template < typename Ostream >
    static inline void placeholders ( Ostream& os, const bool include_id ) {
        std::string name_ = name_t::call();

        if ( name_ != "_child_count" && name_ != "_item_resource" && name_ != "_album_art_uri" ) {
            if ( name_ == "_id" ) {
                if ( include_id ) {
                    os << "?";
                    Comma< S, N >::comma ( os );
                }

            } else {
                os << "?";
                Comma< S, N >::comma ( os );
            }
        }

        SqlParametersImpl< S, next_t >::placeholders ( os, include_id );
    }
};
template < typename S >
struct SqlParametersImpl< S, typename boost::fusion::result_of::size< S >::type > {
    template < typename Ostream >
    static inline void serialize ( Ostream&, const bool, const std::string& ) {}

    template < typename Ostream >
    static inline void placeholders ( Ostream&, const bool ) {}
};
template < typename S >
/**
* @brief create the sql statement name query string.
*/
struct SqlParameters : SqlParametersImpl< S, boost::mpl::int_< 0 > > {};


/* write the sql values */
template <typename BASE, typename T>
struct DidlWriteType {
    static inline void write ( db::Sqlite3Statement * stmt, const BASE&, const std::string & name, int & index, const T & value ) {
        if ( name != "_id" && name != "_child_count" ) {
            stmt->bind_int ( ++index, value );
        }
    }
};
template <typename BASE>
struct DidlWriteType<BASE, std::list<didl::DidlResource>> { //skip DidlResource
    static inline void write ( db::Sqlite3Statement*, const BASE&, const std::string &, int &, const std::list<didl::DidlResource> & ) {}
};
template <typename BASE>
struct DidlWriteType<BASE, std::list<didl::DidlAlbumArtUri>> {
    static inline void write ( db::Sqlite3Statement*, const BASE&, const std::string &, int &, const std::list<didl::DidlAlbumArtUri> & ) {}
};
template <typename BASE>
struct DidlWriteType<BASE, std::string> {
    static inline void write ( db::Sqlite3Statement * stmt, const BASE&, const std::string &, int & index, const std::string & value ) {
        stmt->bind_text ( ++index, value );
    }
};

template < typename S, typename N >
struct DidlBindWriteImpl {

    typedef typename boost::fusion::result_of::value_at< S, N >::type current_t;
    typedef typename boost::mpl::next< N >::type next_t;
    typedef boost::fusion::extension::struct_member_name< S, N::value > name_t;

    template < typename Sqlite3Statement >
    static inline void bind ( Sqlite3Statement * stmt, int & index, S & base, const S & s ) {
        DidlWriteType<S, current_t>::write ( stmt, base, name_t::call(), index, boost::fusion::at< N > ( s ) );
        DidlBindWriteImpl< S, next_t >::bind ( stmt, index, base, s );
    }
};

template < typename S >
struct DidlBindWriteImpl< S, typename boost::fusion::result_of::size< S >::type > {
    template < typename Sqlite3Statement >
    static inline void bind ( Sqlite3Statement*, int &, S &, const S & ) {}
};

template< typename S >
struct DidlBindWrite : DidlBindWriteImpl< S, boost::mpl::int_< 0 > > {};


template < typename T, class Enable = void > struct DidlBind {
    static inline void bind ( const T & ) {}
};

template< typename T >
struct DidlBind<T, typename std::enable_if<std::is_base_of<didl::DidlItem, T>::value>::type> {
    static inline void bind ( const T & s ) {

        db::db_connection_ptr db_ = SquawkServer::instance()->db();

        for ( auto & res : s.audioItemRes() ) {
            size_t resource_id_  = 0;
            db::db_statement_ptr stmt_resource_ = db_->prepareStatement ( "select ROWID from tbl_cds_resource where ref_obj = ?" );
            stmt_resource_->bind_int ( 1, res.refObj() );
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

            stmt->bind_int ( 1, s.id() );
            stmt->bind_text ( 2, res.protocolInfo() );
            stmt->bind_text ( 3, res.path() );

            stmt->bind_text ( 4, ( res.attributes().find ( didl::DidlResource::bitrate ) != res.attributes().end() ?
                                   res.attributes() [ didl::DidlResource::bitrate ] : 0 ) );

            stmt->bind_text ( 5, ( res.attributes().find ( didl::DidlResource::bitsPerSample ) != res.attributes().end() ?
                                   res.attributes() [ didl::DidlResource::bitsPerSample ] : 0 ) );

            stmt->bind_text ( 6, ( res.attributes().find ( didl::DidlResource::colorDepth ) != res.attributes().end() ?
                                   res.attributes() [ didl::DidlResource::colorDepth ] : 0 ) );

            stmt->bind_text ( 7, res.dlnaProfile() );

            stmt->bind_text ( 8, ( res.attributes().find ( didl::DidlResource::duration ) != res.attributes().end() ?
                                   res.attributes() [ didl::DidlResource::duration ] : 0 ) );

            stmt->bind_text ( 9, ( res.attributes().find ( didl::DidlResource::framerate ) != res.attributes().end() ?
                                   res.attributes() [ didl::DidlResource::framerate] : 0 ) );

            stmt->bind_text ( 10, res.mimeType() );

            stmt->bind_text ( 11, ( res.attributes().find ( didl::DidlResource::nrAudioChannels ) != res.attributes().end() ?
                                    res.attributes() [ didl::DidlResource::nrAudioChannels ] : 0 ) );

            stmt->bind_text ( 12, ( res.attributes().find ( didl::DidlResource::resolution ) != res.attributes().end() ?
                                    res.attributes() [ didl::DidlResource::resolution ] : "" ) );

            stmt->bind_text ( 13, ( res.attributes().find ( didl::DidlResource::sampleFrequency ) != res.attributes().end() ?
                                    res.attributes() [ didl::DidlResource::sampleFrequency ] : 0 ) );

            stmt->bind_int ( 14, res.size() );

            // the id in the sql where clause
            if ( resource_id_ > 0 )
            { stmt->bind_int ( 15, resource_id_ ); }

            stmt->update();
        }
    }
};
template<typename T>
struct DidlBind<T, typename std::enable_if<std::is_base_of<didl::DidlContainerAlbum, T>::value>::type> {
    static inline void bind ( const didl::DidlContainerAlbum & s ) {
        for ( auto & album_art_uri : s.albumArtUri() ) {
            size_t album_art_uri_id_  = 0;
            db::db_statement_ptr stmt_album_art_uri_ = SquawkServer::instance()->db()->prepareStatement ( "select ROWID from tbl_cds_album_art_uri where ROWID = ?" );
            stmt_album_art_uri_->bind_int ( 1, album_art_uri.id() );
            db::db_statement_ptr stmt_ = nullptr;

            if ( stmt_album_art_uri_->step() ) {
                album_art_uri_id_ = stmt_album_art_uri_->get_int ( 0 );

                stmt_ = SquawkServer::instance()->db()->prepareStatement (
                            "update tbl_cds_album_art_uri SET " \
                            "ref_obj=?, protocol_info?=, uri=?, bitrate=?, bits_per_sample=?, color_depth=?, dlna_profile=?, duration=?, framerate=?, mime_type=?,"\
                            "nr_audio_channels=?, resolution=?, sample_frequency=?, size=? "\
                            "where ROWID = ?" );

            } else {

                stmt_ = SquawkServer::instance()->db()->prepareStatement (
                            "insert into tbl_cds_album_art_uri( "\
                            "ref_obj, path, uri, profile) "\
                            "values (?,?,?,?)" );
            }

            stmt_->bind_int ( 1, s.id() );
            stmt_->bind_text ( 2, album_art_uri.path() );
            stmt_->bind_text ( 3, album_art_uri.uri() );;
            stmt_->bind_text ( 4, album_art_uri.profile() );

            // the id in the sql where clause
            if ( album_art_uri_id_ > 0 )
            { stmt_->bind_int ( 5, album_art_uri_id_ ); }

            stmt_->update();
        }
    }
};

/* read the sql values */
template <typename BASE, typename T>
struct DidlReadType {
    static inline void read ( db::Sqlite3Statement * stmt, const BASE & base, const std::string & name, int & index, T & value ) {
        if ( name != "_child_count" ) {
            value = stmt->get_int ( index++ );

        } else {
            size_t result_count = 0;
            std::stringstream query_string_;

            if ( DidlType< BASE >::cls() == didl::objectContainerAlbumMusicAlbum ) {
                query_string_ << "select count(*) from tbl_cds_object where cls = ? and parent_id = ?";

            } else {
                query_string_ << "select count(*) from tbl_cds_object where parent_id = ?";
            }

            db::db_statement_ptr stmt_objects_ = SquawkServer::instance()->db()->prepareStatement ( query_string_.str() );
            int count_ = 0;

            if (  DidlType< BASE >::cls() == didl::objectContainerAlbumMusicAlbum )
            { stmt_objects_->bind_int ( ++count_, didl::objectItemAudioItemMusicTrack ); }

            stmt_objects_->bind_int ( ++count_, base.id() );

            while ( stmt_objects_->step() ) {
                result_count = stmt_objects_->get_int ( 0 );
            }

            value = result_count;
        }
    }
};
template <typename BASE>
struct DidlReadType<BASE, didl::DIDL_CLASS> {
    static inline void read ( db::Sqlite3Statement * stmt, const BASE&, const std::string &, int & index, didl::DIDL_CLASS & value ) {
        value = static_cast<didl::DIDL_CLASS> ( stmt->get_int ( index++ ) );
    }
};
template <typename BASE>
struct DidlReadType<BASE, std::string> {
    static inline void read ( db::Sqlite3Statement * stmt, const BASE&, const std::string &, int & index, std::string & value ) {
        value = stmt->get_string ( index++ );
    }
};
template <typename BASE>
struct DidlReadType<BASE, std::list<didl::DidlResource>> {
    static inline void read ( db::Sqlite3Statement *, const BASE & base, const std::string &, int &, std::list<didl::DidlResource> & value ) {
        db::db_statement_ptr stmt_resource = SquawkServer::instance()->db()->prepareStatement (
                "select ROWID, ref_obj, protocol_info, path, bitrate, bits_per_sample, color_depth, dlna_profile, duration, framerate, mime_type, "\
                "       nr_audio_channels, resolution, sample_frequency, size from tbl_cds_resource where ref_obj = ?" );
        stmt_resource->bind_int ( 1, base.id() );

        value.clear();

        while ( stmt_resource->step() ) {
            value.push_back ( didl::DidlResource (
                                  stmt_resource->get_int ( 0 ) /*ROWID*/,
                                  stmt_resource->get_int ( 1 ) /*ref_id*/,
                                  stmt_resource->get_int ( 14 ) /* size */,
                                  "" /* uri */,
                                  stmt_resource->get_string ( 3 ) /*path*/,
                                  stmt_resource->get_string ( 2 ) /*protocol_info*/,
                                  stmt_resource->get_string ( 7 ) /*dlna_profile*/,
                                  stmt_resource->get_string ( 10 ) /*mime_type*/,

            std::map< didl::DidlResource::UPNP_RES_ATTRIBUTES, std::string > ( {
                { didl::DidlResource::bitrate, stmt_resource->get_string ( 4 ) /*bitrate*/ },
                { didl::DidlResource::bitsPerSample, stmt_resource->get_string ( 5 ) /*bits_per_sample*/ },
                { didl::DidlResource::colorDepth, stmt_resource->get_string ( 6 ) /*color_depth*/ },
                { didl::DidlResource::duration, stmt_resource->get_string ( 8 ) /*duration*/ },
                { didl::DidlResource::framerate, stmt_resource->get_string ( 9 ) /*framerate*/ },
                { didl::DidlResource::nrAudioChannels, stmt_resource->get_string ( 11 ) /*nr_audio_channels*/ },
                { didl::DidlResource::resolution, stmt_resource->get_string ( 12 ) /*resolution*/ },
                { didl::DidlResource::sampleFrequency, stmt_resource->get_string ( 13 ) /*sample_frequency*/ }
            } ) ) );
        }
    }
};
template <typename BASE>
struct DidlReadType<BASE, std::list<didl::DidlAlbumArtUri>> { //TODO fill ressource uri
    static inline void read ( db::Sqlite3Statement *, const BASE & base, const std::string &, int &, std::list<didl::DidlAlbumArtUri> & value ) {
        db::db_statement_ptr stmt_album_art_uri_ = SquawkServer::instance()->db()->prepareStatement (
                    "select ROWID, ref_obj, path, uri, profile from tbl_cds_album_art_uri where ref_obj = ?" );
        stmt_album_art_uri_->bind_int ( 1, base.id() );

        value.clear();

        while ( stmt_album_art_uri_->step() ) {
            value.push_back ( didl::DidlAlbumArtUri ( stmt_album_art_uri_->get_int ( 0 ) /*ROWID*/, stmt_album_art_uri_->get_int ( 1 ) /*object_ref*/,
                              stmt_album_art_uri_->get_string ( 2 ) /* path,*/,
                              stmt_album_art_uri_->get_string ( 3 ) /*uri*/,
                              stmt_album_art_uri_->get_string ( 4 ) /*profile*/ ) );
        }
    }
};

template < typename S, typename N >
struct DidlBindReadImpl {

    typedef typename boost::fusion::result_of::value_at< S, N >::type current_t;
    typedef typename boost::mpl::next< N >::type next_t;
    typedef boost::fusion::extension::struct_member_name< S, N::value > name_t;

    template < typename Sqlite3Statement >
    static inline void bind ( Sqlite3Statement * stmt, int & index, S & base, S & s ) {
        DidlReadType<S, current_t>::read ( stmt, s, name_t::call(), index, boost::fusion::at< N > ( s ) );
        DidlBindReadImpl< S, next_t >::bind ( stmt, index, base, s );
    }
};

template < typename S >
struct DidlBindReadImpl< S, typename boost::fusion::result_of::size< S >::type > {
    template < typename Sqlite3Statement >
    static inline void bind ( Sqlite3Statement*, int&, S&, const S& ) {}
};

template < typename S >
struct DidlBindRead : DidlBindReadImpl< S, boost::mpl::int_< 0 > >  {};


inline void parse_attributes( std::stringstream & query_string, const std::map< std::string, std::string > & filters ) {
    bool is_first_ = true;

    for ( auto & f : filters ) {
        if( f.first != "cls" ) {
            if ( is_first_ ) { is_first_=false; } { query_string << " or "; }
            query_string << f.first << "=\'" << f.second + "\' ";
        }
    }
};
inline void parse_filters( std::stringstream & query_string, const std::map< std::string, std::string > & filters ) {
    bool has_filters_;
    if( filters.empty() ) has_filters_ = false;
    else if( filters.find( "cls" ) != filters.end() && filters.size() == 1 ) has_filters_ = false;
    else has_filters_ = true;

    if ( has_filters_ ) {
        query_string << " AND (";
        parse_attributes( query_string, filters );
        query_string << ")";
    }
};
inline void parse_class( didl::DIDL_CLASS cls, const std::string & prefix, std::stringstream & query_string, std::map< std::string, std::string > filters ) { //TODO reference
    if ( cls != didl::object ) {
        query_string << prefix << " cls = " << cls;
    } else if ( filters.find( "cls" ) != filters.end() ) {
        query_string << prefix << " (";
        boost::tokenizer<> classes_( filters["cls"] );
        bool first_ = true;
        for ( boost::tokenizer<>::iterator beg = classes_.begin(); beg != classes_.end(); ++beg ) {
            if( first_ ) first_=false; else query_string << " OR ";
            query_string << "cls = " << *beg;
        }
        query_string << ") ";
    }
}

/** \brief Data persistence access object.
 * TODO do not throw DBException
 */
class UpnpContentDirectoryDao {
public:
    explicit UpnpContentDirectoryDao();

    UpnpContentDirectoryDao ( const UpnpContentDirectoryDao& ) = delete;
    UpnpContentDirectoryDao ( UpnpContentDirectoryDao&& ) = delete;
    UpnpContentDirectoryDao& operator= ( const UpnpContentDirectoryDao& ) = delete;
    UpnpContentDirectoryDao& operator= ( UpnpContentDirectoryDao&& ) = delete;
    ~UpnpContentDirectoryDao() {}

    /** @brief Start database transaction */
    void startTransaction();
    /** @brief End database transaction */
    void endTransaction();

    /**
     * @brief sweep all
     * @param mtime
     */
    void sweep ( long mtime );

    /**
     * Save the didl object
     */
    template< typename T >
    T save ( T o ) {
        if ( squawk::DEBUG ) { LOG4CXX_TRACE( logger, "Save:" << o ); }

        db::db_statement_ptr stmt_object_ = _db->prepareStatement ( "select ROWID, object_update_id from tbl_cds_object where path = ?" );
        stmt_object_->bind_text ( 1, o.path() );

        if ( stmt_object_->step() ) {
            o._id = stmt_object_->get_int ( 0 );
            //update row
            std::stringstream query_string_;
            query_string_ << "update tbl_cds_object SET ";
            SqlParameters< T >::serialize ( query_string_, false, "=?" );
            query_string_ << " where ROWID = ?";

            if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "save object, query_string = \"" << query_string_.str() << "\"" ); }

            db::db_statement_ptr stmt = _db->prepareStatement ( query_string_.str() );
            int index_ = 0;
            DidlBindWrite< T >::bind ( stmt.get(), index_, o, o );
            stmt->bind_int ( ++index_, o._id );
            stmt->update();

        } else {
            // insert row
            std::stringstream query_string_;
            query_string_ << "insert into tbl_cds_object( ";
            SqlParameters< T >::serialize ( query_string_, false, "" );
            query_string_ << " ) values( ";
            SqlParameters< T >::placeholders ( query_string_, false );
            query_string_ << " )";

            if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "update object, query_string = \"" << query_string_.str() << "\"" ); }

            db::db_statement_ptr stmt = _db->prepareStatement ( query_string_.str() );
            int index_ = 0;
            DidlBindWrite< T >::bind ( stmt.get(), index_, o, o );
            stmt->insert();
            o._id = _db->last_insert_rowid();
        }

        DidlBind< T >::bind ( o );
        return o;
    }

    /**
     * \brief get the didl object by id
     */
    template< typename T >
    T object ( const size_t & id ) {
        if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "get_object:" << id ); }

        T o;
        std::stringstream query_string_;
        query_string_ << "select ";
        SqlParameters< T >::serialize ( query_string_, true, "" );
        query_string_ << " from tbl_cds_object where ROWID = ?";

        if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "Execute query (object): " << query_string_.str() ); }

        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_.str() );
        stmt_objects_->bind_int ( 1, id );

        while ( stmt_objects_->step() ) {
            int index_ = 0;
            DidlBindRead< T >::bind ( stmt_objects_.get(), index_, o, o );
        }

        return o;
    }

    /**
     * \brief get the didl object by path
     */
    template< typename T >
    T object ( const std::string & path ) {
        if ( squawk::DEBUG ) { LOG4CXX_DEBUG ( logger, "get_object:" << path ); }

        T o;
        std::stringstream query_string_;
        query_string_ << "select ";
        SqlParameters< T >::serialize ( query_string_, true, "" );
        query_string_ << " from tbl_cds_object where path = ?";

        if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "Execute query (object): " << query_string_.str() ); }

        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_.str() );
        stmt_objects_->bind_text ( 1, path );

        while ( stmt_objects_->step() ) {
            int index_ = 0;
            DidlBindRead< T >::bind ( stmt_objects_.get(), index_, o, o );
        }

        return o;
    }

    template< typename T >
    std::list< T > children ( const size_t id, const size_t & start_index, const size_t & result_count,
                              std::map< std::string, std::string > filters = std::map< std::string, std::string >(),
                              std::pair< std::string, std::string > sort = std::pair< std::string, std::string > { "title", "asc" } ) const {
        if ( squawk::DEBUG )
            LOG4CXX_DEBUG ( logger, "children<" << didl::className ( DidlType< T >::cls() ) <<
                            ">::" << id << " (" << start_index << ", " << result_count << ")" );

        std::list< T > object_list_;


        std::stringstream query_string_;
        query_string_ << "select ";
        SqlParameters< T >::serialize ( query_string_, true, "" );
        query_string_ << " from tbl_cds_object where parent_id = ? ";
        parse_class( DidlType< T >::cls(), " and ", query_string_, filters );
        parse_filters( query_string_, filters );
        query_string_ << " order by " << sort.first << " " << sort.second;

        if ( result_count > 0 ) {
            query_string_ << " limit ?, ? ";
        }

        if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "Execute query (children): " << query_string_.str() ); }

        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_.str() );
        stmt_objects_->bind_int ( 1, id );

        if ( result_count > 0 ) {
            stmt_objects_->bind_int ( 2, start_index );
            stmt_objects_->bind_int ( 3, result_count );
        }

        while ( stmt_objects_->step() ) {
            T item_;
            int index_ = 0;
            DidlBindRead< T >::bind ( stmt_objects_.get(), index_, item_, item_ );
            object_list_.push_back ( item_ );
        }

        return object_list_;
    }

    /**
     * @brief count total child objects by type.
     * @param cls
     * @param parent
     * @return
     */
    size_t childrenCount ( didl::DIDL_CLASS cls, const size_t & parent, std::map< std::string, std::string > filters = std::map< std::string, std::string >() );

    template< typename T >
    std::list< T > objects ( const size_t & start_index, const size_t & result_count,
                             std::map< std::string, std::string > filters = std::map< std::string, std::string >(),
                             std::pair< std::string, std::string > sort = std::pair< std::string, std::string > { "title", "asc" } ) const {
        if ( squawk::DEBUG )
            LOG4CXX_DEBUG ( logger, "objects<" << didl::className ( DidlType< T >::cls() ) <<
                            ">" << " (" << start_index << ", " << result_count << ")" );

        std::list< T > object_list_;
        std::stringstream query_string_;
        query_string_ << "select ";
        SqlParameters< T >::serialize ( query_string_, true, "" );
        query_string_ << " from tbl_cds_object where ";
        parse_class( DidlType< T >::cls(), "", query_string_, filters );
        parse_filters( query_string_, filters );
        query_string_ << " order by " << sort.first << " " << sort.second;

        if ( result_count > 0 ) {
            query_string_ << " limit ?, ? ";
        }

        if ( squawk::DEBUG ) { LOG4CXX_TRACE ( logger, "Execute query (objects): " << query_string_.str() ); }

        db::db_statement_ptr stmt_objects_ = _db->prepareStatement ( query_string_.str() );

        if ( result_count > 0 ) {
            stmt_objects_->bind_int ( 1, start_index );
            stmt_objects_->bind_int ( 2, result_count );
        }

        while ( stmt_objects_->step() ) {
            T item_;
            int index_ = 0;
            DidlBindRead< T >::bind ( stmt_objects_.get(), index_, item_, item_ );
            object_list_.push_back ( item_ );
        }

        return object_list_;
    }

    /**
     * @brief count total objects by type with filter.
     * @param cls the type
     * @param filters filter
     * @return number of objects
     */
    size_t objectsCount ( didl::DIDL_CLASS cls, std::map< std::string, std::string > filters = std::map< std::string, std::string >() );

    /**
     * @brief didl statistics
     * @return
     */
    didl::DidlStatistics statistics();

    /**
     * \brief get or create a artist object.
     */
    didl::DidlContainerArtist artist ( const std::string & clean_name );
    /**
     * @brief count the total numbers of artists.
     * @param filters
     * @return
     */
    size_t artistsCount ( std::map< std::string, std::string > filters = std::map<std::string, std::string >() );
    /**
     * @brief get list of artists
     * @param start_index
     * @param result_count
     * @param filters
     * @param sort
     * @return
     */
    std::list< didl::DidlContainerArtist > artists ( const size_t & start_index, const size_t & result_count,
            std::map< std::string, std::string > filters = std::map<std::string, std::string >(),
            std::pair< std::string, std::string > sort = std::pair<std::string, std::string > ( "title", "asc" ) ) const;
    /**
     * @brief save artist
     * @param artist
     * @return
     */
    didl::DidlContainerArtist save ( const didl::DidlContainerArtist artist );

    /** \brief touch a file in the database.
     * <p>updates the timestamp of the record(s) with the
     *    given path. Returns number of records touched.</p>
     */
    int touch ( const std::string & path, const unsigned long mtime );

    didl::DidlResource save ( const didl::DidlResource resource);

private:
    /* CREATE TABLES */
    const std::list< std::string > CREATE_STATEMENTS {
        "CREATE TABLE IF NOT EXISTS tbl_cds_object( cls, parent_id, size, title, path, mime_type, mtime, rating, year, track, playback_count, object_update_id, contributor, artist, author, publisher, genre, album, series, dlna_profile, last_playback_time, import, timestamp );",
        "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexObjectPath ON tbl_cds_object(path);",
        "CREATE TABLE IF NOT EXISTS tbl_cds_artist( clean_name, name, import );",
        "CREATE UNIQUE INDEX IF NOT EXISTS UniqueIndexArtistCleanName ON tbl_cds_artist(clean_name);",
        "CREATE TABLE IF NOT EXISTS tbl_cds_resource( ref_obj, protocol_info, path, bitrate, bits_per_sample, color_depth, dlna_profile, duration, framerate, mime_type, nr_audio_channels, resolution, sample_frequency, size );",
        "CREATE INDEX IF NOT EXISTS UniqueIndexResourceRef ON tbl_cds_resource(ref_obj);",
        "CREATE TABLE IF NOT EXISTS tbl_cds_album_art_uri( ref_obj, path, uri, profile);",
        "CREATE INDEX IF NOT EXISTS UniqueIndexAlbumArtUriRef ON tbl_cds_album_art_uri(ref_obj);",
    };

    static log4cxx::LoggerPtr logger;

    db::db_connection_ptr _db;
};

typedef std::shared_ptr< squawk::UpnpContentDirectoryDao > ptr_upnp_dao;

} //namespace squawk
#endif // UPNPCONTENTDIRECTORYDAO_H

