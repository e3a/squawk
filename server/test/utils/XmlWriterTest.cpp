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

#include "../../src/utils/xml.h"

#include <gtest/gtest.h>

#define XML_BOOKS "<?xml version=\"1.0\"?>\n<PARTS><TITLE>Computer Parts</TITLE><PART><ITEM>Motherboard</ITEM><MANUFACTURER>ASUS</MANUFACTURER><MODEL>P3B-F</MODEL><COST> 123.00</COST></PART><PART><ITEM>Video Card</ITEM><MANUFACTURER>ATI</MANUFACTURER><MODEL>All-in-Wonder Pro</MODEL><COST> 160.00</COST></PART><PART><ITEM>Sound Card</ITEM><MANUFACTURER>Creative Labs</MANUFACTURER><MODEL>Sound Blaster Live</MODEL><COST> 80.00</COST></PART><PART><ITEM> inch Monitor</ITEM><MANUFACTURER>LG Electronics</MANUFACTURER><MODEL> 995E</MODEL><COST> 290.00</COST></PART></PARTS>\n"

#define XML_ATTRS "<?xml version=\"1.0\"?>\n" \
"<messages>" \
"<note ID=\"501\">" \
"<to>Tove</to>" \
"<from>Jani</from>" \
"<heading>Reminder</heading>" \
"<body>Don't forget me this weekend!</body>" \
"</note>" \
"<note ID=\"502\">" \
"<to>Jani</to>" \
"<from>Tove</from>" \
"<heading>Re: Reminder</heading>" \
"<body>I will not!</body>" \
"</note>" \
"</messages>\n"

#define XML_NS "<?xml version=\"1.0\"?>\n" \
"<bestellung xmlns:produkt=\"http://localhost/XML/produkt\" xmlns:kunde=\"http://localhost/XML/kunde\">" \
"<produkt:nummer>p49393</produkt:nummer>" \
"<produkt:name>JXY Rasierer VC100</produkt:name>" \
"<produkt:menge>1</produkt:menge>" \
"<produkt:preis>69,--</produkt:preis>" \
"<kunde:nummer>k2029</kunde:nummer>" \
"<kunde:name>Meier, Fritz</kunde:name>" \
"<kunde:lieferadresse>Donnerbalkenstr.14, 80111 München</kunde:lieferadresse>" \
"</bestellung>\n"

#define XML_NS_ENCODED "<?xml version=\"1.0\"?>\n" \
"<bestellung xmlns:produkt=\"http://localhost/XML/produkt\" xmlns:kunde=\"http://localhost/XML/kunde\">" \
"<produkt:nummer>p49393</produkt:nummer>" \
"<produkt:name>JXY Rasierer VC100</produkt:name>" \
"<produkt:menge>1</produkt:menge>" \
"<produkt:preis>69,--</produkt:preis>" \
"<kunde:nummer>k2029</kunde:nummer>" \
"<kunde:name>Meier, Fritz</kunde:name>" \
"<kunde:lieferadresse>Donnerbalkenstr.14, 80111 M&#xFC;nchen</kunde:lieferadresse>" \
"</bestellung>\n"

#define XML_ROOT_NS "<?xml version=\"1.0\"?>\n" \
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">" \
"<specVersion>" \
"<major>1</major>" \
"<minor>0</minor>" \
"</specVersion>" \
"<dlna:X_DLNADOC xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">DMS-1.50</dlna:X_DLNADOC>" \
"</root>\n"

#define XML_SOAP_ENVELOPE "<?xml version=\"1.0\"?>\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:BrowseResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><Result xsi:type=\"xsd:string\"/></u:BrowseResponse></s:Body></s:Envelope>\n"

#define XML_SOAP_ENVELOPE_CONTENT "<?xml version=\"1.0\"?>\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:BrowseResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><Result xsi:type=\"xsd:string\">ABC</Result></u:BrowseResponse></s:Body></s:Envelope>\n"

TEST( XmlWriter, SimpleXML ) {

    commons::xml::XMLWriter writer;
    commons::xml::Node root_node = writer.element( "PARTS" );

    writer.element( root_node, "", "TITLE", "Computer Parts" );

    commons::xml::Node child_node = writer.element( root_node, "", "PART", "" );
    writer.element( child_node, "", "ITEM", "Motherboard" );
    writer.element( child_node, "", "MANUFACTURER", "ASUS" );
    writer.element( child_node, "", "MODEL", "P3B-F" );
    writer.element( child_node, "", "COST", " 123.00" );

    child_node = writer.element( root_node, "", "PART", "" );
    writer.element( child_node, "", "ITEM", "Video Card" );
    writer.element( child_node, "", "MANUFACTURER", "ATI" );
    writer.element( child_node, "", "MODEL", "All-in-Wonder Pro" );
    writer.element( child_node, "", "COST", " 160.00" );

    child_node = writer.element( root_node, "", "PART", "" );
    writer.element( child_node, "", "ITEM", "Sound Card" );
    writer.element( child_node, "", "MANUFACTURER", "Creative Labs" );
    writer.element( child_node, "", "MODEL", "Sound Blaster Live" );
    writer.element( child_node, "", "COST", " 80.00" );

    child_node = writer.element( root_node, "", "PART", "" );
    writer.element( child_node, "", "ITEM", " inch Monitor" );
    writer.element( child_node, "", "MANUFACTURER", "LG Electronics" );
    writer.element( child_node, "", "MODEL", " 995E" );
    writer.element( child_node, "", "COST", " 290.00" );

    EXPECT_EQ( XML_BOOKS, writer.str() );
}
TEST( XmlWriter, AtributeXML ) {

    commons::xml::XMLWriter writer;
    commons::xml::Node root_node = writer.element( "messages" );

    commons::xml::Node child_node = writer.element( root_node, "", "note", "" );
    writer.attribute( child_node, "ID", "501" );

    writer.element( child_node, "", "to", "Tove" );
    writer.element( child_node, "", "from", "Jani" );
    writer.element( child_node, "", "heading", "Reminder" );
    writer.element( child_node, "", "body", "Don't forget me this weekend!" );

    child_node = writer.element( root_node, "", "note", "" );
    writer.attribute( child_node, "ID", "502" );

    writer.element( child_node, "", "to", "Jani" );
    writer.element( child_node, "", "from", "Tove" );
    writer.element( child_node, "", "heading", "Re: Reminder" );
    writer.element( child_node, "", "body", "I will not!" );

    EXPECT_EQ( XML_ATTRS, writer.str() );
}
TEST( XmlWriter, NamespaceXML ) {

    commons::xml::XMLWriter writer;
    commons::xml::Node root_node = writer.element( "bestellung" );
    writer.ns( root_node, "http://localhost/XML/produkt", "produkt" );
    writer.ns( root_node, "http://localhost/XML/kunde", "kunde" );

    writer.element( root_node, "http://localhost/XML/produkt", "nummer", "p49393" );
    writer.element( root_node, "http://localhost/XML/produkt", "name", "JXY Rasierer VC100" );
    writer.element( root_node, "http://localhost/XML/produkt", "menge", "1" );
    writer.element( root_node, "http://localhost/XML/produkt", "preis", "69,--" );
    writer.element( root_node, "http://localhost/XML/kunde", "nummer", "k2029" );
    writer.element( root_node, "http://localhost/XML/kunde", "name", "Meier, Fritz" );
    writer.element( root_node, "http://localhost/XML/kunde", "lieferadresse", "Donnerbalkenstr.14, 80111 München" );

    EXPECT_EQ( XML_NS_ENCODED, writer.str() );
}
TEST( XmlWriter, RootNamespaceXML ) {

    commons::xml::XMLWriter writer;
    commons::xml::Node root_node = writer.element( "root" );
    writer.ns( root_node, "urn:schemas-upnp-org:device-1-0" );

    commons::xml::Node spec_node = writer.element( root_node, "", "specVersion" );
    writer.element( spec_node, "", "major", "1" );
    writer.element( spec_node, "", "minor", "0" );

    commons::xml::Node dlna_node = writer.element( root_node, "", "X_DLNADOC", "DMS-1.50" );
    writer.ns( dlna_node, "urn:schemas-dlna-org:device-1-0", "dlna", true );

    EXPECT_EQ( XML_ROOT_NS, writer.str() );
}
TEST( XmlWriter, SoapEnvelope ) {

    commons::xml::XMLWriter xmlWriter;
    commons::xml::Node envelope_node = xmlWriter.element( "Envelope" );
    xmlWriter.ns(envelope_node, "http://schemas.xmlsoap.org/soap/envelope/", "s", true);
    xmlWriter.ns(envelope_node, "http://www.w3.org/2001/XMLSchema-instance", "xsi", false);
    xmlWriter.attribute(envelope_node, "http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    commons::xml::Node body_node = xmlWriter.element(envelope_node, "http://schemas.xmlsoap.org/soap/envelope/", "Body");

    commons::xml::Node browse_response_node = xmlWriter.element(body_node, "", "BrowseResponse");
    xmlWriter.ns(browse_response_node, "urn:schemas-upnp-org:service:ContentDirectory:1", "u", true);

    commons::xml::Node result_node = xmlWriter.element(browse_response_node, "", "Result");
    xmlWriter.attribute(result_node, "http://www.w3.org/2001/XMLSchema-instance", "type", "xsd:string");

    EXPECT_EQ( XML_SOAP_ENVELOPE, xmlWriter.str() );
}
TEST( XmlWriter, SoapEnvelopeWithContnet ) {

    commons::xml::XMLWriter xmlWriter;
    commons::xml::Node envelope_node = xmlWriter.element( "Envelope" );
    xmlWriter.ns(envelope_node, "http://schemas.xmlsoap.org/soap/envelope/", "s", true);
    xmlWriter.ns(envelope_node, "http://www.w3.org/2001/XMLSchema-instance", "xsi", false);
    xmlWriter.attribute(envelope_node, "http://schemas.xmlsoap.org/soap/envelope/", "encodingStyle", "http://schemas.xmlsoap.org/soap/encoding/");
    commons::xml::Node body_node = xmlWriter.element(envelope_node, "http://schemas.xmlsoap.org/soap/envelope/", "Body");

    commons::xml::Node browse_response_node = xmlWriter.element(body_node, "", "BrowseResponse");
    xmlWriter.ns(browse_response_node, "urn:schemas-upnp-org:service:ContentDirectory:1", "u", true);

    commons::xml::Node result_node = xmlWriter.element(browse_response_node, "", "Result", "ABC");
    xmlWriter.attribute(result_node, "http://www.w3.org/2001/XMLSchema-instance", "type", "xsd:string");

    EXPECT_EQ( XML_SOAP_ENVELOPE_CONTENT, xmlWriter.str() );
}
