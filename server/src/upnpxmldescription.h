/*
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

#include "squawk.h"

namespace squawk {

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
    UpnpXmlDescription ( const std::string & path ) : HttpServlet ( path ) {}
    virtual void do_get ( ::http::HttpRequest & request, ::http::HttpResponse & response );
};
}//namespace squawk
#endif // UPNPXMLDESCRIPTION_H
