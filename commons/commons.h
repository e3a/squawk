/*
    commons utils.
    Copyright (C) 2013  <e.knecht@netwings.ch>

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

#ifndef COMMONS_UTILS_H
#define COMMONS_UTILS_H

#include <algorithm>
#include <ctime>
#include <list>
#include <string>
#include <sstream>
#include <sys/utsname.h>
#include <sys/stat.h>

#include <iostream> //TODO remove
#include <bitset> //TODO remove
/* #include "http.h" */

namespace commons {
/**
 * \brief namespace for the string utilities.
 */
namespace string {
  /**
   * @brief Left trim the string
   * @param s in string
   * @param t characters to trim
   * @return the trimmed string
   */
  inline std::string ltrim(std::string s, const char* t = " \t\n\r\f\v" ) {
      s.erase(0, s.find_first_not_of( t ));
      return s;
  }
  /**
   * @brief Right trim the string
   * @param s in string
   * @param t characters to trim
   * @return the trimmed string
   */
  inline std::string rtrim(std::string s, const char* t = " \t\n\r\f\v" ) {
      s.erase(s.find_last_not_of( t ) + 1);
      return s;
  }
  /**
   * @brief Trim the string
   * @param s in string
   * @param t characters to trim
   * @return the trimmed string
   */
  inline std::string trim(std::string s, const char* t = " \t\n\r\f\v" ) {
        return ltrim(rtrim(s, t), t);
  }
  /**
   * @brief Convert the string to lowercase
   * @param s in string
   * @return
   */
  inline std::string to_lower(std::string str) {
    std::ostringstream ss;
    std::locale loc;
    for(std::string::size_type i=0; i<str.length(); ++i) {
      ss << std::tolower(str[i],loc);
    }
    return ss.str();
  };
  /**
   * @brief Convert the string to uppercase
   * @param s in string
   * @return
   */
  inline std::string to_upper(std::string str) {
    std::ostringstream ss;
    std::locale loc;
    for(std::string::size_type i=0; i<str.length(); ++i) {
      ss << std::toupper(str[i],loc);
    }
    return ss.str();
  };
  inline bool ends_with(const std::string & str, const std::string & suffix, bool icase = false) {
      if( suffix.size() > str.size() ) return false;
      if ( str.compare(str.size()-suffix.size(), str.size(), suffix) == 0 ) {
        return true;
      } else if( icase && to_upper( str ).compare(str.size()-suffix.size(), str.size(), to_upper( suffix )) == 0 ) {
        return true;
      } else return false;
  };
  /**
   * @brief Test if the string starts with
   * @param string
   * @param prefix
   * @return
   */
  inline bool starts_with(std::string str, std::string prefix) {
    return( str.compare(0, prefix.size(), prefix) == 0 );
  };
  /**
   * @brief Check if string is number
   * @param string
   * @return
   */
  inline bool is_number(const std::string &s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
  }
  template<typename T>
  /**
   * returns a string for the input value.
   *
   * <code>
        string str = to_string< int > (4);
   * </code>
   */
  std::string to_string(T t) {
    std::stringstream s;
    s << t;
    return s.str();
  };
  template<typename R>
  R parse_string(std::string str) {
    std::stringstream buffer(str);
    R var;
    buffer >> var;
    return var;
  };
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
  };
  inline std::string escape_json(std::string str) {
    std::ostringstream ss;
    for (std::string::const_iterator iter = str.begin(); iter != str.end(); iter++) {
        switch (*iter) {
            case '\\': ss << "\\\\"; break;
            case '"': ss << "\\\""; break;
            case '/': ss << "\\/"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << *iter; break;
        }
    }
    return ss.str();
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
                        result = (( utf_buffer[0] & 0b00011111 ) << 12 ) | (( utf_buffer[1] & 0b00111111 ) << 6 ) | utf_buffer[2] & 0b00111111;

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
  };
  inline std::string time_to_string(int playtime) {
    int _seconds = playtime;
    std::stringstream ss;
    if(_seconds > 3600) {
      int hours = _seconds / 3600;
      if(hours < 10) {
	ss << "0";
      }
      ss << hours << ":";
      _seconds = _seconds - (hours * 3600);
    }
    if(_seconds > 60) {
      int minutes = _seconds / 60;
      if(minutes < 10) {
	ss << "0";
      }
      ss << minutes << ":";
      _seconds = _seconds - (minutes * 60);    
    } else ss << "00:";
    if(_seconds < 10) {
      ss << "0";
    }
    ss << _seconds;
    return ss.str();
  }
}}
namespace commons {
/**
 * \brief namespace for the system utilities.
 */
namespace system {
inline std::string uname() {
  struct utsname uts;
  uname(&uts);
  std::ostringstream system;
  system << uts.sysname << "/" << uts.version;
  return system.str();
};
inline std::string time_string() {
  time_t rawtime;
  time (&rawtime);
  std::string str_time = std::string( std::ctime ( &rawtime ) );
  return commons::string::trim( str_time );
};
}}
namespace commons {
/**
 * \brief namespace for the file system utilities.
 */
namespace filesystem {
inline std::list<std::string> getPathTokens(const std::string & path) {
    std::list<std::string> result_list;
    if(path.empty() || path.find('/') == std::string::npos)
        result_list.push_back( path );

    int end_pos = 0;
    int first_pos = 0;
    do {
        first_pos = path.find( '/', end_pos );
        if(first_pos != std::string::npos ) {
            end_pos = path.find( '/', first_pos + 1 );
            if(end_pos != std::string::npos ) {
                if( end_pos - first_pos > 1 ) {
                    result_list.push_back( path.substr(first_pos + 1, ( end_pos - first_pos - 1 ) ) );
                }
            } else if( path.size() - first_pos > 1 ) {
                result_list.push_back( path.substr( first_pos + 1 ) );
            }
        }
    } while( first_pos != std::string::npos );
    return result_list;
}
inline bool is_directory(std::string & filename) {
  struct stat sb;
  return (stat(filename.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
};
}}
#endif // COMMONS_UTILS_H
