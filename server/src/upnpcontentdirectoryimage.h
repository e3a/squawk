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
#ifndef UPNPIMAGEDIRECTORY_H
#define UPNPIMAGEDIRECTORY_H

#include "squawk.h"
#include "upnpcontentdirectory.h"

namespace squawk {

class UpnpContentDirectoryImage : public ContentDirectoryModule {
public:
    UpnpContentDirectoryImage() {}

    virtual int getRootNode( ::didl::DidlXmlWriter * didl_element );
    virtual bool match( ::upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual std::tuple<size_t, size_t> parseNode( didl::DidlXmlWriter * didl_element, ::upnp::UpnpContentDirectoryRequest * parseRequest );
};
}//namespace squawk
#endif // UPNPIMAGEDIRECTORY_H
