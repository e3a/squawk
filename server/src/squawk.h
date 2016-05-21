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
#include "utils/xml.h"

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
static const bool SUAWK_SERVER_DEBUG = true;

//#ifdef SQUAWK_RELEASE
//static const char* C_SQUAWK_RELEASE = SQUAWK_RELEASE;
//#else
//static const char* C_SQUAWK_RELEASE = "0.0.0";
//#endif

}//namespace squawk
#endif // SQUAWK_H
