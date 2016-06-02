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

#include "squawkconfig.h"

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

namespace squawk {

const std::string SquawkConfig::HELP_TEXT = "Options description: \n" \
"\t-v [ --version ]         print version string\n" \
"\t--help                   produce help message\n" \
"\t-r [ --rescan ]          rescan database at startup.\n" \
"\t-l [ --logger ] arg      log file. standard out when not set or empty. \n" \
"\t-c [ --config-file ] arg configuration properties file.\n" \
"\t--server-name arg        server visible name\n" \
"\t--media-directory arg    media paths. (comma seperated list)\n" \
"\t--http-ip arg            http server IP.\n" \
"\t--http-port arg          http server port.\n" \
"\t--http-docroot arg       http server docroot.\n" \
"\t--http-bower arg         http server bower components path.\n" \
"\t--http-threads arg       http server threads.\n" \
"\t--database-file arg      database storage file.\n" \
"\t--tmp-directory arg      temporary directory\n" \
"\t--local-address arg      multicast local IP\n" \
"\t--multicast-address arg  multicast address\n" \
"\t--multicast-port arg     multicast port\n " \
"\t--cover-names arg        names to use as cover (without extension)\n ";

std::string SquawkConfig::name() {
    if(  store.find( CONFIG_SERVER_NAME ) != store.end() ) {
        return store[ CONFIG_SERVER_NAME ].front();
    } else return std::string();
}
std::string SquawkConfig::logger() {
    if(  store.find( CONFIG_LOGGER_PROPERTIES ) != store.end() ) {
        return store[ CONFIG_LOGGER_PROPERTIES ].front();
    } else return std::string();
}
std::string SquawkConfig::multicastAddress() {
    return store[ CONFIG_MULTICAST_ADDRESS ].front();
}
int SquawkConfig::multicastPort() {
    return std::stoi( store[ CONFIG_MULTICAST_PORT ].front() );
}
std::string SquawkConfig::httpAddress() {
    return store[ CONFIG_HTTP_IP ].front();
}
int SquawkConfig::httpPort() {
    return std::stoi( store[ CONFIG_HTTP_PORT ].front() );
}
std::string SquawkConfig::localListenAddress() {
    return store[ CONFIG_LOCAL_LISTEN_ADDRESS ].front();
}
std::string SquawkConfig::tmpDirectory() {
    return store[ CONFIG_TMP_DIRECTORY ].front();
}
std::string SquawkConfig::databaseFile() {
    return store[ CONFIG_DATABASE_FILE ].front();
}
std::string SquawkConfig::docRoot() {
    return store[ CONFIG_HTTP_DOCROOT ].front();
}
std::string SquawkConfig::bowerRoot() {
    return store[ CONFIG_HTTP_BOWER ].front();
}
std::list< std::string > SquawkConfig::mediaDirectories() {
    return store[ CONFIG_MEDIA_DIRECTORY ];
}
std::string SquawkConfig::configFile() {
    return store[ CONFIG_FILE ].front();
}
std::string SquawkConfig::uuid() {
    return store[ CONFIG_UUID ].front();
}
std::list< std::string > SquawkConfig::coverNames() {
    return store[ CONFIG_COVER_NAMES];
}

bool SquawkConfig::validate() {
    bool valid = true;
    if(store.find( CONFIG_FILE ) == store.end()) {
        std::cerr << "* the configuration file is not set." << std::endl;
        valid = false;
    } if(store.find( CONFIG_MEDIA_DIRECTORY ) == store.end()) {
        std::cerr << "* the media directory location is not set." << std::endl;
        valid = false;
    } if(store.find( CONFIG_HTTP_DOCROOT ) == store.end()) {
        std::cerr << "* the docroot location is not set." << std::endl;
        valid = false;
    } if(store.find( CONFIG_HTTP_BOWER ) == store.end()) {
        std::cerr << "* the bower location is not set." << std::endl;
        valid = false;
    } if(store.find( CONFIG_DATABASE_FILE ) == store.end()) {
        std::cerr << "* the database file is not set." << std::endl;
        valid = false;
    } if(store.find( CONFIG_TMP_DIRECTORY ) == store.end()) {
        std::cerr << "* the folder for the temporary files is not set." << std::endl;
        valid = false;

    } if(store.find( CONFIG_SERVER_NAME ) == store.end()) {
        setValue(CONFIG_SERVER_NAME, "Squawk Media Server, v" + SQUAWK_RELEASE_ );
    } if(store.find( CONFIG_HTTP_IP ) == store.end()) {
        setValue(CONFIG_HTTP_IP, _get_ip() );
    } if(store.find( CONFIG_LOCAL_LISTEN_ADDRESS ) == store.end()) {
        setValue(CONFIG_LOCAL_LISTEN_ADDRESS, _get_ip() );
    } if(store.find( CONFIG_MULTICAST_ADDRESS ) == store.end()) {
        setValue(CONFIG_MULTICAST_ADDRESS, std::string("239.255.255.250"));
    } if(store.find( CONFIG_MULTICAST_PORT ) == store.end()) {
        setValue(CONFIG_MULTICAST_PORT, "1900");
    } if(store.find( CONFIG_HTTP_PORT ) == store.end()) {
        setValue(CONFIG_HTTP_PORT, "8080");
    } if(store.find( CONFIG_UUID ) == store.end()) {
        uuid_t out;
        uuid_generate_random((unsigned char *)&out);
        char buffer[37];
        uuid_unparse((unsigned char *)&out, buffer);
        setValue( CONFIG_UUID, std::string(buffer) );
    } if(store.find( CONFIG_COVER_NAMES ) == store.end()) {
        setValue( CONFIG_COVER_NAMES, "cover", true, true );
        setValue( CONFIG_COVER_NAMES, "front", true, true );
    }
    return valid;
}
void SquawkConfig::load( std::string filename ) {

    std::ifstream t( filename ); //let the XMLReader load the file.
    std::stringstream buffer;
    buffer << t.rdbuf();

    commons::xml::XMLReader reader( buffer.str() );
    std::vector< commons::xml::Node > root_node = reader.getElementsByName ( "squawk" );
    for( auto & node : root_node[0].children () ) {
        if( node.name() == CONFIG_MEDIA_DIRECTORIES ) {
            for( auto & dir : node.children() ) {
                setValue( CONFIG_MEDIA_DIRECTORY, dir.content(), true, true );
            }
        } else if( node.name() == CONFIG_COVER_NAMES ) {
            for( auto & dir : node.children() ) {
                setValue( CONFIG_COVER_NAMES, dir.content(), true, true );
            }
        } else {
            setValue( node.name(), node.content(), false, false );
        }
    }
}
void SquawkConfig::save(const std::string & filename) const {
    commons::xml::XMLWriter writer;
    commons::xml::Node root_node = writer.element( "squawk" );
    for( auto & itr : store ) {
        if( itr.first ==  CONFIG_MEDIA_DIRECTORY ) {
            commons::xml::Node directories = writer.element( root_node, "", CONFIG_MEDIA_DIRECTORIES );
            for( auto & dir : itr.second ) {
                writer.element( directories, "", itr.first, dir );
            }
        } else if( itr.first ==  CONFIG_COVER_NAMES ) {
            commons::xml::Node directories = writer.element( root_node, "", CONFIG_COVER_NAMES );
            for( auto & dir : itr.second ) {
                writer.element( directories, "", CONFIG_COVER_NAME, dir );
            }
        } else {
            bool is_first_ = true;
            std::stringstream ss;
            for( auto & item_ : itr.second ) {
                if( is_first_ ) { is_first_ = false; }
                else { ss << ","; }
                ss << item_;
            }
            writer.element( root_node, "", itr.first, ss.str() );
        }
    }
    writer.write( filename, true );
}
bool SquawkConfig::parse(int ac, const char* av[]) {
    bool valid = true;
    for(int i=0; i<ac; i++) {
        //search for switches
        if(std::string(av[i]) == std::string("-r") || std::string(av[i]) == std::string("--rescan")) {
             rescan = true;
        } else if(std::string(av[i]) == std::string("-h") || std::string(av[i]) == std::string("--help")) {
            std::cerr << HELP_TEXT << std::endl;
            return false;
        } else if(std::string(av[i]) == std::string("-v") || std::string(av[i]) == std::string("--version")) {
            std::cout << SQUAWK_RELEASE_ << std::endl;
            return false;

        } else if( i + 1 < ac ) {
            if(std::string(av[i]) == std::string("-c") || std::string(av[i]) == std::string("--config-file")) {
                setValue(CONFIG_FILE, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("-l") || std::string(av[i]) == std::string("--logger")) {
                setValue(CONFIG_LOGGER_PROPERTIES, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--server_name")) {
                setValue(CONFIG_SERVER_NAME, std::string(av[++i]), true, true );
            } else if(std::string(av[i]) == std::string("--media-directory")) {
                setValue(CONFIG_MEDIA_DIRECTORY, std::string(av[++i]), true, true );
            } else if(std::string(av[i]) == std::string("--http-ip")) {
                setValue(CONFIG_HTTP_IP, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--http-port")) {
                setValue(CONFIG_HTTP_PORT, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--http-docroot")) {
                setValue(CONFIG_HTTP_DOCROOT, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--http-bower")) {
                setValue(CONFIG_HTTP_BOWER, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--database-file")) {
                setValue(CONFIG_DATABASE_FILE, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--tmp-directory")) {
                setValue(CONFIG_TMP_DIRECTORY, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--local-address")) {
                setValue(CONFIG_LOCAL_LISTEN_ADDRESS, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--multicast-address")) {
                setValue(CONFIG_MULTICAST_ADDRESS, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--multicast-port")) {
                setValue(CONFIG_MULTICAST_PORT, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--cover-names")) {
                typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
                tokenizer tok{ std::string( av[++i] ) };
                for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
                    setValue(CONFIG_COVER_NAMES, *it, true, true );
            }
        } else {
            std::cerr << "parameter not set for key " << std::string(av[i]) << std::endl;
            valid = false;
        }
    }
    return valid;
}
std::string SquawkConfig::_get_ip() {
    std::string ip_ = "127.0.0.1";
    struct ifaddrs* ifAddrStruct = NULL;

    getifaddrs( &ifAddrStruct );
    char addressBuffer[INET_ADDRSTRLEN];
    for( struct ifaddrs* ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next ) {
        if( (ifa->ifa_addr)->sa_family == AF_INET ) {

            void* tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

            inet_ntop( AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN );

            if( strcmp( ifa->ifa_name, "lo" ) ) {
                ip_ = addressBuffer;
            }
        }
    }
    if( ifAddrStruct != NULL ) {
        freeifaddrs( ifAddrStruct );
    }
    return ip_;
}
}//namespace squawk
