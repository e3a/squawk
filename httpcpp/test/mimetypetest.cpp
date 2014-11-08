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
#include "mimetypes.h"
#include <gtest/gtest.h>

TEST(MimeTypeParseTest2, GetTypeByExtension) {
  EXPECT_TRUE(http::mime::MPEG == http::mime::mime_type("mp3"));
  EXPECT_TRUE(http::mime::VORBIS == http::mime::mime_type("ogg"));
}
TEST(MimeTypeParseTest2, GetByExtension) {
  EXPECT_EQ(std::string("audio/mpeg"), http::mime::mime_type(http::mime::MPEG));
}
TEST(MimeTypeParseTest2, GetExtensionByMimeType) {
    EXPECT_EQ(std::string("mp3"), http::mime::extension(http::mime::MPEG));
    EXPECT_EQ(std::string("flac"), http::mime::extension(http::mime::FLAC));
}
TEST(MimeTypeParseTest2, GetExtensionByMimeTypeText) {
    EXPECT_EQ(std::string("mp3"), http::mime::extension("audio/mpeg"));
    EXPECT_EQ(std::string("flac"), http::mime::extension("audio/x-flac"));
}
