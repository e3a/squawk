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
#include "../src/media/fileparser.h"
#include <gtest/gtest.h>

TEST(ParseText, CleanArtist) {
    squawk::media::FileParser parser;
    std::string artist1 = std::string("Test Artist Name");
    ASSERT_STREQ("test artist name", parser.get_artist_clean_name(artist1).c_str());
    ASSERT_STREQ("Test Artist Name", artist1.c_str());
}
TEST(ParseText, ArtistLetter) {
    squawk::media::FileParser parser;
    std::string artist1 = std::string("test artist name");
    ASSERT_STREQ("t", parser.get_artist_letter(artist1).c_str());
}
TEST(ParseText, GetPathCD01) {
    squawk::media::FileParser parser;
    std::string path = std::string("/path/CD01");
    ASSERT_STREQ("/path", parser.get_album_clean_path(path).c_str());
}
TEST(ParseText, GetPathCD02) {
    squawk::media::FileParser parser;
    std::string path = std::string("/path/CD02");
    ASSERT_STREQ("/path", parser.get_album_clean_path(path).c_str());
}
TEST(ParseText, GetPathNoCD) {
    squawk::media::FileParser parser;
    std::string path = std::string("/path/Album - 2009 - Artist");
    ASSERT_STREQ("/path/Album - 2009 - Artist", parser.get_album_clean_path(path).c_str());
}
