/*
    The UPNP Service Description
    Copyright (C) 2013  <etienne> <etienne@gmail.com>

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

#ifndef UPNPXMLDESCRIPTION_H
#define UPNPXMLDESCRIPTION_H

#include <string>

#include "http.h"
#include "squawk.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace upnp {

/**
 * Create the UPNP XML description for the Device.
 *
 * Certain values are taken from the SquawkConfig class.
 *
 * @brief The UpnpXmlDescription class
 */
class UpnpXmlDescription : public ::http::HttpServlet {
public:
    /**
     * Create the UPNPXmlDescription class.
     * @brief UpnpXmlDescription
     * @param path The servlet path
     * @param config the SquawkConfig class
     */
    UpnpXmlDescription( const std::string & path, http::HttpServletContext context ) : HttpServlet( path ),
        uuid( context.parameter( squawk::CONFIG_UUID ) ) {}
    virtual void do_get(::http::HttpRequest & request, ::http::HttpResponse & response);
private:
    static log4cxx::LoggerPtr logger;
    std::string uuid;
};
}}

#endif // UPNPXMLDESCRIPTION_H
