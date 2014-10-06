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


#ifndef SQUAWKSERVICE_H
#define SQUAWKSERVICE_H

#include "squawk.h"

#include "db/squawkdao.h"
#include "squawkconfig.h"

#include "log4cxx/logger.h"

namespace squawk {
class SquawkServiceImpl : public SquawkService {

public:
  SquawkServiceImpl(squawk::db::SquawkDAO * dao, squawk::SquawkConfig * squawk_config) : dao(dao), squawk_config(squawk_config) {};

  virtual void start_transaction();
  virtual void end_transaction();

  bool exist_audiofile(std::string path, unsigned long mtime, unsigned long size);
  bool exist_imagefile(std::string path, unsigned long mtime, unsigned long size);

  virtual std::list< squawk::model::Album > get_albums();
  virtual std::list< squawk::model::Album > get_albums_by_artist(unsigned long artist_id);
  virtual std::list< squawk::model::Artist > get_artists();
  virtual squawk::model::Album get_album(unsigned long id, bool recursive = false);
  virtual squawk::model::Album get_album(std::string path);
  
  virtual unsigned long save_artist(squawk::model::Artist & artist);
  virtual unsigned long save_album(std::string path, squawk::model::Album * album);
  virtual unsigned long save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, squawk::model::Song * song);
  virtual unsigned long save_imagefile(std::string filename, long mtime, long size, unsigned long album_id, squawk::model::Image * imagefile);
  
  std::string getPath(std::string path);

private:
  static log4cxx::LoggerPtr logger;
  squawk::db::SquawkDAO * dao;
  squawk::SquawkConfig * squawk_config;
};
}
#endif // SQUAWKSERVICE_H
