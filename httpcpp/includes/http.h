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
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <time.h>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>

/**
 * \brief the libhttp package.
 */
 
namespace http {
static const bool DEBUG = true;
static const size_t BUFFER_SIZE = 8192;
} //http

namespace http {
namespace utils {
/**
 * URL decode the in string to out string.
 * @brief url_decode
 * @param in the url encoded string.
 * @param out the output string.
 * @return false when the string can not be deocoded.
 */
inline bool url_decode(const std::string & in, std::string & out) {
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%') {
            if (i + 3 <= in.size()) {
                int value;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    out += static_cast<char>(value);
                    i += 2;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (in[i] == '+') {
            out += ' ';
        } else {
            out += in[i];
        }
    }
    return true;
}
class UrlEscape {
private:
    static std::string charToHex(unsigned char c) {
        short i = c;

        std::stringstream s;

        s << "%" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << i;

        return s.str();
    }
public:
    static std::string urlEncode( const std::string &toEncode ) {
        std::ostringstream out;

        for(std::string::size_type i=0; i < toEncode.length(); ++i) {
            short t = toEncode.at(i);

            if( t == 45 ||          // hyphen
                (t >= 48 && t <= 57) ||       // 0-9
                (t >= 65 && t <= 90) ||       // A-Z
                t == 95 ||          // underscore
                (t >= 97 && t <= 122) ||  // a-z
                t == 126            // tilde
                    ) {
                out << toEncode.at(i);
            } else {
                 out << charToHex( t );
            }
        }

        return out.str();
    }
};
inline std::string escape_xml(std::string str) {
    std::ostringstream ss;
    short utf_chars = 0, utf_count = 0;
    char utf_buffer[4];
    for (std::string::const_iterator iter = str.begin(); iter != str.end(); iter++) {
        switch (*iter) {
        case '&': ss << "&amp;"; break;
        case '"': ss << "&quot;"; break;
        case '\'': ss << "&apos;"; break;
        case '<': ss << "&lt;"; break;
        case '>': ss << "&gt;"; break;
        default:

            if( ( *iter & 0b11110000 ) == 0b11110000 ) {
                utf_buffer[ utf_count++ ] = *iter;
                utf_chars = 4;

            } else if( ( *iter & 0b11100000 ) == 0b11100000 ) {
                utf_buffer[ utf_count++ ] = *iter;
                utf_chars = 3;

            } else if( ( *iter & 0b11000000 ) == 0b11000000 ) {
                utf_buffer[ utf_count++ ] = *iter;
                utf_chars = 2;

            } else if( ( *iter & 0b10000000 ) == 0b10000000 && utf_chars != 0 ) {
                utf_buffer[ utf_count++ ] = *iter;
                if( utf_count == utf_chars ) {
                    unsigned long result = 0;
                    if( utf_chars == 2 ) {
                        result = (( utf_buffer[0] & 0b00111111 ) << 6 ) | ( utf_buffer[1] & 0b00111111 );

                    } else if( utf_chars == 3 ) {
                        result = (( utf_buffer[0] & 0b00011111 ) << 12 ) | (( utf_buffer[1] & 0b00111111 ) << 6 ) | ( utf_buffer[2] & 0b00111111 );

                    } else if( utf_chars == 4 ) {
                        result = (( utf_buffer[0] & 0b00001111 ) << 18 ) | (( utf_buffer[1] & 0b00111111 ) << 12 ) | (( utf_buffer[2] & 0b00111111 ) << 6 )| ( utf_buffer[3] & 0b00111111 );
                    }
                    utf_chars = 0; utf_count = 0;
                    if( result <= 9900 ) { // TODO why this is neccessary?
                        ss << "&#" << result << ";";
                    } else {
                        std::cout << "skip value: " << result << std::endl;
                    }
                }
            } else {
                ss << *iter;
            }
            break;
        }
    }
    return ss.str();
}
} // utils
} // http

#include "httpcpp/mimetypes.h"
#include "httpcpp/httpstatus.h"
#include "httpcpp/httpresponse.h"
#include "httpcpp/httprequest.h"
#include "httpcpp/httputils.h"
#include "httpcpp/httpresponseparser.h"
#include "httpcpp/httprequestparser.h"
#include "httpcpp/ihttpclientconnection.h"
#include "httpcpp/httpclient.h"
#include "httpcpp/httpservletcontext.h"
#include "httpcpp/httpservlet.h"
#include "httpcpp/httprequesthandler.h"
#include "httpcpp/ihttpserver.h"
#include "httpcpp/webserver.h"

#endif // HTTP_H
