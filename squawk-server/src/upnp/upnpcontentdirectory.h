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

#include <list>
#include <map>
#include <string>

#include "http.h"
#include "upnp.h"
#include "xml.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace upnp {

/**
 * @brief The UpnpContentDirectory class
 * The UPNP content directory class is the factory for all content directory implementations. the implementations
 * are registerered with the register method.
 */
class UpnpContentDirectory : public ::http::HttpServlet {
public:
    UpnpContentDirectory( const std::string & path, http::HttpServletContext context ) : HttpServlet( path ) {}
    void registerContentDirectoryModule( commons::upnp::ContentDirectoryModule * module);
    virtual void do_post( http::HttpRequest & request, http::HttpResponse & response);
    virtual void do_default( const std::string & method, http::HttpRequest & request, http::HttpResponse & response);
private:
    static log4cxx::LoggerPtr logger;
    void browse( commons::xml::XMLWriter * xmlWriter, commons::upnp::UpnpContentDirectoryRequest * upnp_command );
    std::list< commons::upnp::ContentDirectoryModule *> modules;
};
}}

#endif // UPNPCONTENTDIRECTORY_H
