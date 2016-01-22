/*
    Upnp music directory module.
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
#ifndef UPNPMUSICDIRECTORYMODULE_H
#define UPNPMUSICDIRECTORYMODULE_H

#include "squawk.h"

#include "upnpcontentdirectory.h"
#include "upnpcontentdirectorydao.h"

#include "log4cxx/logger.h"

namespace squawk {

/** \brief SOAP cds music containers */
class UpnpContentDirectoryMusic : public ContentDirectoryModule {
public:
    UpnpContentDirectoryMusic( http::HttpServletContext context, ptr_upnp_dao upnp_cds_dao ) : _upnp_cds_dao( upnp_cds_dao ),
        _http_uri( "http://" + ( context.containsParameter( squawk::CONFIG_HTTP_IP ) ? context.parameter( squawk::CONFIG_HTTP_IP ) : "0.0.0.0" ) + ":" +
                   ( context.containsParameter( squawk::CONFIG_HTTP_PORT ) ? context.parameter( squawk::CONFIG_HTTP_PORT ) : "80" ) + "/" ) {}

    virtual int getRootNode( didl::DidlWriter * didl_element );
    virtual bool match( upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual std::tuple<size_t, size_t> parseNode( didl::DidlWriter * didl_element, upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    static log4cxx::LoggerPtr logger;
    ptr_upnp_dao _upnp_cds_dao;
    const std::string _http_uri;
};
}
#endif // UPNPMUSICDIRECTORYMODULE_H
