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

#include <thread>

#include "squawkconfig.h"
#include "media/fileparser.h"
#include "ssdp.h"

/**
 * @brief The SquawkServer Class
 */
class SquawkServer {
  public:
    /**
     * @brief Create new Squawk Server.
     * @param squawk_config The configuration Object.
     */
    SquawkServer(squawk::SquawkConfig * squawk_config) : squawk_config(squawk_config) {}
    virtual ~SquawkServer() {}

    /**
     * @brief Start the Server.
     */
    void start();
    /**
     * @brief Stop the Server.
     */
    void stop();

  private:
    std::thread http_thread;
    std::thread ssdp_thread;
    squawk::SquawkConfig * squawk_config = nullptr;
    squawk::db::Sqlite3Database * database = nullptr;
    squawk::media::FileParser * parser = nullptr;
    http::WebServer * web_server = nullptr;
    squawk::ssdp::SSDPServerImpl * ssdp_server = nullptr;
};

#endif // SQUAWKSERVER_H
