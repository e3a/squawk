#ifndef UPNPIMAGEDIRECTORY_H
#define UPNPIMAGEDIRECTORY_H

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

class UpnpImageDirectory : public commons::upnp::ContentDirectoryModule {
public:
    UpnpImageDirectory( http::HttpServletContext context ) :
        db(squawk::db::Sqlite3Database::instance().connection( context.parameter( squawk::CONFIG_DATABASE_FILE ) ) ) {}
    virtual void getRootNode( commons::xml::XMLWriter * xmlWriter, commons::xml::Node * element, commons::upnp::CdsResult * cds_result );
    virtual bool match( commons::upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual void parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result, commons::upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    static log4cxx::LoggerPtr logger;
    squawk::db::db_connection_ptr db;
};
}}
#endif // UPNPIMAGEDIRECTORY_H
