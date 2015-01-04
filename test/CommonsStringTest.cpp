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
#include <list>
#include <string>

#include "../commons/commons.h"

#include <gtest/gtest.h>


TEST( CommonsTest, IsNumber ) {
    ASSERT_TRUE( commons::string::is_number( "123" ) );
    ASSERT_FALSE( commons::string::is_number( "ABC" ) );
    ASSERT_FALSE( commons::string::is_number( "A123" ) );
}
TEST( CommonsTest, PathTokens ) {
    std::list<std::string> result = commons::filesystem::getPathTokens( "/foo/bar" );
    ASSERT_EQ( 2, result.size() );
    std::list<std::string>::iterator itr = result.begin();
    ASSERT_STREQ( "foo", (*itr).c_str() );
    ASSERT_STREQ( "bar", (*++itr).c_str() );
}
TEST( CommonsTest, PathTokensStartSlash ) {
    std::list<std::string> result = commons::filesystem::getPathTokens( "//foo/bar" );
    ASSERT_EQ( 2, result.size() );
    std::list<std::string>::iterator itr = result.begin();
    ASSERT_STREQ( "foo", (*itr).c_str() );
    ASSERT_STREQ( "bar", (*++itr).c_str() );
}TEST( CommonsTest, PathTokensEndSlash ) {
    std::list<std::string> result = commons::filesystem::getPathTokens( "/foo/bar/" );
    ASSERT_EQ( 2, result.size() );
    std::list<std::string>::iterator itr = result.begin();
    ASSERT_STREQ( "foo", (*itr).c_str() );
    ASSERT_STREQ( "bar", (*++itr).c_str() );
}
TEST( CommonsTest, PathTokensSlash ) {
    std::list<std::string> result = commons::filesystem::getPathTokens( "//Super/califragi//listic/expialidocious/" );
    ASSERT_EQ( 4, result.size() );
    std::list<std::string>::iterator itr = result.begin();
    ASSERT_STREQ( "Super", (*itr).c_str() );
    ASSERT_STREQ( "califragi", (*++itr).c_str() );
    ASSERT_STREQ( "listic", (*++itr).c_str() );
    ASSERT_STREQ( "expialidocious", (*++itr).c_str() );
}
