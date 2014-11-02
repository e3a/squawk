/*
    The UPNP Connection Manager
    Copyright (C) 2013  <etienne> <etienne@mail.com>

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

#ifndef UPNPCONNECTIONMANAGER_H
#define UPNPCONNECTIONMANAGER_H

#include <list>
#include <map>
#include <string>

#include "http.h"
#include "upnp.h"
#include "xml.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace servlet {

/**
 * @brief The UpnpConnectionManager class.
 */
class UpnpConnectionManager : public ::http::HttpServlet {
public:
    UpnpConnectionManager( const std::string path ) : HttpServlet(path) {}
    virtual void do_post(::http::HttpRequest & request, ::http::HttpResponse & response);
private:
    static log4cxx::LoggerPtr logger;
};
}}
#endif // UPNPCONNECTIONMANAGER_H
