/*
    Test api servlet.

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

#include "../src/upnpcontentdirectoryapi.h"

#include <gtest/gtest.h>

namespace squawk {

TEST(UpnpContentDirectoryTest, TestAttributes) {

    std::string attributes = "id,title,child_count";
    std::list< std::string > result  = {"id", "title", "child_count" };
    EXPECT_EQ( result, squawk::UpnpContentDirectoryApi::parse_attributes( attributes ) );

    attributes = "id";
    result  = {"id"};
    EXPECT_EQ( result, squawk::UpnpContentDirectoryApi::parse_attributes( attributes ) );

    attributes = "";
    result  = {};
    EXPECT_EQ( result, squawk::UpnpContentDirectoryApi::parse_attributes( attributes ) );
}
TEST(UpnpContentDirectoryTest, TestFilter) {

    std::string attributes = "title=TitleA,genre=Pop";
    std::map< std::string, std::string > result  = { {"title", "TitleA" }, { "genre", "Pop" } };
    EXPECT_EQ( result, squawk::UpnpContentDirectoryApi::parse_filters( attributes ) );

    attributes = "title=TitleA";
    result  = { { "title", "TitleA" } };
    EXPECT_EQ( result, squawk::UpnpContentDirectoryApi::parse_filters( attributes ) );

    attributes = "";
    result  = {};
    EXPECT_EQ( result, squawk::UpnpContentDirectoryApi::parse_filters( attributes ) );
}
TEST(UpnpContentDirectoryTest, TestSort) {

    std::string attributes = "title,desc";
    std::pair< std::string, std::string > result  = std::make_pair("title", "desc" );
    EXPECT_EQ( result, squawk::UpnpContentDirectoryApi::parse_sort( attributes ) );

    attributes = "id";
    result  = std::make_pair("id", "asc" );
    EXPECT_EQ( result, squawk::UpnpContentDirectoryApi::parse_sort( attributes ) );

    attributes = "";
    result  = {};
    EXPECT_EQ( result, squawk::UpnpContentDirectoryApi::parse_sort( attributes ) );
}
}//squawk
