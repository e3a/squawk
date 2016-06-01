/*
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
#include "upnpcontentdirectory.h"

namespace squawk {

/** \brief SOAP cds music containers */
class UpnpContentDirectoryMusic : public ContentDirectoryModule {
public:
    UpnpContentDirectoryMusic() {}
    virtual int getRootNode ( didl::DidlXmlWriter * didl_element );
    virtual bool match ( upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual std::tuple<size_t, size_t> parseNode ( didl::DidlXmlWriter * didl_element, upnp::UpnpContentDirectoryRequest * parseRequest );
};
}//namespace squawk
#endif // UPNPMUSICDIRECTORYMODULE_H
