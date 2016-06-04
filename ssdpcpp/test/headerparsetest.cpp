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

#include <array>
#include <string>

#include "ssdp.h"
#include "../src/ssdpserverimpl.h"

#include <gtest/gtest.h>

namespace ssdp {
TEST( HeaderParseTest, Response ) {
    SSDPServerImpl server("127.0.0.1", "255.255.255.0", 1900, std::map< std::string, std::string >() );

//    ResponseLines:
//            Cache-Control: max-age=1800
//            Content-Length: 0
//            Date: Sun Jul  5 10:34:23 2015
//            Ext:
//            Location: http://192.168.0.13:8080/rootDesc.xml
//            Server: Linux/#44~14.04.1-Ubuntu SMP Fri Mar 13 10:33:29 UTC 2015 DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0
//            St: d7eede24-9688-4f20-9d59-ab93d074027f
//            Usn: uuid:d7eede24-9688-4f20-9d59-ab93d074027f::d7eede24-9688-4f20-9d59-ab93d074027f

    http::HttpResponse response;
    response.parameter( "Cache-Control", "max-age=1800");
    response.parameter( "Content-Length", "0");
    response.parameter( "Date", "Sun Jul  5 10:34:23 2015");
    response.parameter( "Ext", "");
    response.parameter( "Location", "http://192.168.0.13:8080/rootDesc.xml");
    response.parameter( "Server", "Linux/#44~14.04.1-Ubuntu SMP Fri Mar 13 10:33:29 UTC 2015 DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0");
    response.parameter( "St", "d7eede24-9688-4f20-9d59-ab93d074027f");
    response.parameter( "Usn", "uuid:d7eede24-9688-4f20-9d59-ab93d074027f::d7eede24-9688-4f20-9d59-ab93d074027f");
    SsdpEvent event = server.parseResponse( response );

    EXPECT_EQ( "http://192.168.0.13:8080/rootDesc.xml", event.location() );
    EXPECT_EQ( 1800, event.cacheControl() );
    EXPECT_EQ( "d7eede24-9688-4f20-9d59-ab93d074027f", event.nt() );
    EXPECT_EQ( "http://192.168.0.13:8080/rootDesc.xml", event.location() );
    EXPECT_EQ( "Linux/#44~14.04.1-Ubuntu SMP Fri Mar 13 10:33:29 UTC 2015 DLNADOC/1.50 UPnP/1.0 SSDP/1.0.0", event.server() );
    EXPECT_EQ( "uuid:d7eede24-9688-4f20-9d59-ab93d074027f::d7eede24-9688-4f20-9d59-ab93d074027f", event.usn() );
}
}//namespace ssdp
