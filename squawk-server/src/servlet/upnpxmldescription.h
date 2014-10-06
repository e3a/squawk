#ifndef UPNPXMLDESCRIPTION_H
#define UPNPXMLDESCRIPTION_H

#include <string>

#include "http.h"
#include "../squawkconfig.h"

#include "tinyxml.h"

#include "log4cxx/logger.h"

namespace squawk {
namespace servlet {

class UpnpXmlDescription : public ::http::HttpServlet {
public:
    UpnpXmlDescription( const std::string path, SquawkConfig * config ) : HttpServlet(path), config(config) {};
    virtual void do_get(::http::HttpRequest & request, ::http::HttpResponse & response);
private:
    static log4cxx::LoggerPtr logger;
    SquawkConfig * config;
    std::string cds();
    std::string device();
    TiXmlElement * create_element(TiXmlElement * parent, std::string name, std::string value);
};
}}

#endif // UPNPXMLDESCRIPTION_H
