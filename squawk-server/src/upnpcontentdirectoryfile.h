#ifndef UPNPFILEDIRECTORY_H
#define UPNPFILEDIRECTORY_H

#include "squawk.h"
#include "upnpcontentdirectory.h"
#include "upnpcontentdirectorydao.h"

namespace squawk {

class UpnpContentDirectoryFile : public ContentDirectoryModule {
public:
    UpnpContentDirectoryFile() {}

    virtual int getRootNode( ::didl::DidlXmlWriter * didl_element );
    virtual bool match( upnp::UpnpContentDirectoryRequest * parseRequest );
    virtual std::tuple<size_t, size_t> parseNode( didl::DidlXmlWriter * didl_element, upnp::UpnpContentDirectoryRequest * parseRequest );

private:
    static log4cxx::LoggerPtr logger;
};
}//squawk
#endif // UPNPFILEDIRECTORY_H
