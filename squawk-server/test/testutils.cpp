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
#include <map>
#include "squawk-utils.h"
#include <gtest/gtest.h>

TEST(SquawkUtils, TestStartsWith) {
  ASSERT_FALSE(squawk::utils::string::starts_with("abcdefghi", "def"));
  ASSERT_TRUE(squawk::utils::string::starts_with("abcdefghi", "abc"));
}
TEST(SquawkUtils, TestEndsWith) {
  ASSERT_TRUE(squawk::utils::string::ends_with("foo/bar/song.mp3", ".mp3"));
  ASSERT_TRUE(squawk::utils::string::ends_with("foo.bar.jpg", ".jpg"));
  ASSERT_FALSE(squawk::utils::string::ends_with("foo.bar.jpg", ".mp3"));
  ASSERT_FALSE(squawk::utils::string::ends_with(".jpg", "foo.bar.jpg"));
}

TEST(SquawkUtils, TestToString) {  
  EXPECT_EQ("1", squawk::utils::string::to_string<int>(1));
  EXPECT_EQ("512", squawk::utils::string::to_string<int>(512));
}

TEST(SquawkUtils, TestToLower) {
  EXPECT_EQ(std::string("aaa bbb ccc"), squawk::utils::string::to_lower(std::string("AAA bbb CCC")));
}
TEST(SquawkUtils, TestToUpper) {
  EXPECT_EQ(std::string("AAA BBB CCC"), squawk::utils::string::to_upper(std::string("AAA bbb CCC")));
}
TEST(SquawkUtils, TestTrim) {
  EXPECT_EQ(std::string("AAA BBB CCC"), squawk::utils::string::trim(std::string("  AAA BBB CCC  ")));
}

TEST(SquawkUtils, TestParseHeadersNotify) {
  char test_request[] = "NOTIFY * HTTP/1.1\r\n"
    "Cache-control: max-age=1800\r\n"
    "Host: 239.255.255.250:1900\r\n"
    "Usn: uuid:a9fe16d3-e374-42a5-ffff-ffffb1ad76d6::upnp:rootdevice\r\n"
    "Location: http://192.168.0.92:36137/dev/a9fe16d3-e374-42a5-ffff-ffffb1ad76d6/desc.xml\r\n"
    "Nt: upnp:rootdevice\r\n"
    "Nts: ssdp:byebye\r\n"
    "Server: Linux/3.1.10-g1e8b3d8 UPnP/1.0 BubbleUPnP/1.7.2.2\r\n\r\n";
  
  std::map< std::string, std::string > result = squawk::utils::http::parse_header(sizeof(test_request), test_request);
  EXPECT_EQ(10, result.size());
  EXPECT_EQ(std::string("NOTIFY"), result["method"]);
  EXPECT_EQ(std::string("*"), result["uri"]);
  EXPECT_EQ(std::string("HTTP/1.1"), result["version"]);
  EXPECT_EQ(std::string("max-age=1800"), result["CACHE-CONTROL"]);
  EXPECT_EQ(std::string("239.255.255.250:1900"), result["HOST"]);
  EXPECT_EQ(std::string("uuid:a9fe16d3-e374-42a5-ffff-ffffb1ad76d6::upnp:rootdevice"), result["USN"]);
  EXPECT_EQ(std::string("http://192.168.0.92:36137/dev/a9fe16d3-e374-42a5-ffff-ffffb1ad76d6/desc.xml"), result["LOCATION"]);
  EXPECT_EQ(std::string("upnp:rootdevice"), result["NT"]);
  EXPECT_EQ(std::string("ssdp:byebye"), result["NTS"]);
  EXPECT_EQ(std::string("Linux/3.1.10-g1e8b3d8 UPnP/1.0 BubbleUPnP/1.7.2.2"), result["SERVER"]);
}

/* TODO TEST(SquawkUtils, TestParseHeadersFourOfour) {
  char test_request[] = "HTTP/1.0 404 Not Found\r\n"
    "Cache-control: max-age=1800\r\n"
    "Host: 239.255.255.250:1900\r\n"
    "Usn: uuid:a9fe16d3-e374-42a5-ffff-ffffb1ad76d6::upnp:rootdevice\r\n"
    "Location: http://192.168.0.92:36137/dev/a9fe16d3-e374-42a5-ffff-ffffb1ad76d6/desc.xml\r\n"
    "Nt: upnp:rootdevice\r\n"
    "Nts: ssdp:byebye\r\n"
    "Server: Linux/3.1.10-g1e8b3d8 UPnP/1.0 BubbleUPnP/1.7.2.2\r\n\r\n";
  
  std::map< std::string, std::string > result = squawk::utils::http::parse_header(sizeof(test_request), test_request);
  EXPECT_EQ(10, result.size());
  EXPECT_EQ(std::string("NOTIFY"), result["method"]);
  EXPECT_EQ(std::string("*"), result["uri"]);
  EXPECT_EQ(std::string("HTTP/1.1"), result["version"]);
  EXPECT_EQ(std::string("max-age=1800"), result["Cache-control"]);
  EXPECT_EQ(std::string("239.255.255.250:1900"), result["Host"]);
  EXPECT_EQ(std::string("uuid:a9fe16d3-e374-42a5-ffff-ffffb1ad76d6::upnp:rootdevice"), result["Usn"]);
  EXPECT_EQ(std::string("http://192.168.0.92:36137/dev/a9fe16d3-e374-42a5-ffff-ffffb1ad76d6/desc.xml"), result["Location"]);
  EXPECT_EQ(std::string("upnp:rootdevice"), result["Nt"]);
  EXPECT_EQ(std::string("ssdp:byebye"), result["Nts"]);
  EXPECT_EQ(std::string("Linux/3.1.10-g1e8b3d8 UPnP/1.0 BubbleUPnP/1.7.2.2"), result["Server"]);
} */

TEST(SquawkUtils, TestParseHeadersNotifyExtraData) {
  char test_request[] = "NOTIFY * HTTP/1.1\r\n"
    "Cache-control: max-age=1800\r\n"
    "Host: 239.255.255.250:1900\r\n"
    "Usn: uuid:a9fe16d3-e374-42a5-ffff-ffffb1ad76d6::upnp:rootdevice\r\n"
    "Location: http://192.168.0.92:36137/dev/a9fe16d3-e374-42a5-ffff-ffffb1ad76d6/desc.xml\r\n"
    "Nt: upnp:rootdevice\r\n"
    "Nts: ssdp:byebye\r\n"
    "Server: Linux/3.1.10-g1e8b3d8 UPnP/1.0 BubbleUPnP/1.7.2.2\r\n\r\n"
    "XXXLocation: http://192.168.0.92:36137/dev/a9fe16d3-e374-42a5-ffff-ffffb1ad76d6/desc.xml\r\n"
    "XXXNt: upnp:rootdevice\r\n";
  
  std::map< std::string, std::string > result = squawk::utils::http::parse_header(sizeof(test_request), test_request);
  EXPECT_EQ(10, result.size());
  EXPECT_EQ(std::string("NOTIFY"), result["method"]);
  EXPECT_EQ(std::string("*"), result["uri"]);
  EXPECT_EQ(std::string("HTTP/1.1"), result["version"]);
  EXPECT_EQ(std::string("max-age=1800"), result["CACHE-CONTROL"]);
  EXPECT_EQ(std::string("239.255.255.250:1900"), result["HOST"]);
  EXPECT_EQ(std::string("uuid:a9fe16d3-e374-42a5-ffff-ffffb1ad76d6::upnp:rootdevice"), result["USN"]);
  EXPECT_EQ(std::string("http://192.168.0.92:36137/dev/a9fe16d3-e374-42a5-ffff-ffffb1ad76d6/desc.xml"), result["LOCATION"]);
  EXPECT_EQ(std::string("upnp:rootdevice"), result["NT"]);
  EXPECT_EQ(std::string("ssdp:byebye"), result["NTS"]);
  EXPECT_EQ(std::string("Linux/3.1.10-g1e8b3d8 UPnP/1.0 BubbleUPnP/1.7.2.2"), result["SERVER"]);
}
