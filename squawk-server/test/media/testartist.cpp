/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <vector>
#include "../../includes/squawk.h"
#include <gtest/gtest.h>

TEST(ParseArtist, CleanArtist) {
    squawk::media::Artist artist("  Artist Name ", "A", "");
    ASSERT_STREQ("artist name", artist.clean_name().c_str() );
}
TEST(ParseArtist, Letter) {
    squawk::media::Artist artist("  Artist Name ", "B", "");
    ASSERT_STREQ("A", artist.letter().c_str() );
}

