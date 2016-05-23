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

#include "squawk.h"

namespace squawk {

/**
 * @brief Squawk config implements a primitive reader/writer for a xml configuration file.
 */
class SquawkConfig {
public:
    SquawkConfig() {}
    ~SquawkConfig() {}

    /** @brief the server visible name. */
    std::string name();
    /** @brief the logger properties files. */
    std::string logger();
    /** @brief the multicast address */
    std::string multicastAddress();
    /** @brief the multicast port */
    int multicastPort();
    /** @brief the http address */
    std::string httpAddress();
    /** @brief the http port */
    int httpPort();
    /** @brief the local listen address */
    std::string localListenAddress();
    /** @brief the directory for temporary files */
    std::string tmpDirectory();
    /** @brief the database file */
    std::string databaseFile();
    /** @brief the web server document root */
    std::string docRoot();
    /** @brief the web server bower root */
    std::string bowerRoot();
    /** @brief the list of media directories */
    std::list< std::string > mediaDirectories();
    /** @brief the configuration file */
    std::string configFile();
    /** @brief the servers uuid */
    std::string uuid();
    /** @brief the cover name*/
    std::list< std::string > coverNames();

    std::map< std::string, std::list< std::string > > getMap() const {
        return store;
    }

    bool rescan = false;

    /** @brief validate the options */
    bool validate();
    /**
     * @brief load the configuration xml file.
     * @param filename
     * @return
     */
    void load(std::string filename);
    /**
     * @brief parse parse the command line options.
     * @param ac options count
     * @param av options strings
     * @return
     */
    bool parse(int ac, const char* av[]);
    /**
     * @brief save the configuration xml file.
     * @param filename
     */
    void save(const std::string & filename) const;

    /**
     * @brief operator write the options to the ostream.
     */
    friend std::ostream& operator <<(std::ostream &os,const SquawkConfig &obj) {
        os <<  "SquawkConfig::" << std::endl;
        for( const auto &iter : obj.store ) {
            os << "\t" << iter.first << " = ";
            for( auto & v : iter.second )
                os << v << " ";
            os << std::endl;
        }
        return os;
    }

private:
#ifdef SQUAWK_RELEASE
    std::string SQUAWK_RELEASE_ = SQUAWK_RELEASE;
#else
    std::string SQUAWK_RELEASE_ = "0.0.0";
#endif
    static const std::string HELP_TEXT;
    std::map< std::string, std::list< std::string > > store;

    /**
     * @brief Add value to store.
     * @param value the new value.
     * @param append Replace the existing value or append the new value to the list.
     *               The new value will be appended if it is not already in the list.
     * #param overwrite Overwrite existing value.When the store already contains a value with the key nothing will be written.
     */
    void setValue( const std::string & key, const std::string & value, bool append = false, bool overwrite = true ) {
        if( store.find( key ) != store.end() && !overwrite ) return;
        if( store.find( key ) != store.end() && append &&
            std::find( std::begin( store[ key ] ), std::end( store[ key ] ), value ) == std::end( store[ key ] ) ) {

            store[ key ].push_back( value );

        } else {
            std::list< std::string > list;
            list.push_back( value );
            store[ key ] = list;
        }
    }

    std::string _get_ip();

    std::string CONFIG_LOGGER_PROPERTIES = "logger";
    std::string CONFIG_MEDIA_DIRECTORIES = "media-directories";
    std::string CONFIG_MEDIA_DIRECTORY = "media-directory";
    std::string CONFIG_COVER_NAMES = "cover-names";
    std::string CONFIG_COVER_NAME = "cover-name";
    std::string CONFIG_MULTICAST_ADDRESS = "multicast-address";
    std::string CONFIG_MULTICAST_PORT = "multicast-port";
    std::string CONFIG_HTTP_IP = "http-ip";
    std::string CONFIG_HTTP_PORT = "http-port";
    std::string CONFIG_DATABASE_FILE = "database-file";
    std::string CONFIG_TMP_DIRECTORY = "tmp-directory";
    std::string CONFIG_LOCAL_LISTEN_ADDRESS = "local-address";
    std::string CONFIG_UUID = "uuid";
    std::string CONFIG_FILE = "config-file";
    std::string CONFIG_HTTP_DOCROOT = "http-docroot";
    std::string CONFIG_HTTP_BOWER = "http-bower";
    std::string CONFIG_SERVER_NAME = "server-name";
};
typedef std::shared_ptr< SquawkConfig > ptr_squawk_config;
}
#endif // SQUAWKCONFIG_H
