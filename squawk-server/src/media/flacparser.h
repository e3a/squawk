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

#ifndef FLACPARSER_H
#define FLACPARSER_H

#include <string>
#include "squawk.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace media {
  
class FlacParser : public MetadataParser {
  public:
    virtual bool parse(struct squawk::media::Audiofile & audiofile, const std::string mime_type, std::string file);
  private:
    static log4cxx::LoggerPtr logger;
};
}}
#endif // FLACPARSER_H
