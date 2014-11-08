/*
    header file for the media database access object.
    Copyright (C) 2013  <copyright holder> <e.knecht@netwings.ch>

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

#ifndef MEDIADAO_H
#define MEDIADAO_H

#include "log4cxx/logger.h"
#include "../db/sqlite3database.h" //TODO include generic header
#include "../db/database.h"


namespace squawk {
namespace media {

class MediaDao {
public:
    MediaDao(squawk::db::Sqlite3Database * db);
    ~MediaDao();
    void start_transaction();
    void end_transaction();
    bool exist_audiofile(std::string filename, long mtime, long size, bool update);
    bool exist_imagefile(std::string filename, long mtime, long size, bool update);
    squawk::media::Album get_album(std::string path);
    unsigned long save_album(std::string path, squawk::media::Album * album);
    unsigned long save_artist(squawk::media::Artist & artist);
    void save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::media::Song * song);
    unsigned long save_imagefile(std::string filename, long mtime, long size, unsigned long album_id, squawk::media::Image * imagefile);
    void sweep( long mtime );

private:
    static log4cxx::LoggerPtr logger;
    squawk::db::Sqlite3Database * db;
    std::map<std::string, squawk::db::Sqlite3Statement *> stmtMap;
    bool exist_table(std::string table_name);
    void create_table(std::string query);
    bool exist(std::string table, std::string filename, long mtime, long size, bool update);
};
}}
#endif // MEDIADAO_H
