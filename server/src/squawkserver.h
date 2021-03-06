/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

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

#ifndef SQUAWKSERVER_H
#define SQUAWKSERVER_H

#include "squawkconfig.h"
#include "../../ssdpcpp/src/ssdpserverimpl.h"

namespace squawk {
//forward declaration of class - make it a friend in DidlObject.
class UpnpContentDirectoryDao;
//forward declaration of class - make it a friend in DidlObject.
class UpnpContentDirectoryParser;

/**
 * @brief The SquawkServer Class
 */
class SquawkServer {
  public:
    SquawkServer(SquawkServer const&) = delete;
    SquawkServer & operator=(SquawkServer const&) = delete;
    virtual ~SquawkServer() {}

    static std::shared_ptr< SquawkServer > instance() {
        static auto _server = std::shared_ptr< SquawkServer >( new SquawkServer() );
        return _server;
    }

    /**
     * @brief Start the Server.
     */
    void start( squawk::SquawkConfig * squawk_config );

    /**
     * @brief Stop the Server.
     */
    void stop();

    squawk::ptr_squawk_config config() const { return _squawk_config; }
    std::shared_ptr< squawk::UpnpContentDirectoryDao > dao() const { return _upnp_cds_dao; }
    db::db_connection_ptr db() const {
        return db::Sqlite3Database::instance().connection( _squawk_config->databaseFile() );
    }

    std::map< std::string, upnp::UpnpDevice > upnp_devices () {
        return _ssdp_devices;
    }

    bool import_media_directory();

  private:
    SquawkServer() {}

    void cleanup_upnp_devices();
    void ssdp_event( ssdp::SSDP_EVENT_TYPE, std::string, ssdp::SsdpEvent device );

    std::map< std::string, upnp::UpnpDevice > _ssdp_devices;
    std::mutex _ssdp_devices_mutex;
    std::unique_ptr<std::thread> _ssdp_devices_thread;
    bool _ssdp_devices_thread_run = true;

    std::shared_ptr< squawk::UpnpContentDirectoryParser > _upnp_file_parser;
    std::thread _upnp_file_parser_thread;

    squawk::ptr_squawk_config _squawk_config;
    std::shared_ptr< squawk::UpnpContentDirectoryDao > _upnp_cds_dao;
    std::shared_ptr< ssdp::SSDPServerImpl > _ssdp_server;
    std::shared_ptr< http::WebServer > web_server;
};
}//namespace squawk
#endif // SQUAWKSERVER_H
