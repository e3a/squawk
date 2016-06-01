#ifndef UPNPCONTENTDIRECTORYAPI_H
#define UPNPCONTENTDIRECTORYAPI_H

#include "squawk.h"
#include "upnpcontentdirectorydao.h"

namespace squawk {

class UpnpContentDirectoryApi : public http::HttpServlet {
public:
    UpnpContentDirectoryApi ( const std::string & path ) : HttpServlet ( path ) {}
    ~UpnpContentDirectoryApi() {}
    virtual void do_get ( http::HttpRequest & request, http::HttpResponse & response ) override;

private:
    FRIEND_TEST ( UpnpContentDirectoryTest, TestAttributes );
    static std::list< std::string > parse_attributes ( const std::string & str );
    FRIEND_TEST ( UpnpContentDirectoryTest, TestFilter );
    static std::map< std::string, std::string > parse_filters ( const std::string & str );
    FRIEND_TEST ( UpnpContentDirectoryTest, TestSort );
    static std::pair< std::string, std::string > parse_sort ( const std::string & str );
};
}//namespace squawk
#endif // UPNPCONTENTDIRECTORYAPI_H
