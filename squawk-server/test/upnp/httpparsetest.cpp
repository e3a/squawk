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


#include <string>
#include <vector>
#include "../src/ssdp/ssdpserverimpl.h"
#include "../src/http/upnp/contentdirectory.h"
#include "../src/http/request_parser.h"
#include <gtest/gtest.h>

// using ::testing::Return;
/* TEST(HttpParser, ParseNotify) {
  
  char *http = "NOTIFY * HTTP/1.1\r\n"
      "Host: 239.255.255.250:1900\r\n"
      "Cache-Control: max-age=60\r\n"
      "Location: http://192.168.0.2:1780/InternetGatewayDevice.xml\r\n"
      "NTS: ssdp:alive\r\n"
      "Server: POSIX, UPnP/1.0 linux/5.20.61.0\r\n"
      "NT: uuid:55DD7905-8CE9-B58F-42A9-EC5064D66B19\r\n"
      "USN: uuid:55DD7905-8CE9-B58F-42A9-EC5064D66B19\r\n";
      "\r\n";
    
      std::map<std::string, std::string> map = squawk::upnp::SSDPServer::parseHeaders(http);
      EXPECT_EQ(10, map.size());
      EXPECT_EQ(map["method"], "NOTIFY");
      EXPECT_EQ(map["uri"], "*");
      EXPECT_EQ(map["version"], "HTTP/1.1");

/*    HttpFetchMock mock;
    HtmlParser parser(mock);
    EXPECT_CALL(mock, GetUriAsString("http://example.net"))
        .WillOnce(Return(std::string(html)));
    std::vector<std::string> links = parser.GetAllLinks("http://example.net");
    EXPECT_EQ(1, links.size()); */
/*  } */

TEST(HttpParser, ParseBody) {

  char * request = "POST /ctl/ContentDir HTTP/1.0\r\n"
    "Host: 192.168.0.13\r\n"
    "User-Agent: Twisted PageGetter\r\n"
    "Content-Length: 438\r\n"
    "SOAPACTION: \"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"\r\n"
    "content-type: text/xml ;charset=\"utf-8\"\r\n"
    "connection: close\r\n"
    "\r\n"
    "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:ns0=\"urn:schemas-upnp-org:service:ContentDirectory:1\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><ns0:Browse><ObjectID>0</ObjectID><BrowseFlag>BrowseMetadata</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>0</RequestedCount><SortCriteria /></ns0:Browse></s:Body></s:Envelope>";

    char * response = "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:ns0=\"urn:schemas-upnp-org:service:ContentDirectory:1\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><ns0:Browse><ObjectID>0</ObjectID><BrowseFlag>BrowseMetadata</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>0</RequestedCount><SortCriteria /></ns0:Browse></s:Body></s:Envelope>";

    squawk::http::request_parser parser;
    char body[strlen(response)];
    parser.parse(request, body, strlen(request));
    std::cout << std::endl << "=================" << std::endl << body << std::endl;
    EXPECT_EQ(std::string(response), std::string(body));
}

TEST(HttpParser, ParseCdsBrowse) {

  char * request = "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:ns0=\"urn:schemas-upnp-org:service:ContentDirectory:1\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><ns0:Browse><ObjectID>0</ObjectID><BrowseFlag>BrowseMetadata</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>0</RequestedCount><SortCriteria /></ns0:Browse></s:Body></s:Envelope>";
  /*
  ContentDirectory * cds = new ContentDirectory();
  cds->parseRequest(request);
  delete cds;
  */
  
}