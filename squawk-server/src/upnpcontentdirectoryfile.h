#ifndef UPNPFILEDIRECTORY_H
#define UPNPFILEDIRECTORY_H

#include "squawk.h"

#include "upnpcontentdirectory.h"
#include "upnpcontentdirectorydao.h"

#include "log4cxx/logger.h"

namespace squawk {

class UpnpContentDirectoryFile : public ContentDirectoryModule {
public:
    UpnpContentDirectoryFile( http::HttpServletContext context, ptr_upnp_dao upnp_cds_dao ) :
        _upnp_cds_dao( upnp_cds_dao ), http_address_( context.parameter( squawk::CONFIG_HTTP_IP ) ),
        http_port_( context.parameter( squawk::CONFIG_HTTP_PORT ) ) {}

    virtual int getRootNode( ::didl::DidlWriter * didl_element );
    virtual bool match( upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual std::tuple<size_t, size_t> parseNode( didl::DidlWriter * didl_element, upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    static log4cxx::LoggerPtr logger;
    ptr_upnp_dao _upnp_cds_dao;
    const std::string http_address_;
    const std::string http_port_;
};
}//squawk
#endif // UPNPFILEDIRECTORY_H
