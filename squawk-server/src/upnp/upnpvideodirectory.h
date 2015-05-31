/*
    Upnp video directory module.
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef UPNPVIDEODIRECTORY_H
#define UPNPVIDEODIRECTORY_H

#include "squawk.h"

#include "../db/sqlite3database.h"
#include "../db/sqlite3connection.h"
#include "../db/sqlite3statement.h"

#include <http.h>
#include <upnp.h>

#include <functional>
#include <iostream>

#include "log4cxx/logger.h"

namespace squawk {
namespace upnp {

class UpnpVideoDirectory : public commons::upnp::ContentDirectoryModule {
public:
    UpnpVideoDirectory( http::HttpServletContext context ) :
        db(squawk::db::Sqlite3Database::instance().connection( context.parameter( squawk::CONFIG_DATABASE_FILE ) ) ),
        http_address_( context.parameter( squawk::CONFIG_HTTP_IP ) ), http_port_( context.parameter( squawk::CONFIG_HTTP_PORT ) ) {}

    //TODO UpnpVideoDirectory(squawk::SquawkConfig * squawk_config, squawk::db::Sqlite3Database * db ) : db(db), squawk_config(squawk_config) {}
    virtual void getRootNode( commons::xml::XMLWriter * xmlWriter, commons::xml::Node * element, commons::upnp::CdsResult * cds_result );
    virtual bool match( commons::upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual void parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result, commons::upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    static log4cxx::LoggerPtr logger;
    squawk::db::db_connection_ptr db;
    std::string http_address_;
    std::string http_port_;

    /* get video count */
    static constexpr const char * SQL_VIDEO_COUNT = "select count(*) from tbl_cds_files where type=2";
    int videoCount() {
        int count;
        try {
            squawk::db::db_statement_ptr stmt_videos = db->prepareStatement( SQL_VIDEO_COUNT );
            if( stmt_videos->step() ) {
                count = stmt_videos->get_int( 0 );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get video count, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in video count.");
            throw;
        }
        return count;
    }
    /* get videos */
    static constexpr const char * SQL_VIDEOS = "select ROWID, name, filename, mime_type, duration, filesize, sampleFrequency, width, height, bitrate, channels from tbl_cds_files where type=2 order by name";
    void videos( std::function<void(const int, const std::string, const std::string,  const int & duration, const int & size, const int & sample_frequency, const int & width, const int & height, const int & bitrate, const int & channels)> callback ) {
        try {
            squawk::db::db_statement_ptr stmt_videos = db->prepareStatement( SQL_VIDEOS );
            while( stmt_videos->step() ) {
                callback( stmt_videos->get_int( 0 ), stmt_videos->get_string( 2 ), stmt_videos->get_string( 3 ),
                          stmt_videos->get_int( 4 ), stmt_videos->get_int( 5 ), stmt_videos->get_int( 6 ),
                          stmt_videos->get_int( 7 ), stmt_videos->get_int( 8 ), stmt_videos->get_int( 9 ),
                          stmt_videos->get_int( 10 ) );
            }

        } catch( squawk::db::DbException & e ) {
            LOG4CXX_FATAL(logger, "Can not get videos, Exception:" << e.code() << "-> " << e.what());
            throw;
        } catch( ... ) {
            LOG4CXX_FATAL(logger, "Other Excpeption in get video.");
            throw;
        }
    }
};
}}
#endif // UPNPVIDEODIRECTORY_H
