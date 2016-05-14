/*
    <one line to give the library's name and an idea of what it does.>
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

#ifndef SQuAWK_H
#define SQuAWK_H

#include <algorithm>
#include <array>
#include <fstream>
#include <list>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include <uuid/uuid.h>

#include "pcrecpp.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <boost/mpl/string.hpp>

#include "http.h"
#include "ssdp.h"
#include "xml.h"

#include "log4cxx/logger.h"

#include "gtest/gtest_prod.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "db/sqlite3database.h"
#include "db/sqlite3connection.h"
#include "db/sqlite3statement.h"

#include "upnp2.h"
#include "didl.h"
#include "didljsonwriter.h"
#include "didlxmlwriter.h"

namespace squawk {
static const bool DEBUG = true;
}

//namespace squawk {
//namespace media {

///**
// * @brief get the clean name
// * @param name
// * @return
// */
//inline std::string clean_name( const std::string & name ) {

//    std::string s = name;

//    std::string allowed = "+-()";
//    s.erase(remove_if(s.begin(), s.end(), [&allowed](const char& c) {
//            return allowed.find(c) != string::npos;
//    }), s.end());

//    boost::algorithm::trim( s );
//    boost::algorithm::to_lower( s );

//    if( s.rfind("the ", 0) == 0)
//        s.erase(0, 4);
//    if( s.rfind("die ", 0) == 0)
//        s.erase(0, 4);
//    if( s.rfind("das ", 0) == 0)
//        s.erase(0, 4);
//    if( s.rfind("der ", 0) == 0)
//        s.erase(0, 4);
//    return s;
//}
//inline std::string get_letter( const std::string & clean_name ) {
//    if(clean_name.length()>0) {
//        return boost::to_upper_copy( clean_name.substr(0, 1) );
//    } else {
//        return std::string("");
//    }
//}
//static pcrecpp::RE re_clean_path("(.*)/CD[\\d+]");
//inline std::string get_album_clean_path(const std::string & path) {
//    string clean_path;
//    if(re_clean_path.PartialMatch(path, &clean_path)) {
//        return boost::algorithm::trim_copy(clean_path);
//    } else {
//        return boost::algorithm::trim_copy(path);
//    }
//}
//}
//}

#endif // SQUAWK_H
