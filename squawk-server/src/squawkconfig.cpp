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

#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <uuid/uuid.h>


//TODO remove
#include <unistd.h>


#define HELP_TEXT std::string("Options description: \n" \
"\t-v [ --version ]         print version string\n" \
"\t--help                   produce help message\n" \
"\t-r [ --rescan ]          rescan database at startup.\n" \
"\t-l [ --logger ] arg      logger properties file. \n" \
"\t-c [ --config-file ] arg configuration properties file.\n" \
"\t--media-directory arg    media path.\n" \
"\t--http-ip arg            http server IP.\n" \
"\t--http-port arg          http server port.\n" \
"\t--http-docroot arg       http server docroot.\n" \
"\t--http-threads arg       http server threads.\n" \
"\t--database-file arg      database storage file.\n" \
"\t--tmp-directory arg      temporary directory\n" \
"\t--local-address arg      multicast local IP\n" \
"\t--multicast-address arg  multicast address\n" \
"\t--multicast-port arg     multicast port\n ")

namespace squawk {

bool SquawkConfig::exist(std::string key) {
  return ( store.find(key) != store.end());
}
void SquawkConfig::value(std::string key, std::string value) {
  store[key] = value;
}
void SquawkConfig::value(std::string key, int value) {
  std::ostringstream ss;
  ss << value;
  store[key] = ss.str();
}
std::string SquawkConfig::string_value(std::string key) {
  return store[key];
}
int SquawkConfig::int_value(std::string key) {
  return atoi(store[key].c_str());
}


bool SquawkConfig::validate() {
    bool valid = true;
    if(! exist(CONFIG_FILE)) {
        std::cerr << "* the configuration file is not set." << std::endl;
        valid = false;
    } if(! exist(CONFIG_MEDIA_DIRECTORY)) {
        std::cerr << "* the media directory location is not set." << std::endl;
        valid = false;
    } if(! exist(CONFIG_HTTP_DOCROOT)) {
        std::cerr << "* the docroot location is not set." << std::endl;
        valid = false;
    } if(! exist(CONFIG_DATABASE_FILE)) {
        std::cerr << "* the database file is not set." << std::endl;
        valid = false;
    } if(! exist(CONFIG_TMP_DIRECTORY)) {
        std::cerr << "* the folder for the temporary files is not set." << std::endl;
        valid = false;

    } if(! exist(CONFIG_HTTP_IP)) {
        value(CONFIG_HTTP_IP, std::string("0.0.0.0"));
    } if(! exist(CONFIG_LOCAL_LISTEN_ADDRESS)) {
        value(CONFIG_LOCAL_LISTEN_ADDRESS, std::string("0.0.0.0"));
    } if(! exist(CONFIG_MULTICAST_ADDRESS)) {
        value(CONFIG_MULTICAST_ADDRESS, std::string("239.255.255.250"));
    } if(! exist(CONFIG_MULTICAST_PORT)) {
        value(CONFIG_MULTICAST_PORT, 1900);
    } if(! exist(CONFIG_HTTP_PORT)) {
        value(CONFIG_HTTP_PORT, 8080);
    } if(! exist(CONFIG_HTTP_THREADS)) {
        value(CONFIG_HTTP_THREADS, 20);
    } if(! exist(CONFIG_UUID)) {
        uuid_t out;
        uuid_generate_random((unsigned char *)&out);
        char buffer[37];
        uuid_unparse((unsigned char *)&out, buffer);
        value(CONFIG_UUID, std::string(buffer));
    }
    return valid;
}

bool SquawkConfig::load(std::string filename) {

    size_t size;
    char *buffer = new char[255];
    buffer = getcwd( buffer, size);
    std::cout << "current dir: " << buffer << std::endl;




  std::filebuf fb;
  if (fb.open (filename.c_str(), std::ios::in)) {
    std::istream is(&fb);
    
    for (std::string line; std::getline(is, line); ) {
      int pos = line.find("=");
      if(pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos+1, line.length());
        if(! exist(key)) {
            store[key] = value;
        }
      }
    }
    fb.close();
    return true;
  }
  return false;
}
void SquawkConfig::save(std::string filename) {
  std::ofstream file;
  file.open( filename.c_str());
  if (file.is_open()) {
    for(std::map< std::string, std::string >::iterator iter = store.begin(); iter != store.end(); ++iter) {
      file << iter->first << "=" << iter->second << std::endl;
    }
    file.close();
  } else {
    std::cerr << "can not open configuration file at:" << filename << std::endl;
    throw -1;
  }
}
bool SquawkConfig::parse(int ac, const char* av[]) {
    std::stringstream buffer;
    bool valid = true;
    for(int i=0; i<ac; i++) {
        //search for switches
        if(std::string(av[i]) == std::string("-r") || std::string(av[i]) == std::string("--rescan")) {
             rescan = true;
        } else if(std::string(av[i]) == std::string("-h") || std::string(av[i]) == std::string("--help")) {
            std::cerr << HELP_TEXT << std::endl;
            return false;
        } else if(std::string(av[i]) == std::string("-v") || std::string(av[i]) == std::string("--version")) {
            std::cerr << 1 << std::endl; //TODO output version
            return false;

        } else if( i + 1 < ac ) {
            if(std::string(av[i]) == std::string("-c") || std::string(av[i]) == std::string("--config-file")) {
                value(CONFIG_FILE, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("-l") || std::string(av[i]) == std::string("--logger")) {
                value(CONFIG_LOGGER_PROPERTIES, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--media-directory")) {
                value(CONFIG_MEDIA_DIRECTORY, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--http-ip")) {
                value(CONFIG_HTTP_IP, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--http-port")) {
                value(CONFIG_HTTP_PORT, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--http-docroot")) {
                value(CONFIG_HTTP_DOCROOT, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--http-threads")) {
                value(CONFIG_HTTP_THREADS, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--database-file")) {
                value(CONFIG_DATABASE_FILE, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--tmp-directory")) {
                value(CONFIG_TMP_DIRECTORY, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--local-address")) {
                value(CONFIG_LOCAL_LISTEN_ADDRESS, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--multicast-address")) {
                value(CONFIG_MULTICAST_ADDRESS, std::string(av[++i]));
            } else if(std::string(av[i]) == std::string("--multicast-port")) {
                value(CONFIG_MULTICAST_PORT, std::string(av[++i]));
            }
        } else {
            std::cerr << "parameter not set for key " << std::string(av[i]) << std::endl;
            valid = false;
        }
    }
    return valid;
}
}
