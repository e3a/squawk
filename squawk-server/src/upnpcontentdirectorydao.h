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

#include <array>

#include "squawk.h"

#include "upnp2.h"
#include "http.h"

#include "db/sqlite3database.h"
#include "db/sqlite3connection.h"
#include "db/sqlite3statement.h"

#include "log4cxx/logger.h"

namespace squawk {

template<class T>
class DidlSequence {
    T value() {
        return std::list< T >();
    }
};

/** \brief Data persistence access object.
 * TODO do not throw DBException
 */
class UpnpContentDirectoryDao {
public:
    explicit UpnpContentDirectoryDao( http::HttpServletContext & context );

/*    UpnpContentDirectoryDao ( const UpnpContentDirectoryDao& ) = delete;
    UpnpContentDirectoryDao ( UpnpContentDirectoryDao&& ) = delete;
    UpnpContentDirectoryDao& operator= ( const UpnpContentDirectoryDao& ) = delete;
    UpnpContentDirectoryDao& operator= ( UpnpContentDirectoryDao&& ) = delete;
    ~UpnpContentDirectoryDao() {}
*/
    /** @brief Start database transaction */
    void startTransaction();
    /** @brief End database transaction */
    void endTransaction();

    template<class T, size_t start_index = 0, size_t result_count = 128 >
    DidlSequence<T> direct_children( const size_t id ) {
        std::cout << "direct_children: " << id << " " << start_index << " " << result_count << std::endl;
    }

    /**
     * @brief sweep all
     * @param mtime
     */
    void sweep ( long mtime );

    std::list< didl::DidlObject > objects( const size_t id, const size_t & start_index, const size_t & result_count ) const;
    std::list< didl::DidlContainer > containers( const size_t id, const size_t & start_index, const size_t & result_count ) const;
    std::list< didl::DidlContainerAlbum > albums( const size_t id, const size_t & start_index, const size_t & result_count ) const;
    std::list< didl::DidlMusicTrack > tracks( const size_t id, const size_t & start_index, const size_t & result_count ) const;
    std::list< didl::DidlMovie > movies( const size_t id, const size_t & start_index, const size_t & result_count ) const;
    std::list< didl::DidlPhoto > photos( const size_t id, const size_t & start_index, const size_t & result_count ) const;

    int albumsCount( std::map< std::string, std::string > filters = std::map< std::string, std::string >() ) const;
    std::list< didl::DidlContainerAlbum > albums( const size_t & start_index, const size_t & result_count,
                                                  std::map< std::string, std::string > filters = std::map< std::string, std::string >(),
                                                  std::string filter = "title" ) const;
    std::list< didl::DidlContainerArtist > artists( const size_t & start_index, const size_t & result_count ) const;

    /** \brief Count all objects with a class definition.
     * <p>The algorithm does not follow the rule of inheritance.
     * All classes match only the exact type and no parent elements.
     * The class Object is a wildcard that will match all objects.<p>
     */
    size_t objectsCount( const didl::DIDL_CLASS cls ) const;
    /** \brief Count all objects with a class definition.
     * <p>The algorithm does not follow the rule of inheritance.
     * All classes match only the exact type and no parent elements.
     * The class Object is a wildcard that will match all objects.<p>
     */
    size_t objectsCount( const didl::DIDL_CLASS cls, const size_t id ) const;

    /**
     * @brief Get a didle container by id.
     * When the container can not be found an empty container will be retunred.
     * @param The id
     * @return  The Container by id or an empty container.
     */
    didl::DidlContainer container( const size_t id ) const;

    didl::DidlContainerAlbum album( const size_t id ) const;
    didl::DidlMusicTrack track( const size_t id ) const;
    didl::DidlMovie movie( const size_t id ) const;
    didl::DidlPhoto photo( const size_t id ) const;

    /** \brief Store object in database
     *  <p>The raw data variables are used for storage. Downcasted objects can be stored completely.</p>
     *  \param o The didl object to store.
     */
    didl::DidlObject save( const didl::DidlObject o );


    /** \brief get or create a artist object. */
    didl::DidlContainerArtist artist( const std::string clean_name );
    std::list< didl::DidlContainerArtist > artists( const size_t & start_index, const size_t & result_count );
    size_t artistsCount();
    didl::DidlContainerArtist save( const didl::DidlContainerArtist artist );


    didl::DidlAlbumArtUri albumArtUri( const size_t id );
    std::list< didl::DidlAlbumArtUri > albumArtUris( const size_t album_id ) const;
    didl::DidlAlbumArtUri save( const didl::DidlAlbumArtUri album_art_uri );

    /** \brief touch a file in the database.
     * <p>updates the timestamp of the record(s) with the
     *    given path. Returns number of records touched.</p>
     */
    int touch( const std::string path, const unsigned long mtime );

private:
    /* CREATE TABLES */
    static const std::list< std::string > CREATE_STATEMENTS;

    static log4cxx::LoggerPtr logger;
    db::db_connection_ptr _db;

    /* handle the didl resource elements */
    didl::DidlResource _save( const didl::DidlResource res );
    didl::DidlResource _resource( const size_t resource_id );
    std::list< didl::DidlResource > _resources( const std::list< didl::DidlResource::UPNP_RES_ATTRIBUTES> & names, const size_t item_id ) const;

    /** @brief get the clean name */
    std::string _clean_name( const std::string & name );
};

typedef std::shared_ptr< squawk::UpnpContentDirectoryDao > ptr_upnp_dao;

} //namespace squawk
#endif // UPNPCONTENTDIRECTORYDAO_H
