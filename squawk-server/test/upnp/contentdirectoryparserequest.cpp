/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include <vector>
#include "../src/http/upnp/contentdirectory.h"
#include <gtest/gtest.h>

TEST(ContentDirectoryRequestParser, ParseWithRootNsDefinitions) {

  const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:ns0=\"urn:schemas-upnp-org:service:ContentDirectory:1\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><ns0:Browse><ObjectID>0</ObjectID><BrowseFlag>BrowseMetadata</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>0</RequestedCount><SortCriteria /></ns0:Browse></s:Body></s:Envelope>";

  squawk::http::upnp::ContentDirectory cds;
  cds.parseRequest(std::string(request));
}

TEST(ContentDirectoryRequestParser, ParseWithMultipleNsDefinitions) {

  const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><ObjectID>0</ObjectID><BrowseFlag>BrowseDirectChildren</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>16</RequestedCount><SortCriteria></SortCriteria></u:Browse></s:Body></s:Envelope>";

  squawk::http::upnp::ContentDirectory cds;
  cds.parseRequest(std::string(request));
}
