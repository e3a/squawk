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
#include "ssdp.h"

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
#define DESCRIPTION "<?xml version=\"1.0\"?>"\
    "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"\
    "<specVersion>"\
    "<major>1</major>"\
    "<minor>0</minor>"\
    "</specVersion>"\
    "<device>"\
    "<deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType>"\
    "<presentationURL>http://192.168.0.8:49152/</presentationURL>"\
    "<friendlyName>MediaTomb</friendlyName>"\
    "<manufacturer>(c) 2005-2008 Gena Batyan &lt;bgeradz@mediatomb.cc&gt;, Sergey Bostandzhyan &lt;jin@mediatomb.cc&gt;, Leonhard Wimmer &lt;leo@mediatomb.cc&gt;</manufacturer>"\
    "<manufacturerURL>http://mediatomb.cc/</manufacturerURL>"\
    "<modelDescription>Free UPnP AV MediaServer, GNU GPL</modelDescription>"\
    "<modelName>MediaTomb</modelName>"\
    "<modelNumber>0.12.1</modelNumber>"\
    "<serialNumber>1</serialNumber>"\
    "<UDN>uuid:77c7acd6-5257-4c82-971d-03f9e4e2c91a</UDN>"\
    "<iconList>"\
    "<icon>"\
    "<mimetype>image/png</mimetype>"\
    "<width>120</width>"\
    "<height>120</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon120.png</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/x-ms-bmp</mimetype>"\
    "<width>120</width>"\
    "<height>120</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon120.bmp</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/jpeg</mimetype>"\
    "<width>120</width>"\
    "<height>120</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon120.jpg</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/png</mimetype>"\
    "<width>48</width>"\
    "<height>48</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon48.png</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/x-ms-bmp</mimetype>"\
    "<width>48</width>"\
    "<height>48</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon48.bmp</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/jpeg</mimetype>"\
    "<width>48</width>"\
    "<height>48</height>"\
    "<depth>24</depth>"\
    "<url>/icons/mt-icon48.jpg</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/png</mimetype>"\
    "<width>32</width>"\
    "<height>32</height>"\
    "<depth>8</depth>"\
    "<url>/icons/mt-icon32.png</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/x-ms-bmp</mimetype>"\
    "<width>32</width>"\
    "<height>32</height>"\
    "<depth>8</depth>"\
    "<url>/icons/mt-icon32.bmp</url>"\
    "</icon>"\
    "<icon>"\
    "<mimetype>image/jpeg</mimetype>"\
    "<width>32</width>"\
    "<height>32</height>"\
    "<depth>8</depth>"\
    "<url>/icons/mt-icon32.jpg</url>"\
    "</icon>"\
    "</iconList>"\
    "<serviceList>"\
    "<service>"\
    "<serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>"\
    "<serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>"\
    "<SCPDURL>cm.xml</SCPDURL>"\
    "<controlURL>/upnp/control/cm</controlURL>"\
    "<eventSubURL>/upnp/event/cm</eventSubURL>"\
    "</service>"\
    "<service>"\
    "<serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>"\
    "<serviceId>urn:upnp-org:serviceId:ContentDirectory</serviceId>"\
    "<SCPDURL>cds.xml</SCPDURL>"\
    "<controlURL>/upnp/control/cds</controlURL>"\
    "<eventSubURL>/upnp/event/cds</eventSubURL>"\
    "</service>"\
    "</serviceList>"\
    "</device>"\
    "<URLBase>http://192.168.0.8:49152/</URLBase>"\
    "</root>"

TEST( UpnpTest, UpnpDeviceParse ) {

    didl::SSDPServerImpl server("127.0.0.1", "255.255.255.0", 1900);
    didl::UpnpDevice device;
    upnp::parseDescription( DESCRIPTION, device );

    EXPECT_EQ( device.versionMajor(), 1 );
    EXPECT_EQ( device.versionMinor(), 0 );

    EXPECT_EQ( device.deviceType(), "urn:schemas-upnp-org:device:MediaServer:1" );
    EXPECT_EQ( device.presentationUrl(), "http://192.168.0.8:49152/" );
    EXPECT_EQ( device.friendlyName(), "MediaTomb" );
    EXPECT_EQ( device.manufacturer(), "(c) 2005-2008 Gena Batyan <bgeradz@mediatomb.cc>, Sergey Bostandzhyan <jin@mediatomb.cc>, Leonhard Wimmer <leo@mediatomb.cc>" );
    EXPECT_EQ( device.manufacturerUrl(), "http://mediatomb.cc/" );
    EXPECT_EQ( device.modelDescription(), "Free UPnP AV MediaServer, GNU GPL" );
    EXPECT_EQ( device.modelName(), "MediaTomb" );
    EXPECT_EQ( device.modelNumber(), "0.12.1" );
    EXPECT_EQ( device.modelUrl(), "" );
    EXPECT_EQ( device.serialNumber(), "1" );
    EXPECT_EQ( device.udn(), "uuid:77c7acd6-5257-4c82-971d-03f9e4e2c91a" );
    EXPECT_EQ( device.upc(), "" );

    EXPECT_EQ( device.iconList().size(), 9 );
    EXPECT_EQ( device.iconList()[0].mimeType(), "image/png" );
    EXPECT_EQ( device.iconList()[0].width(), 120 );
    EXPECT_EQ( device.iconList()[0].height(), 120 );
    EXPECT_EQ( device.iconList()[0].depth(), 24 );
    EXPECT_EQ( device.iconList()[0].url(), "/icons/mt-icon120.png" );

    EXPECT_EQ( device.serviceList().size(), 2 );
    EXPECT_EQ( device.serviceList()[1].serviceType(), "urn:schemas-upnp-org:service:ContentDirectory:1" );
    EXPECT_EQ( device.serviceList()[1].serviceId(), "urn:upnp-org:serviceId:ContentDirectory" );
    EXPECT_EQ( device.serviceList()[1].scpdUrl(), "cds.xml" );
    EXPECT_EQ( device.serviceList()[1].controlUrl(), "/upnp/control/cds" );
    EXPECT_EQ( device.serviceList()[1].eventSubUrl(), "/upnp/event/cds" );
}
}//namespace upnp
