#ifndef UPNPIMAGEDIRECTORY_H
#define UPNPIMAGEDIRECTORY_H

#include "squawk.h"
#include "../squawkconfig.h"
#include "../db/sqlite3database.h"
#include <upnp.h>

#include <functional>
#include <iostream>

#include "log4cxx/logger.h"

namespace squawk {
namespace upnp {

class UpnpImageDirectory : public commons::upnp::ContentDirectoryModule {
public:

    UpnpImageDirectory(squawk::SquawkConfig * squawk_config, squawk::db::Sqlite3Database * db ) : db(db), squawk_config(squawk_config) {}
    virtual void getRootNode( commons::xml::XMLWriter * xmlWriter, commons::xml::Node * element, commons::upnp::CdsResult * cds_result );
    virtual bool match( commons::upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual void parseNode( commons::xml::XMLWriter * xmlWriter, commons::upnp::CdsResult * cds_result, commons::upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    squawk::db::Sqlite3Database * db;
    squawk::SquawkConfig * squawk_config;
    static log4cxx::LoggerPtr logger;


};
}}
#endif // UPNPIMAGEDIRECTORY_H
