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

private:
    static log4cxx::LoggerPtr logger;
};
}
#endif // UPNPIMAGEDIRECTORY_H
