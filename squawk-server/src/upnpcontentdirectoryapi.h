#ifndef UPNPCONTENTDIRECTORYAPI_H
#define UPNPCONTENTDIRECTORYAPI_H

#include "squawk.h"
#include "upnpcontentdirectorydao.h"

namespace squawk {

class UpnpContentDirectoryApi : public http::HttpServlet {
public:
    UpnpContentDirectoryApi( const std::string & path, ptr_upnp_dao dao, didl::SSDPServerImpl * ssdp_server ) :
        HttpServlet ( path ), _dao( dao ), _ssdp_server( ssdp_server ) {}
    ~UpnpContentDirectoryApi() {}
    virtual void do_get ( http::HttpRequest & request, http::HttpResponse & response ) override;


private:
    static log4cxx::LoggerPtr logger;
    ptr_upnp_dao _dao;
    didl::SSDPServerImpl * _ssdp_server;

    FRIEND_TEST(UpnpContentDirectoryTest, TestAttributes);
    static std::list< std::string > parse_attributes( const std::string & str );
    FRIEND_TEST(UpnpContentDirectoryTest, TestFilter);
    static std::map< std::string, std::string > parse_filters( const std::string & str );
    FRIEND_TEST(UpnpContentDirectoryTest, TestSort);
    static std::pair< std::string, std::string > parse_sort( const std::string & str );
};
}//namespace squawk
#endif // UPNPCONTENTDIRECTORYAPI_H
