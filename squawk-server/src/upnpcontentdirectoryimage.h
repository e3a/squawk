#ifndef UPNPIMAGEDIRECTORY_H
#define UPNPIMAGEDIRECTORY_H

#include "squawk.h"

#include "upnpcontentdirectory.h"
#include "upnpcontentdirectorydao.h"

#include "log4cxx/logger.h"

namespace squawk {

class UpnpContentDirectoryImage : public ContentDirectoryModule {
public:
    UpnpContentDirectoryImage( http::HttpServletContext context, ptr_upnp_dao upnp_cds_dao ) : _upnp_cds_dao( upnp_cds_dao ) {}

    virtual int getRootNode( ::didl::DidlWriter * didl_element );
    virtual bool match( ::upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual std::tuple<size_t, size_t> parseNode( didl::DidlWriter * didl_element, ::upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    static log4cxx::LoggerPtr logger;
    ptr_upnp_dao _upnp_cds_dao;
};
}
#endif // UPNPIMAGEDIRECTORY_H
