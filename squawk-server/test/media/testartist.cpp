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
#include "../../src/media/artist.h"
#include <gtest/gtest.h>

TEST(ParseArtist, CleanArtist) {
    squawk::media::Artist artist("  Artist Name " );
    ASSERT_STREQ("artist name", artist.cleanName().c_str() );
}
TEST(ParseArtist, CleanArtistRage) {
    squawk::media::Artist artist("Rage Against The Machine");
    ASSERT_STREQ("rage against the machine", artist.cleanName().c_str() );
}
TEST(ParseArtist, Letter) {
    squawk::media::Artist artist("  Artist Name " );
    ASSERT_STREQ("A", artist.letter().c_str() );
}



