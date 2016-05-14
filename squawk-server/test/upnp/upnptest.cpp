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

#include "upnp2.h"

#include <gtest/gtest.h>

namespace upnp {
TEST( UpnpTest, ParseRequest ) {
/*
<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns0="urn:schemas-upnp-org:service:ContentDirectory:1" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
   <s:Body>
      <ns0:Browse>
         <ObjectID>0</ObjectID>
         <BrowseFlag>BrowseMetadata</BrowseFlag>
         <Filter>*</Filter>
         <StartingIndex>0</StartingIndex>
         <RequestedCount>0</RequestedCount>
         <SortCriteria />
      </ns0:Browse>
   </s:Body>
</s:Envelope>
*/
    const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:ns0=\"urn:schemas-upnp-org:service:ContentDirectory:1\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><ns0:Browse><ObjectID>0</ObjectID><BrowseFlag>BrowseMetadata</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>0</RequestedCount><SortCriteria /></ns0:Browse></s:Body></s:Envelope>";
    UpnpContentDirectoryRequest command = parseRequest( request );
    EXPECT_EQ( UpnpContentDirectoryRequest::BROWSE, command.type );

    std::vector< std::string > names = command.getNames();
    EXPECT_EQ( 6, names.size() );
    EXPECT_EQ( "ObjectID", names[2] );
    EXPECT_EQ( "BrowseFlag", names[0] );
    EXPECT_EQ( "Filter", names[1] );
    EXPECT_EQ( "StartingIndex", names[5] );
    EXPECT_EQ( "RequestedCount", names[3] );
    EXPECT_EQ( "SortCriteria", names[4] );

    EXPECT_EQ( "0", command.getValue( "ObjectID" ) );
    EXPECT_EQ( "BrowseMetadata", command.getValue( "BrowseFlag" ) );
    EXPECT_EQ( "*", command.getValue( "Filter" ) );
    EXPECT_EQ( "0", command.getValue( "StartingIndex" ) );
    EXPECT_EQ( "0", command.getValue( "RequestedCount" ) );
    EXPECT_EQ( "", command.getValue( "SortCriteria" ) );

}
TEST( UpnpTest, ParseRequest2 ) {
/*
 *<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
   <s:Body>
      <u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
         <ObjectID>0</ObjectID>
         <BrowseFlag>BrowseDirectChildren</BrowseFlag>
         <Filter>*</Filter>
         <StartingIndex>0</StartingIndex>
         <RequestedCount>16</RequestedCount>
         <SortCriteria />
      </u:Browse>
   </s:Body>
</s:Envelope>
*/
    const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><ObjectID>0</ObjectID><BrowseFlag>BrowseDirectChildren</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>16</RequestedCount><SortCriteria></SortCriteria></u:Browse></s:Body></s:Envelope>";

    UpnpContentDirectoryRequest command = parseRequest( request );
    EXPECT_EQ( UpnpContentDirectoryRequest::BROWSE, command.type );

    EXPECT_TRUE( command.contains( "ObjectID" ) );
    EXPECT_TRUE( command.contains( "BrowseFlag", "BrowseDirectChildren" ) );
}
TEST( UpnpTest, ParseRequestStdout ) {
/*
 *<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
   <s:Body>
      <u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
         <ObjectID>0</ObjectID>
         <BrowseFlag>BrowseDirectChildren</BrowseFlag>
         <Filter>*</Filter>
         <StartingIndex>0</StartingIndex>
         <RequestedCount>16</RequestedCount>
         <SortCriteria />
      </u:Browse>
   </s:Body>
</s:Envelope>
*/
    const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><ObjectID>0</ObjectID><BrowseFlag>BrowseDirectChildren</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>16</RequestedCount><SortCriteria></SortCriteria></u:Browse></s:Body></s:Envelope>";
    const char * response = "UpnpContentDirectoryRequest::\tBrowseFlag = BrowseDirectChildren\n\tFilter = *\n\tObjectID = 0\n\tRequestedCount = 16\n\tSortCriteria = \n\tStartingIndex = 0\n";
    UpnpContentDirectoryRequest command = parseRequest( request );
    std::stringstream out;
    out << command;
    EXPECT_EQ( std::string( response ), out.str() );
}
TEST( UpnpTest, ParseRequest3 ) {

    const char * request = "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?><s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><ObjectID>0</ObjectID><BrowseFlag>BrowseDirectChildren</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>50</RequestedCount><SortCriteria></SortCriteria></u:Browse></s:Body></s:Envelope>";
    const char * response = "UpnpContentDirectoryRequest::\tBrowseFlag = BrowseDirectChildren\n\tFilter = *\n\tObjectID = 0\n\tRequestedCount = 50\n\tSortCriteria = \n\tStartingIndex = 0\n";
    UpnpContentDirectoryRequest command = parseRequest( request );
    std::stringstream out;
    out << command;
    EXPECT_EQ( std::string( response ), out.str() );
}
}
