/*
    http servlet definition.
    Copyright (C) 2014  <e.knecht@netwings.ch>

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

#ifndef HTTPSERVLET
#define HTTPSERVLET

#include "pcrecpp.h"

namespace http {

/**
 * Abstract class for a HTTP Server.

   Usage:
        You can subclass the HttpServlet and override the HTTP methods that are required.
        The path that is required to create an instance of HttpServlet must be a PCRE regular expression.
        with the get_request_path_element function you can access the groups of the expression:


      An optional sub-pattern that does not exist in the matched string is assigned the empty string.
      Therefore, a number group that does not exist will return in a no match. You can use a string and convert the
      number yourself.

        for example:

                HttpServlet servlet( "/api/user/(\\d+)" );
                servlet.get_request_path_element(0);
 */
class HttpServlet {
private:
	pcrecpp::RE * re = nullptr;
	std::string path;
public:
        static std::map<std::string, std::string> servlet_registry;

	/**
	 * @brief Abstract HttpServlet for implementing your own Servlets.
	 * @param path The Request Path for this servlet
	 */
	HttpServlet ( std::string path );
	/**
	 * @brief Desctructor for the HttpServlet.
	 */
        virtual ~HttpServlet();
	/**
	 * Callback function for the GET method.
	 * @param request The HTTP request path,
	 * @param response The HTTP Response object,
	 */
	bool match ( const std::string & request_path );
	/**
	 * @brief Match the path with parameters in the url
	 * @param request_path The Http request path
	 * @param arg1 Mandatory first parameter
	 * @return
	 */
	template <class FIRST> bool match ( const std::string & request_path, FIRST * arg1 ) {
		return re->FullMatch ( request_path.c_str(), arg1 );
	}
	/**
	 * @brief Match the path with parameters in the url
	 * @param request_path The Http request path
	 * @param arg1 Mandatory first parameter
	 * @param arg2 Optional second parameter
	 * @return
	 */
	template <class FIRST, class SECOND> bool match ( const std::string & request_path, FIRST * arg1, SECOND * arg2 ) {
		return re->FullMatch ( request_path.c_str(), arg1, arg2 );
	}
	/**
	 * @brief Match the path with parameters in the url
	 * @param request_path The Http request path
	 * @param arg1 Mandatory first parameter
	 * @param arg2 Optional second parameter
	 * @param arg3 Optional third parameter
	 * @return
	 */
	template <class FIRST, class SECOND, class THIRD> bool match ( const std::string & request_path, FIRST * arg1, SECOND * arg2, THIRD * arg3 ) {
		return re->FullMatch ( request_path.c_str(), arg1, arg2, arg3 );
	}
	/**
	 * Callback function for the GET method.
	 * @param request The HTTP Request object.
	 * @param response The HTTP Response object.
	 */
	virtual void do_get ( HttpRequest & request, HttpResponse & response );
	/**
	 * Callback function for the POST method.
	 * @param request The HTTP Request object.
	 * @param response The HTTP Response object.
	 */
	virtual void do_post ( HttpRequest & request, HttpResponse & response );
	/**
	 * Callback function for the DELETE method.
	 * @param request The HTTP Request object.
	 * @param response The HTTP Response object.
	 */
	virtual void do_delete ( HttpRequest & request, HttpResponse & response );
	/**
	 * Callback function for the PUT method.
	 * @param request The HTTP Request object.
	 * @param response The HTTP Response object.
	 */
	virtual void do_put ( HttpRequest & request, HttpResponse & response );
	/**
	 * Callback function for the HEAD method.
	 * @param request The HTTP Request object.
	 * @param response The HTTP Response object.
	 */
	virtual void do_head ( HttpRequest & request, HttpResponse & response );
	/**
	 * Callback function for the TRACE method.
	 * @param request The HTTP Request object.
	 * @param response The HTTP Response object.
	 */
	virtual void do_trace ( HttpRequest & request, HttpResponse & response );
	/**
	 * Callback function for the OPTIONS method.
	 * @param request The HTTP Request object.
	 * @param response The HTTP Response object.
	 */
	virtual void do_options ( HttpRequest & request, HttpResponse & response );
	/**
	 * Callback function for the CONNECT method.
	 * @param request The HTTP Request object.
	 * @param response The HTTP Response object.
	 */
	virtual void do_connect ( HttpRequest & request, HttpResponse & response );
	/** TODO remove
	 * Callback function for the other methods.
	 * @param request The HTTP Request object.
	 * @param response The HTTP Response object.
	 */
	virtual void do_default ( const std::string & method, HttpRequest & request, HttpResponse & response );

	/**
	 * Create a stock reply with the given status code.
	 * @brief create_stock_reply
	 * @param status
	 * @param response
	 */
	void create_stock_reply ( http_status status, HttpResponse & response );

	/**
	 * Get the servlet path.
	 * @brief create_stock_reply
	 * @param status
	 * @param response
	 */
	std::string getPath() {
		return path;
	}

	/**
	 * Utility to create a response.
	 * @brief create_response Utility to create a HttpResponse.
	 * @param response The HttpResponse Object
	 * @param http_status The Http Status
	 * @param ss The Stream with the Response Body.
	 * @param content_length The content Length.
	 * @param mime_type The Mime-Type of the Response.
	 */
	void create_response ( HttpResponse & response, ::http::http_status http_status, std::istream * ss, int content_length, mime::MIME_TYPE mime_type );
};

template<typename T> HttpServlet * createT() { return new T; }

typedef std::map<std::string, HttpServlet*(*)()> map_type;

struct ServletFactory {
public:

//    static map_type * map_;
    static HttpServlet * createInstance( std::string const & name) {
        map_type::iterator it = getMap()->find( name );
        if(it == getMap()->end())
            return nullptr;
        return it->second();
    }

// protected:
    static map_type * getMap() {
        // never delete'ed. (exist until program termination)
        // because we can't guarantee correct destruction order
        // if(!map_) { map_ = new map_type; }
        static map_type * map_ = new map_type();
        return map_;
    }

private:
};

template<typename T>
struct ServletRegister : ServletFactory {
    ServletRegister(std::string const & name) {
        getMap()->insert( std::make_pair( name, &createT<T> ) );
    }
};
} //http
#endif // HTTPSERVLET

