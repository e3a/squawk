/*
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

#include <gtest/gtest.h>

#include "../upnpcontentdirectoryparser.h"

namespace squawk {
TEST(TestUpnpContentDirectoryParser, ParseMultidiscName) {

    ASSERT_TRUE( UpnpContentDirectoryParser::_multidisc_name( "cd1" ) );
    ASSERT_TRUE( UpnpContentDirectoryParser::_multidisc_name( "CD01" ) );
    ASSERT_FALSE( UpnpContentDirectoryParser::_multidisc_name( "Some Other CD Name" ) );
}
}//squawk


