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
#include "http.h"
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

TEST(HttpRequestParser, ParseWithBody) {

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

    char body[strlen(response)];
    http::HttpRequest http_request = http::HttpParser::parse_http_request(request, strlen(request));
    EXPECT_EQ(std::string("POST"), http_request.request_method);
    EXPECT_EQ(std::string("/ctl/ContentDir"), http_request.request_uri);
    EXPECT_EQ(std::string("HTTP/1.0"), http_request.request_version);

    EXPECT_EQ(http_request.request_lines.size(), 6);
    EXPECT_EQ(std::string("192.168.0.13"), http_request.request_lines["Host"]);
    EXPECT_EQ(std::string("close"), http_request.request_lines["Connection"]);
    
    EXPECT_EQ(std::string(response), http_request.body);
}
TEST(HttpRequestParser, ParseSSDPNotify) {

  char * request = "NOTIFY * HTTP/1.1\r\n"
    "Cache-Control: max-age=1800\r\n"
    "Content-Length: 0\r\n"
    "DATE: Sat Dec 28 09:59:08 2013\r\n"
    "Host: 239.255.255.250:1900\r\n"
    "Location: http://192.168.0.13:8080/rootDesc.xml\r\n"
    "Nt: upnp:rootdevice\r\n"
    "Nts: ssdp:alive\r\n"
    "Server: Linux/#1 SMP Debian 3.2.51-1 DLNADOC/1.50 UPnP/1.0 Squawk/1.0.0\r\n"
    "Usn: uuid:a11fd44b-7377-578c-893d-8b02b3454397\r\n"
    "\r\n";

    http::HttpRequest http_request = http::parse_http_request(request, strlen(request));
    EXPECT_EQ(std::string("NOTIFY"), http_request.request_method);
    EXPECT_EQ(std::string("*"), http_request.request_uri);
    EXPECT_EQ(std::string("HTTP/1.1"), http_request.request_version);

    EXPECT_EQ(http_request.request_lines.size(), 9);
    EXPECT_EQ(std::string("239.255.255.250:1900"), http_request.request_lines["Host"]);
    EXPECT_EQ(std::string("Sat Dec 28 09:59:08 2013"), http_request.request_lines["Date"]);
    EXPECT_EQ(std::string("ssdp:alive"), http_request.request_lines["Nts"]);
    
    EXPECT_EQ(0, http_request.body.size());
}

TEST(HttpRequestParser, ParseGetParameter) {

    char request[] = {
    0x47, 0x45, 0x54, 0x20, 0x2f, 0x73, 0x75, 0x63, 
    0x68, 0x65, 0x2f, 0x3f, 0x71, 0x3d, 0x70, 0x6f, 
    0x73, 0x74, 0x26, 0x73, 0x65, 0x61, 0x72, 0x63, 
    0x68, 0x5f, 0x73, 0x75, 0x62, 0x6d, 0x69, 0x74, 
    0x2e, 0x78, 0x3d, 0x2d, 0x39, 0x33, 0x32, 0x26, 
    0x73, 0x65, 0x61, 0x72, 0x63, 0x68, 0x5f, 0x73, 
    0x75, 0x62, 0x6d, 0x69, 0x74, 0x2e, 0x79, 0x3d, 
    0x2d, 0x31, 0x32, 0x26, 0x72, 0x6d, 0x3d, 0x73, 
    0x65, 0x61, 0x72, 0x63, 0x68, 0x20, 0x48, 0x54, 
    0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, 0x0d, 0x0a, 
    0x48, 0x6f, 0x73, 0x74, 0x3a, 0x20, 0x77, 0x77, 
    0x77, 0x2e, 0x68, 0x65, 0x69, 0x73, 0x65, 0x2e, 
    0x64, 0x65, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 0x6e, 
    0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 0x20, 
    0x6b, 0x65, 0x65, 0x70, 0x2d, 0x61, 0x6c, 0x69, 
    0x76, 0x65, 0x0d, 0x0a, 0x41, 0x63, 0x63, 0x65, 
    0x70, 0x74, 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 
    0x2f, 0x68, 0x74, 0x6d, 0x6c, 0x2c, 0x61, 0x70, 
    0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 
    0x6e, 0x2f, 0x78, 0x68, 0x74, 0x6d, 0x6c, 0x2b, 
    0x78, 0x6d, 0x6c, 0x2c, 0x61, 0x70, 0x70, 0x6c, 
    0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2f, 
    0x78, 0x6d, 0x6c, 0x3b, 0x71, 0x3d, 0x30, 0x2e, 
    0x39, 0x2c, 0x69, 0x6d, 0x61, 0x67, 0x65, 0x2f, 
    0x77, 0x65, 0x62, 0x70, 0x2c, 0x2a, 0x2f, 0x2a, 
    0x3b, 0x71, 0x3d, 0x30, 0x2e, 0x38, 0x0d, 0x0a, 
    0x55, 0x73, 0x65, 0x72, 0x2d, 0x41, 0x67, 0x65, 
    0x6e, 0x74, 0x3a, 0x20, 0x4d, 0x6f, 0x7a, 0x69, 
    0x6c, 0x6c, 0x61, 0x2f, 0x35, 0x2e, 0x30, 0x20, 
    0x28, 0x58, 0x31, 0x31, 0x3b, 0x20, 0x4c, 0x69, 
    0x6e, 0x75, 0x78, 0x20, 0x69, 0x36, 0x38, 0x36, 
    0x29, 0x20, 0x41, 0x70, 0x70, 0x6c, 0x65, 0x57, 
    0x65, 0x62, 0x4b, 0x69, 0x74, 0x2f, 0x35, 0x33, 
    0x37, 0x2e, 0x33, 0x36, 0x20, 0x28, 0x4b, 0x48, 
    0x54, 0x4d, 0x4c, 0x2c, 0x20, 0x6c, 0x69, 0x6b, 
    0x65, 0x20, 0x47, 0x65, 0x63, 0x6b, 0x6f, 0x29, 
    0x20, 0x43, 0x68, 0x72, 0x6f, 0x6d, 0x65, 0x2f, 
    0x33, 0x31, 0x2e, 0x30, 0x2e, 0x31, 0x36, 0x35, 
    0x30, 0x2e, 0x36, 0x33, 0x20, 0x53, 0x61, 0x66, 
    0x61, 0x72, 0x69, 0x2f, 0x35, 0x33, 0x37, 0x2e, 
    0x33, 0x36, 0x0d, 0x0a, 0x52, 0x65, 0x66, 0x65, 
    0x72, 0x65, 0x72, 0x3a, 0x20, 0x68, 0x74, 0x74, 
    0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 
    0x68, 0x65, 0x69, 0x73, 0x65, 0x2e, 0x64, 0x65, 
    0x2f, 0x0d, 0x0a, 0x41, 0x63, 0x63, 0x65, 0x70, 
    0x74, 0x2d, 0x45, 0x6e, 0x63, 0x6f, 0x64, 0x69, 
    0x6e, 0x67, 0x3a, 0x20, 0x67, 0x7a, 0x69, 0x70, 
    0x2c, 0x64, 0x65, 0x66, 0x6c, 0x61, 0x74, 0x65, 
    0x2c, 0x73, 0x64, 0x63, 0x68, 0x0d, 0x0a, 0x41, 
    0x63, 0x63, 0x65, 0x70, 0x74, 0x2d, 0x4c, 0x61, 
    0x6e, 0x67, 0x75, 0x61, 0x67, 0x65, 0x3a, 0x20, 
    0x65, 0x6e, 0x2d, 0x55, 0x53, 0x2c, 0x65, 0x6e, 
    0x3b, 0x71, 0x3d, 0x30, 0x2e, 0x38, 0x0d, 0x0a, 
    0x43, 0x6f, 0x6f, 0x6b, 0x69, 0x65, 0x3a, 0x20, 
    0x77, 0x74, 0x33, 0x5f, 0x65, 0x69, 0x64, 0x3d, 
    0x25, 0x33, 0x42, 0x32, 0x38, 0x38, 0x36, 0x38, 
    0x39, 0x36, 0x33, 0x36, 0x39, 0x32, 0x30, 0x31, 
    0x37, 0x34, 0x25, 0x37, 0x43, 0x32, 0x31, 0x33, 
    0x38, 0x38, 0x32, 0x33, 0x38, 0x30, 0x37, 0x35, 
    0x30, 0x30, 0x33, 0x39, 0x36, 0x32, 0x35, 0x37, 
    0x3b, 0x20, 0x77, 0x74, 0x33, 0x5f, 0x73, 0x69, 
    0x64, 0x3d, 0x25, 0x33, 0x42, 0x32, 0x38, 0x38, 
    0x36, 0x38, 0x39, 0x36, 0x33, 0x36, 0x39, 0x32, 
    0x30, 0x31, 0x37, 0x34, 0x0d, 0x0a, 0x0d, 0x0a };      

    http::HttpRequest http_request = http::parse_http_request(request, sizeof(request));
    EXPECT_EQ(std::string("GET"), http_request.request_method);
    EXPECT_EQ(std::string("/suche/"), http_request.request_uri);
    EXPECT_EQ(std::string("HTTP/1.1"), http_request.request_version);

    EXPECT_EQ(http_request.request_lines.size(), 8);
    EXPECT_EQ(std::string("www.heise.de"), http_request.request_lines["Host"]);
    EXPECT_EQ(std::string("keep-alive"), http_request.request_lines["Connection"]);
    EXPECT_EQ(std::string("text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"), http_request.request_lines["Accept"]);
    EXPECT_EQ(std::string("Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36"), http_request.request_lines["User-Agent"]);
    EXPECT_EQ(std::string("http://www.heise.de/"), http_request.request_lines["Referer"]);
    EXPECT_EQ(std::string("gzip,deflate,sdch"), http_request.request_lines["Accept-Encoding"]);
    EXPECT_EQ(std::string("en-US,en;q=0.8"), http_request.request_lines["Accept-Language"]);
    EXPECT_EQ(std::string("wt3_eid=%3B288689636920174%7C2138823807500396257; wt3_sid=%3B288689636920174"), http_request.request_lines["Cookie"]);

    // ?q=post&search_submit.x=-932&search_submit.y=-12&rm=search
    
    EXPECT_EQ(4, http_request.parameters.size());
    EXPECT_EQ(std::string("post"), http_request.parameters["q"]);
    EXPECT_EQ(std::string("-932"), http_request.parameters["search_submit.x"]);
    EXPECT_EQ(std::string("-12"), http_request.parameters["search_submit.y"]);
    EXPECT_EQ(std::string("search"), http_request.parameters["rm"]);

    EXPECT_EQ(0, http_request.body.size());
}

TEST(HttpRequestParser, ParseSSDPSearch) {

    char request[] = {
    0x4d, 0x2d, 0x53, 0x45, 0x41, 0x52, 0x43, 0x48, 
    0x20, 0x2a, 0x20, 0x48, 0x54, 0x54, 0x50, 0x2f, 
    0x31, 0x2e, 0x31, 0x0d, 0x0a, 0x48, 0x4f, 0x53, 
    0x54, 0x3a, 0x20, 0x32, 0x33, 0x39, 0x2e, 0x32, 
    0x35, 0x35, 0x2e, 0x32, 0x35, 0x35, 0x2e, 0x32, 
    0x35, 0x30, 0x3a, 0x31, 0x39, 0x30, 0x30, 0x0d, 
    0x0a, 0x4d, 0x41, 0x4e, 0x3a, 0x20, 0x22, 0x73, 
    0x73, 0x64, 0x70, 0x3a, 0x64, 0x69, 0x73, 0x63, 
    0x6f, 0x76, 0x65, 0x72, 0x22, 0x0d, 0x0a, 0x4d, 
    0x58, 0x3a, 0x20, 0x35, 0x0d, 0x0a, 0x53, 0x54, 
    0x3a, 0x20, 0x73, 0x73, 0x64, 0x70, 0x3a, 0x61, 
    0x6c, 0x6c, 0x0d, 0x0a, 0x0d, 0x0a };
      
    http::HttpRequest http_request = http::parse_http_request(request, sizeof(request));
    EXPECT_EQ(std::string("M-SEARCH"), http_request.request_method);
    EXPECT_EQ(std::string("*"), http_request.request_uri);
    EXPECT_EQ(std::string("HTTP/1.1"), http_request.request_version);

    EXPECT_EQ(http_request.request_lines.size(), 4);
    EXPECT_EQ(std::string("239.255.255.250:1900"), http_request.request_lines["Host"]);
    EXPECT_EQ(std::string("\"ssdp:discover\""), http_request.request_lines["Man"]);
    EXPECT_EQ(std::string("5"), http_request.request_lines["Mx"]);
    EXPECT_EQ(std::string("ssdp:all"), http_request.request_lines["St"]);
    std::cout << "BODY:" << http_request.body << std::endl;
    EXPECT_EQ(0, http_request.body.size());
}

TEST(HttpRequestParser, NormalizeKey) {

  EXPECT_EQ(std::string("Content-Type"), http::normalize_key(std::string("CONTENT-TYPE")));
  EXPECT_EQ(std::string("Content-Type"), http::normalize_key(std::string("content-type")));
  EXPECT_EQ(std::string("Content-Type"), http::normalize_key(std::string("Content-Type")));
  
}
