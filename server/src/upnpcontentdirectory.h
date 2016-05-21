/*
    Copyright (C) 2016  <etienne> <etienne.knecht@hotmail.com>

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
#ifndef UPNPCONTENTDIRECTORY_H
#define UPNPCONTENTDIRECTORY_H

#include "squawk.h"
#include "squawkserver.h"

namespace squawk {

inline std::string http_uri( const std::string & suffix = "" ) {
    std::stringstream ss;
    ss << "http://" << SquawkServer::instance()->config()->httpAddress() << ":" << SquawkServer::instance()->config()->httpPort() << "/" << suffix;
    return ss.str();
}

/**
 * Content Directory Module
 */
class ContentDirectoryModule {
  public:
    /**
     * @brief getRootNode
     * @return the number of objects.
     */
    virtual int getRootNode( didl::DidlXmlWriter * didl_element ) = 0;
    virtual bool match( upnp::UpnpContentDirectoryRequest * parseRequest ) = 0;
    virtual std::tuple<size_t, size_t> parseNode( didl::DidlXmlWriter * didl_element, upnp::UpnpContentDirectoryRequest * parseRequest ) = 0;

    /**
     * @brief Get int from request path.
     * This method parses a path string ( /foo/bar/123 ) and returns the id (123) as int.
     * @param request The UpnpContentDirectoryRequest.
     * @return id the or or 0
     */
    static size_t item_id( upnp::UpnpContentDirectoryRequest * request ) {
        if( request->getValue( upnp::OBJECT_ID ).find( "/" ) != std::string::npos ) {
            std::string str_id_ = request->getValue( upnp::OBJECT_ID ).substr( request->getValue( upnp::OBJECT_ID ).rfind( "/" ) + 1 );
            if( !str_id_.empty() &&
                std::find_if( str_id_.begin(), str_id_.end(),
                    [](char c) { return !std::isdigit(c); } ) == str_id_.end() ) {

                /*Not empty and numeric*/
                return std::stoi( str_id_ );
            } else return 0;
        } else return 0;
    }

    /**
     * @brief Get string from request path.
     * This method parses a path string ( /foo/bar/hello ) and returns the trailing string (hello)
     * @param request The UpnpContentDirectoryRequest.
     * @return id the string or an empty string
     */
    static std::string item_string( upnp::UpnpContentDirectoryRequest * request ) {
        if( request->getValue( upnp::OBJECT_ID ).find( "/" ) == std::string::npos )
            return "";
        return request->getValue( upnp::OBJECT_ID ).substr( request->getValue( upnp::OBJECT_ID ).rfind( "/" ) + 1 );
    }
    /** \brief Evaluates to true when the request.ObjectID starts with id. */
    static bool matchObjectId( upnp::UpnpContentDirectoryRequest * request, const std::string & id ) {
        if( request->contains( "ObjectID" ) && request->getValue( "ObjectID" ).substr(0, id.size() ) == id ) {
            return true;
        } else return false;
    }
    static size_t epoch_time( const size_t year ) {
        struct tm st_year_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        st_year_.tm_hour = 0;   st_year_.tm_min = 0; st_year_.tm_sec = 0;
        st_year_.tm_mon = 0; st_year_.tm_mday = 1;
        st_year_.tm_year = year - 1900;

        time_t tmp_time_ = mktime ( &st_year_ );
        return time_t( tmp_time_ );
    }
};

/**
 * @brief The UpnpContentDirectory class
 * The UPNP content directory class is the factory for all content directory implementations. the implementations
 * are registerered with the register method.
 */
class UpnpContentDirectory : public http::HttpServlet {
public:
    UpnpContentDirectory( const std::string & path ) : HttpServlet( path ) {}

    void registerContentDirectoryModule( std::unique_ptr< ContentDirectoryModule > modules );
    /** TODO doees this work? */
    void registerContentDirectoryModule( std::list< std::unique_ptr< ContentDirectoryModule > > modules );

    virtual void do_post( http::HttpRequest & request, http::HttpResponse & response);
    virtual void do_default( const std::string & method, http::HttpRequest & request, http::HttpResponse & response);

private:
    static log4cxx::LoggerPtr logger;
    std::list< std::unique_ptr< ContentDirectoryModule > > _modules;

    void browse( commons::xml::XMLWriter * xmlWriter, upnp::UpnpContentDirectoryRequest * upnp_command );
    void notify( std::list< int > update_ids );
};
}//namespace squawk
#endif // UPNPCONTENTDIRECTORY_H
