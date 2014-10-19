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

#include "../src/squawkservice.h"

#include <string>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Return;
class SquawkServiceMock : public squawk::SquawkService {
public:
/*
  MOCK_CONST_METHOD0(exist_audiofile, std::string(const std::string path));
  MOCK_CONST_METHOD0(exist_imagefile, std::string(const std::string path));

  MOCK_CONST_METHOD0(get_albums, void());
  MOCK_CONST_METHOD0(exist_imagefile, std::string(const std::string ah));
 */
  /*
  std::list< squawk::model::Album > get_albums_by_artist(unsigned long artist_id) {};
  // std::list< squawk::model::Artist > get_artists() {};
  squawk::model::Album get_album(unsigned long id, bool recursive = false) {};
  squawk::model::Album get_album(std::string path) {};
  
  unsigned long save_artist(squawk::model::Artist * artist) {};
  unsigned long save_album(string path, squawk::model::Album * album) {};
  unsigned long save_audiofile(std::string filename, long mtime, long size, unsigned long album_id, Audiofile * audiofile) {};
  unsigned long save_imagefile(std::string filename, long mtime, long size, unsigned long album_id, Imagefile * imagefile) {};
  */
};

TEST(ApiArtistHandler, SimpleCall) {
/*    SquawkServiceMock mock;
    squawk::api::ApiArtistHandler handler(mock);
    EXPECT_CALL(mock, get_artists())
        .WillOnce(Return(new std::list()));
	
      squawk::http::HttpRequest request();
      squawk::http::reply reply();
      handler.handle_request(string("/api/artist"), &request, &reply, buffer[0]);
  */
//    std::vector<std::string> links = parser.GetAllLinks("http://example.net");
//    EXPECT_EQ(1, links.size());
//    EXPECT_STREQ("http://example.net/index.html", links[0].c_str());
}

