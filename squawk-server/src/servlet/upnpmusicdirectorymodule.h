/*
    <one line to give the program's name and a brief idea of what it does.>
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
#ifndef UPNPMUSICDIRECTORYMODULE_H
#define UPNPMUSICDIRECTORYMODULE_H

#include "squawk.h"
#include "../squawkconfig.h"
#include "../db/sqlite3database.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace servlet {

class UpnpMusicDirectoryModule : public ContentDirectoryModule {
public:
    UpnpMusicDirectoryModule(squawk::SquawkConfig * squawk_config, squawk::db::Sqlite3Database * db ) : db(db), squawk_config(squawk_config) {};

    virtual std::string getRootNode();
    virtual bool match(std::map<std::string, std::string> request);
    virtual std::string parseNode(std::map<std::string, std::string> request);
  private:
    static log4cxx::LoggerPtr logger;
    squawk::SquawkConfig * squawk_config;
    squawk::db::Sqlite3Database * db;
};
}}

#endif // UPNPMUSICDIRECTORYMODULE_H
