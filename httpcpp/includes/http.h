/*
    Datastructures and utility classes for the HTTP server.
    Copyright (C) 2013  <copyright holder> <email>

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

#ifndef HTTP_H
#define HTTP_H

#include <array>
#include <map>
#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <time.h>
#include <functional>

#include "mimetypes.h"
#include "commons.h"

#include "pcrecpp.h"

#define HTTP_HEADER_CONTENT_LENGTH 	std::string("Content-Length") 	// The length of the response body in octets (8-bit bytes)
#define HTTP_HEADER_CONTENT_TYPE 	std::string("Content-Type") 	//
#define HTTP_HEADER_DATE            std::string("Date")             // The date and time that the message was sent (in "HTTP-date" format as defined by RFC 2616)
#define HTTP_HEADER_LAST_MODIFIED   std::string("Last-Modified")    // The last modified date for the requested object (in "HTTP-date" format as defined by RFC 2616)
#define HTTP_HEADER_EXPIRES         std::string("Expires")          // Gives the date/time after which the response is considered stale
#define HTTP_HEADER_HOST 		std::string("Host")		// The domain name of the server (for virtual hosting), and the TCP port number on which the server is listening
#define HTTP_HEADER_IF_MODIFIED_SINCE std::string("If-Modified-Since")	// Allows a 304 Not Modified to be returned if content is unchanged
#define HTTP_HEADER_CACHE_CONTROL 	std::string("Cache-Control")	// Used to specify directives that MUST be obeyed by all caching mechanisms along the request/response chain
									// (Cache-Control: no-cache, max-age=0, max-age=n [seconds])
#define HTTP_HEADER_CONTENT_DISPOSITION 	std::string("Content-Disposition")	// An opportunity to raise a "File Download" dialogue box for a known MIME type with binary format or suggest a filename for dynamic content. Quotes are necessary with special characters.
#define HTTP_HEADER_CONNECTION      std::string("Connection")       // What type of connection the user-agent would prefer

namespace http {
    /**
     * The http status types.
     * @brief http status types.
     */
    enum class http_status {
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NO_CONTENT = 204,
        PARTIAL_CONTENT = 206,
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        MOVED_TEMPORARILY = 302,
        NOT_MODIFIED = 304,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503
  };
inline http_status parse_status(const int & status) {
    switch( status ) {
    case 200: return http_status::OK;
    case 201: return http_status::CREATED;
    case 202: return http_status::ACCEPTED;
    case 204: return http_status::NO_CONTENT;
    case 206: return http_status::PARTIAL_CONTENT;
    case 300: return http_status::MULTIPLE_CHOICES;
    case 301: return http_status::MOVED_PERMANENTLY;
    case 302: return http_status::MOVED_TEMPORARILY;
    case 304: return http_status::NOT_MODIFIED;
    case 400: return http_status::BAD_REQUEST;
    case 401: return http_status::UNAUTHORIZED;
    case 403: return http_status::FORBIDDEN;
    case 404: return http_status::NOT_FOUND;
    case 500: return http_status::INTERNAL_SERVER_ERROR;
    case 501: return http_status::NOT_IMPLEMENTED;
    case 502: return http_status::BAD_GATEWAY;
    case 503: return http_status::SERVICE_UNAVAILABLE;
    default: return http_status::BAD_REQUEST;
    }
}
inline int parse_status(const http_status & status) {
    switch( status ) {
    case http_status::OK: return 200;
    case http_status::CREATED: return 201;
    case http_status::ACCEPTED: return 202;
    case http_status::NO_CONTENT: return 204;
    case http_status::PARTIAL_CONTENT: return 206;
    case http_status::MULTIPLE_CHOICES: return 300;
    case http_status::MOVED_PERMANENTLY: return 301;
    case http_status::MOVED_TEMPORARILY: return 302;
    case http_status::NOT_MODIFIED: return 304;
    case http_status::BAD_REQUEST: return 400;
    case http_status::UNAUTHORIZED: return 401;
    case http_status::FORBIDDEN: return 403;
    case http_status::NOT_FOUND: return 404;
    case http_status::INTERNAL_SERVER_ERROR: return 500;
    case http_status::NOT_IMPLEMENTED: return 501;
    case http_status::BAD_GATEWAY: return 502;
    case http_status::SERVICE_UNAVAILABLE: return 503;
    default: return 400;
    }
}
    inline std::tuple<int, int> parseRange( const std::string & range ) {
        std::string clean_range = commons::string::trim( range );
        if( commons::string::starts_with( clean_range, "bytes=") ) {
            clean_range = commons::string::trim( clean_range.substr(6) );
            int start = 0; int end = -1;
            if( clean_range.find( "-") != std::string::npos ) {
                std::string from = clean_range.substr( 0, clean_range.find( "-") );
                start = commons::string::parse_string<int>( from );
                if( clean_range.find( "-" )+1 != clean_range.size() ) {
                    std::string to = clean_range.substr( clean_range.find( "-")+1, clean_range.size() );
                    end = commons::string::parse_string<int>( to );
                }
                return std::tuple<int, int>(start, end);
            }
        }
        return std::tuple<int, int>(0, -1);
    }

   /**
    * @brief Http Request class.
    */
    struct HttpRequest {
        HttpRequest() :
            request_method(std::string("")), uri(std::string("")), protocol(""), body(std::string("")),
            client_ip(std::string("")), http_version_major(0), http_version_minor(0),
            request_lines(std::map< std::string, std::string >()),
            parameters(std::map< std::string, std::string >()) {}

        std::string request_method, uri, protocol, body, client_ip;
        int http_version_major, http_version_minor;
        std::map< std::string, std::string > request_lines;
        std::map< std::string, std::string > parameters;

        bool is_persistent() { //TODO check standard
            return http_version_major == 1 && http_version_minor == 1 &&
                    request_lines.find( HTTP_HEADER_CONNECTION ) != request_lines.end() &&
                    request_lines[ HTTP_HEADER_CONNECTION ] == "keep-alive";
        }

        friend std::ostream& operator<<(std::ostream& out, const http::HttpRequest & request) {
            out << request.request_method << " " << request.uri << " " << request.http_version_major << "." << request.http_version_minor << "\n";
            out << "RequestLines:\n";
            for( auto request_line : request.request_lines ) {
                out << "\t" << request_line.first << ": " << request_line.second << "\n";
            }
            if( request.parameters.size() > 0 ) {
                out << "Parameters:\n";
                for( auto parameter : request.parameters ) {
                    out << "\t" << parameter.first << ": " << parameter.second << "\n";
                }
            }
            return out;
        }
    };

  /**
   * @brief The HttpResponse class
   */
  class HttpResponse {
  public:
      HttpResponse() {}
      ~HttpResponse() {
          if( body_istream ) {
              delete body_istream; //TODO unique pointer
          }
      };

      size_t get_size() {
          return size;
      }
      void set_status(http_status status) {
          this->status = status;
      }
      void add_header(std::string key, std::string value) {
          headers[key] = value;
      }
      std::string get_header(std::string key) {
          return headers[key];
      }
      bool hasHeader(const std::string & key) {
          return( headers.find( key ) != headers.end() );
      }
      /**
       * @brief reset the HttpResponse.
       * Delete the Stream and Header Map.
       */
      void reset();
      /**
       * @brief set expires in seconds.
       * @param seconds
       * Set the expire time in seconds. if not set no header will be added.
       */
      void set_expires(int seconds);
      /**
       * @brief set last modified date
       * @param time
       * Last modified date. if not set no header will be added.
       */
      void set_last_modified(time_t last_modified);
      /**
       * @brief set_mime_type
       * @param type
       */
      void set_mime_type(mime::MIME_TYPE type);

      http_status status;
      std::string protocol;
      short http_version_major;
      short http_version_minor;
      std::string remote_ip;

      void set_content( std::istream & is );
      void produce( std::array< char, 8192 > buffer, size_t size );

      size_t fill_buffer( char * buffer, size_t buffer_size );
      void set_istream( std::istream * is );

      //DEPRECATED
      std::string get_message_header();

      HttpResponse & operator<<( const std::string & str );

      friend std::ostream& operator<<(std::ostream& out, const http::HttpResponse & response) {
          out << response.protocol << "/" << response.http_version_major << "." << response.http_version_minor << " " << parse_status( response.status ) << "\n";
          out << "ResponseLines:\n";
          for( auto response_line : response.headers ) {
              out << "\t" << response_line.first << ": " << response_line.second << "\n";
          }
          return out;
      }

  private:
      std::map<std::string, std::string> headers;
      size_t size = 0;
      std::stringstream body_stream;
      int seconds = 0;
      time_t last_modified;
      mime::MIME_TYPE type = mime::MIME_TYPE::TEXT; //TODO octed bla bla
      static std::string to_string(http_status status);
      std::istream * body_istream = nullptr;
  };

  class HttpClient {
    public:
        HttpClient(const std::string & ip, const int & port, const std::string & uri);
        ~HttpClient();

        void connect();

    private:
  //      asio::io_service & io_service_;
  //      tcp::socket socket_;
  };

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
            /**
             * @brief Abstract HttpServlet for implementing your own Servlets.
             * @param path The Request Path for this servlet
             */
            HttpServlet(std::string path);
            /**
             * @brief Desctructor for the HttpServlet.
             */
            ~HttpServlet();
            /**
             * Callback function for the GET method.
             * @param request The HTTP request path,
             * @param response The HTTP Response object,
             */
            bool match(const std::string & request_path);
            /**
             * @brief Match the path with parameters in the url
             * @param request_path The Http request path
             * @param arg1 Mandatory first parameter
             * @return
             */
            template <class FIRST> bool match( const std::string & request_path, FIRST * arg1 ) {
                return re->FullMatch(request_path.c_str(), arg1);
            }
            /**
             * @brief Match the path with parameters in the url
             * @param request_path The Http request path
             * @param arg1 Mandatory first parameter
             * @param arg2 Optional second parameter
             * @return
             */
            template <class FIRST, class SECOND> bool match( const std::string & request_path, FIRST * arg1, SECOND * arg2 ) {
                return re->FullMatch(request_path.c_str(), arg1, arg2);
            }
            /**
             * @brief Match the path with parameters in the url
             * @param request_path The Http request path
             * @param arg1 Mandatory first parameter
             * @param arg2 Optional second parameter
             * @param arg3 Optional third parameter
             * @return
             */
            template <class FIRST, class SECOND, class THIRD> bool match( const std::string & request_path, FIRST * arg1, SECOND * arg2, THIRD * arg3 ) {
                return re->FullMatch(request_path.c_str(), arg1, arg2, arg3 );
            }
            /**
			 * Callback function for the GET method.
             * @param request The HTTP Request object.
             * @param response The HTTP Response object.
			 */
            virtual void do_get(HttpRequest & request, HttpResponse & response);
			/**
			 * Callback function for the POST method.
             * @param request The HTTP Request object.
             * @param response The HTTP Response object.
			 */
            virtual void do_post(HttpRequest & request, HttpResponse & response);
            /**
			 * Callback function for the DELETE method.
             * @param request The HTTP Request object.
             * @param response The HTTP Response object.
			 */
            virtual void do_delete(HttpRequest & request, HttpResponse & response);
			/**
			 * Callback function for the PUT method.
             * @param request The HTTP Request object.
             * @param response The HTTP Response object.
			 */
            virtual void do_put(HttpRequest & request, HttpResponse & response);
            /**
             * Callback function for the HEAD method.
             * @param request The HTTP Request object.
             * @param response The HTTP Response object.
             */
            virtual void do_head(HttpRequest & request, HttpResponse & response);

            /**
             * Callback function for the SUBSCIRBE method.
             * @param request The HTTP Request object.
             * @param response The HTTP Response object.
             */
            virtual void do_subscribe(HttpRequest & request, HttpResponse & response);

            /**
             * Create a stock reply with the given status code.
             * @brief create_stock_reply
             * @param status
             * @param response
             */
            void create_stock_reply(http_status status, HttpResponse & response);

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
            void create_response(HttpResponse & response, ::http::http_status http_status, std::istream * ss, int content_length, mime::MIME_TYPE mime_type );
    };

    enum class PARSE_STATE { TRUE, FALSE, CONTINUE };

    enum class parser_type { METHOD, REQUEST_URI, REQUEST_PROTOCOL, REQUEST_VERSION_MAJOR,
                       REQUEST_VERSION_MINOR, REQUEST_KEY, REQUEST_VALUE, REQUEST_BODY,
                       RESPONSE_STATUS, RESPONSE_STATUS_TEXT };

    class HttpParser {
    public:
        /**
         * @brief Parser for the HTTP requests.
         */
        HttpParser();
        /**
         * @brief Static function to parse a HTTP request.
         */
        static PARSE_STATE parse_http_request( http::HttpRequest & request, const std::array<char, 8192> input, size_t size );
        static parser_type next(parser_type type);
        static parser_type nextResponseType(parser_type type);
        /**
         * @brief Static function to parse a HTTP response.
         */
        static PARSE_STATE parse_http_response( http::HttpResponse & response, const std::array<char, 8192> input, size_t size );
    };


   /* typedef void (*response_handler)(); */

    /**
     * @brief The HttpRequestHandler class
     */
    class HttpRequestHandler {
        public:
            /**
             * @brief Register the servlets.
             * @param servlet A servlet instance.
             */
            virtual void register_servlet(HttpServlet * servlet) = 0;
            /**
             * @brief Callback handler for requests.
             * @param request The HttpRequest object.
             * @param response The HttpResponse object.
             */
            virtual void handle_request(HttpRequest & request, HttpResponse & response, std::function<void()> fptr ) = 0;
    };

    /**
     * @brief The main Web Server Class.
     */
    class WebServer : public HttpRequestHandler {
    public:
        WebServer(std::string local_ip, int port /*, int threads */)
            : local_ip(local_ip), port(port) /*, threads(threads) */ {}
        ~WebServer() {}

        /**
         * @brief Start the server.
         */
        void start();
        /**
         * @brief Stop the server.
         */
        void stop();

        /**
         * @brief Register servlets.
         * @param servlet The Servlet class.
         */
        virtual void register_servlet(HttpServlet * servlet);
        /**
         * @brief The handle request callback method.
         * @brief handle_request
         * @param request
         * @param response
         */
        virtual void handle_request(HttpRequest & request, HttpResponse & response, std::function<void()> fptr );
    private:
        std::vector<HttpServlet *> servlets;
        std::string local_ip;
        int port; //, threads;
    };
}
namespace http {
namespace utils {
/**
 * @brief Normalize the http parameter key to a camel case style.
 * @param key the key.
 * @return the camel case key.
 */
inline std::string normalize_key(std::string key) {
  std::locale loc;
  std::stringstream ss_buffer;
  char lastChar;
  for (size_t i=0; i<key.length(); i++) {
    if( i == 0 ) {
   ss_buffer << std::toupper(key[i],loc);
    } else if( lastChar == '-' ) {
   ss_buffer << std::toupper(key[i],loc);
    } else {
   ss_buffer << std::tolower(key[i],loc);
    }
    lastChar = key[i];
  }
  return ss_buffer.str();
};
/**
 * @brief Get parameters from request line and write them to the request.
 * @param parameters the request line string.
 * @param request the request where to store the parameters.
 */
inline void get_parameters(std::string parameters, HttpRequest & request) {
  enum parse_mode { KEY, VALUE } mode = KEY;
  std::stringstream ss_buffer_key;
  std::stringstream ss_buffer_value;
  for (size_t i=0; i<parameters.length(); i++) {
    if( parameters[i] == '=' ) {
  mode = VALUE;
    } else if( parameters[i] == '&' ) {
  std::cout << ss_buffer_key.str() << " = " <<  ss_buffer_value.str() << std::endl;
  request.parameters[ss_buffer_key.str()] = ss_buffer_value.str();
  ss_buffer_key.str(std::string()); ss_buffer_key.clear();
  ss_buffer_value.str(std::string()); ss_buffer_value.clear();
  mode = KEY;
    } else if( mode == KEY ) {
  ss_buffer_key << parameters[i];
    } else {
  ss_buffer_value << parameters[i];
    }
  }
  if( mode == VALUE ) {
    request.parameters[ss_buffer_key.str()] = ss_buffer_value.str();
  }
};
}}
#endif // HTTP_H
