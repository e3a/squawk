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
#ifndef UPNPCONTENTDIRECTORY_H
#define UPNPCONTENTDIRECTORY_H

#include <string>
#include <map>
#include "tinyxml.h"

#include "http.h"
#include "squawk.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace servlet {

/**
 * @brief The UpnpContentDirectory class
 * The UPNP content directory class is the factory for all content directory implementations. the implementations
 * are registerered with the register method.
 */
class UpnpContentDirectory : public ::http::HttpServlet {
public:
    UpnpContentDirectory( const std::string path ) : HttpServlet(path) {};
    void registerContentDirectoryModule(ContentDirectoryModule * module);
    virtual void do_post(::http::HttpRequest & request, ::http::HttpResponse & response);
    std::string parseRequest(std::string r);
  private:
    static log4cxx::LoggerPtr logger;
    std::string invoke(std::string method, std::map<std::string, std::string> parameters);
    std::string browse(std::map<std::string, std::string> parameters);
    std::list<ContentDirectoryModule *> modules;
    void readAttributes(TiXmlElement * element, std::map<std::string, std::string> * namespaces, std::map<std::string, std::string> * attributes);
};
}}

#endif // UPNPCONTENTDIRECTORY_H
