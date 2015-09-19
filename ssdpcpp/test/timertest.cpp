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

#include "ssdp.h"

#include <gtest/gtest.h>


namespace ssdp {
TEST( TimerTest, ParseTimeTest ) {

    auto time = parse_keep_alive( "max-age=1800" );
    EXPECT_EQ( 1800, time );
}
TEST( TimerTest, ParseTimeSpacesTest ) {

    auto time = parse_keep_alive( "max-age = 1800" );
    EXPECT_EQ( 1800, time );
}
TEST( TimerTest, TimeoutCheckTest ) {

    auto last_seen = std::time ( 0 );

    ASSERT_FALSE( check_timeout( last_seen, 1000) );

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    ASSERT_TRUE( check_timeout( last_seen, 1) );

}
} //ssdp
