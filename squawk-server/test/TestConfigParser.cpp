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
#include "squawk.h"
#include <gtest/gtest.h>

TEST(TestConfigParser, LoadConfig) {



    ASSERT_STREQ("clean name", ::squawk::media::clean_name("clean name").c_str());
    ASSERT_STREQ("clean name", ::squawk::media::clean_name(" clean name ").c_str());
    ASSERT_STREQ("clean name", ::squawk::media::clean_name(" Clean Name ").c_str());
    ASSERT_STREQ("clean name", ::squawk::media::clean_name("+-Clean Name ").c_str());
    ASSERT_STREQ("clean name", ::squawk::media::clean_name("The Clean Name ").c_str());
    ASSERT_STREQ("clean name", ::squawk::media::clean_name("Das Clean Name ").c_str());
}

