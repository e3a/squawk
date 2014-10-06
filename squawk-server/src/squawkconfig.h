/*
    Squawk Config implements a primitive reader for a key/value property file.
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

#ifndef SQUAWKCONFIG_H
#define SQUAWKCONFIG_H

#include <string>
#include <map>

#define CONFIG_UUID "uuid"
#define CONFIG_FILE "config-file"
#define CONFIG_MEDIA_DIRECTORY "media-directory"
#define CONFIG_HTTP_DOCROOT "http-docroot"
#define CONFIG_HTTP_IP "http-ip"
#define CONFIG_HTTP_PORT "http-port"
#define CONFIG_HTTP_THREADS "http-threads"
#define CONFIG_DATABASE_FILE "database-file" 
#define CONFIG_TMP_DIRECTORY "tmp-directory"
#define CONFIG_LOCAL_LISTEN_ADDRESS "local-address"
#define CONFIG_MULTICAST_ADDRESS "multicast-address"
#define CONFIG_MULTICAST_PORT "multicast-port"
#define CONFIG_LOGGER_PROPERTIES "logger"

namespace squawk {

/**
 * Squawk config implements a primitive reader/writer for a key/value property file.
 */
class SquawkConfig {
public:
    SquawkConfig() {};

    bool rescan = false;
    bool exist(std::string key);

    void value(std::string key, std::string value);
    void value(std::string key, int value);
    std::string string_value(std::string key);
    int int_value(std::string key);

    bool validate();
    bool load(std::string filename);
    bool parse(int ac, const char* av[]);
    void save(std::string filename);

private:
  std::map< std::string, std::string > store;
};
}
#endif // SQUAWKCONFIG_H
