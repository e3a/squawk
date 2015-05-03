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

#include "mimetypes.h"
#include "commons.h"
#include "gtest/gtest_prod.h"
#include "pcrecpp.h"

namespace http {
static const bool DEBUG = true;
static const size_t BUFFER_SIZE = 8192;
} //http

#include "httpstatus.h"
#include "httpresponse.h"
#include "httprequest.h"
#include "httputils.h"
#include "httpresponseparser.h"
#include "httprequestparser.h"
#include "ihttpclientconnection.h"
#include "httpclient.h"
#include "httpservlet.h"
#include "httprequesthandler.h"
#include "webserver.h"

#endif // HTTP_H
