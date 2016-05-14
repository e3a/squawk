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

#include "../commons/xml.h"

#include <gtest/gtest.h>

#define XML_BOOKS "<?xml version=\"1.0\"?>" \
"<PARTS>" \
"   <TITLE>Computer Parts</TITLE>" \
"   <PART>" \
"      <ITEM>Motherboard</ITEM>" \
"      <MANUFACTURER>ASUS</MANUFACTURER>" \
"      <MODEL>P3B-F</MODEL>" \
"      <COST> 123.00</COST>" \
"   </PART>" \
"   <PART>" \
"      <ITEM>Video Card</ITEM>" \
"      <MANUFACTURER>ATI</MANUFACTURER>" \
"      <MODEL>All-in-Wonder Pro</MODEL>" \
"      <COST> 160.00</COST>" \
"   </PART>" \
"   <PART>" \
"      <ITEM>Sound Card</ITEM>" \
"      <MANUFACTURER>Creative Labs</MANUFACTURER>" \
"      <MODEL>Sound Blaster Live</MODEL>" \
"      <COST> 80.00</COST>" \
"   </PART>" \
"   <PART>" \
"      <ITEM> inch Monitor</ITEM>" \
"      <MANUFACTURER>LG Electronics</MANUFACTURER>" \
"      <MODEL> 995E</MODEL>" \
"      <COST> 290.00</COST>" \
"   </PART>" \
"</PARTS>"


#define XML_ATTRS "<?xml version=\"1.0\"?>" \
"<messages>" \
"  <note ID=\"501\">" \
"    <to>Tove</to>" \
"    <from>Jani</from>" \
"    <heading>Reminder</heading>" \
"    <body>Don't forget me this weekend!</body>" \
"  </note>" \
"  <note ID=\"502\">" \
"    <to>Jani</to>" \
"    <from>Tove</from>" \
"    <heading>Re: Reminder</heading>" \
"    <body>I will not!</body>" \
"  </note> " \
"</messages>"

#define XML_NS "<?xml version=\"1.0\"?>" \
"    <bestellung xmlns:produkt=\"http://localhost/XML/produkt\" xmlns:kunde=\"http://localhost/XML/kunde\">" \
"     <produkt:nummer>p49393</produkt:nummer>" \
"     <produkt:name>JXY Rasierer VC100</produkt:name>" \
"     <produkt:menge>1</produkt:menge>" \
"     <produkt:preis>69,--</produkt:preis>" \
"     <kunde:nummer>k2029</kunde:nummer>" \
"     <kunde:name>Meier, Fritz</kunde:name>" \
"     <kunde:lieferadresse>Donnerbalkenstr.14, 80111 München</kunde:lieferadresse>" \
"    </bestellung>"

TEST( XmlParse, SimpleXML ) {

    commons::xml::XMLReader reader( std::string( XML_BOOKS ) );
    std::vector< commons::xml::Node > list = reader.getElementsByName( "PART" );
    EXPECT_EQ( list.size(), 4 );
    for( auto the_node : list ) {
        EXPECT_EQ( the_node.name(), "PART" );
    }
    commons::xml::Node first_node = list[0];
    std::vector< commons::xml::Node > children = first_node.children();
    EXPECT_EQ( children.size(), 4 );

    EXPECT_EQ( children[0].name(), "ITEM" );
    EXPECT_EQ( children[0].content(), "Motherboard" );
    EXPECT_EQ( children[1].name(), "MANUFACTURER" );
    EXPECT_EQ( children[1].content(), "ASUS" );
    EXPECT_EQ( children[2].name(), "MODEL" );
    EXPECT_EQ( children[2].content(), "P3B-F" );
    EXPECT_EQ( children[3].name(), "COST" );
    EXPECT_EQ( children[3].content(), " 123.00" );
}

TEST( XmlParse, AttrsXML ) {

    commons::xml::XMLReader reader( std::string( XML_ATTRS ) );
    std::vector< commons::xml::Node > list = reader.getElementsByName( "note" );
    EXPECT_EQ( list.size(), 2 );
    for( auto the_node : list ) {
        EXPECT_EQ( the_node.name(), "note" );
    }
    commons::xml::Node first_node = list[0];
    std::vector< commons::xml::Node > children = first_node.children();
    EXPECT_EQ( children.size(), 4 );

    EXPECT_EQ( children[0].name(), "to" );
    EXPECT_EQ( children[0].content(), "Tove" );
    EXPECT_EQ( children[1].name(), "from" );
    EXPECT_EQ( children[1].content(), "Jani" );
    EXPECT_EQ( children[2].name(), "heading" );
    EXPECT_EQ( children[2].content(), "Reminder" );
    EXPECT_EQ( children[3].name(), "body" );
    EXPECT_EQ( children[3].content(), "Don't forget me this weekend!" );

    std::vector< commons::xml::Attribute > atts = list[0].attributes();
    EXPECT_EQ( atts.size(), 1 );
    EXPECT_EQ( atts[0].name(), "ID" );
    EXPECT_EQ( atts[0].content(), "501" );
}

TEST( XmlParse, NamespaceXML ) {

    commons::xml::XMLReader reader( std::string( XML_NS) );
    std::vector< commons::xml::Node > list = reader.getElementsByName( "bestellung" );
    EXPECT_EQ( list.size(), 1 );
    for( auto the_node : list ) {
        EXPECT_EQ( the_node.name(), "bestellung" );
    }
    commons::xml::Node first_node = list[0];
    std::vector< commons::xml::Node > children = first_node.children();
    EXPECT_EQ( children.size(), 7 );

    EXPECT_EQ( children[0].name(), "nummer" );
    EXPECT_EQ( children[0].ns(), "http://localhost/XML/produkt" );
    EXPECT_EQ( children[0].content(), "p49393" );
}
TEST( XmlParse, NamespaceElement ) {
    commons::xml::XMLReader reader( std::string( XML_NS) );
    std::vector< commons::xml::Node > list = reader.getElementsByName( "http://localhost/XML/produkt" , "nummer" );
    EXPECT_EQ( list.size(), 1 );
    for( auto the_node : list ) {
        EXPECT_EQ( the_node.name(), "nummer" );
        EXPECT_EQ( the_node.content(), "p49393" );
    }
}
