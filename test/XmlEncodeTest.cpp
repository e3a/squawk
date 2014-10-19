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

#include "../commons/commons.h"

#include <gtest/gtest.h>


TEST( XmlEncodeTest, SimpleText ) {

    std::string test("Lorem ipsum dolor sit amet");
    EXPECT_EQ( test, commons::string::escape_xml( test ) );
}

TEST( XmlEncodeTest, EscapeGT ) {

    std::string test("<Lorem ipsum dolor sit amet>");
    EXPECT_EQ( "&lt;Lorem ipsum dolor sit amet&gt;", commons::string::escape_xml( test ) );
}

TEST( XmlEncodeTest, EscapeUml ) {

    std::string test("Lörem ipsum dölör sit ämet");
    EXPECT_EQ( "L&#246;rem ipsum d&#246;l&#246;r sit &#228;met", commons::string::escape_xml( test ) );
}

TEST( XmlEncodeTest, EscapeExtraEuro ) {

    std::string test("Lör\u20ACm ipsum dölör sit ämet");
    EXPECT_EQ( "L&#246;r&#8364;m ipsum d&#246;l&#246;r sit &#228;met", commons::string::escape_xml( test ) );
}

TEST( XmlEncodeTest, EscapeExtraViolin ) {

    std::string test("Lor𝄞 m ipsum dölör sit ämet");
    EXPECT_EQ( "Lor&#119070; m ipsum d&#246;l&#246;r sit &#228;met", commons::string::escape_xml( test ) );
}
