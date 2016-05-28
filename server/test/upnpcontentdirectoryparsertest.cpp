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
TEST(UpnpContentDirectoryParserTest, ParseMultidiscName) {

    ASSERT_TRUE( UpnpContentDirectoryParser::_multidisc_name( "cd1" ) );
    ASSERT_TRUE( UpnpContentDirectoryParser::_multidisc_name( "CD01" ) );
    ASSERT_TRUE( UpnpContentDirectoryParser::_multidisc_name( "Disk 01" ) );
    ASSERT_TRUE( UpnpContentDirectoryParser::_multidisc_name( "Disc 1of2" ) );
    ASSERT_FALSE( UpnpContentDirectoryParser::_multidisc_name( "Some Other CD Name" ) );
}
TEST(UpnpContentDirectoryParserTest, TestCleanName) {
    ASSERT_STREQ("clean name", UpnpContentDirectoryParser::_clean_name("clean name").c_str());
    ASSERT_STREQ("clean name", UpnpContentDirectoryParser::_clean_name(" clean name ").c_str());
    ASSERT_STREQ("clean name", UpnpContentDirectoryParser::_clean_name(" Clean Name ").c_str());
    ASSERT_STREQ("clean name", UpnpContentDirectoryParser::_clean_name("+-Clean Name ").c_str());
    ASSERT_STREQ("clean name", UpnpContentDirectoryParser::_clean_name("The Clean Name ").c_str());
    ASSERT_STREQ("clean name", UpnpContentDirectoryParser::_clean_name("Das Clean Name ").c_str());
}
TEST(UpnpContentDirectoryParserTest, MimeType ) {

    ASSERT_EQ( http::mime::FLAC, UpnpContentDirectoryParser::_mime_type( ".flac" ) );
    ASSERT_EQ( http::mime::GIF, UpnpContentDirectoryParser::_mime_type( ".gif" ) );
    ASSERT_EQ( http::mime::JPEG, UpnpContentDirectoryParser::_mime_type( ".jpg" ) );
    ASSERT_EQ( http::mime::MKV, UpnpContentDirectoryParser::_mime_type( ".mkv" ) );
    ASSERT_EQ( http::mime::PDF, UpnpContentDirectoryParser::_mime_type( ".pdf" ) );
}
TEST(UpnpContentDirectoryParserTest, FileType ) {

    ASSERT_EQ( didl::objectItemAudioItemMusicTrack, UpnpContentDirectoryParser::_file_type( "audio/x-flac" ) );
    ASSERT_EQ( didl::objectItemImageItemPhoto, UpnpContentDirectoryParser::_file_type( "image/gif" ) );
    ASSERT_EQ( didl::objectItemImageItemPhoto, UpnpContentDirectoryParser::_file_type( "image/jpeg" ) );
    ASSERT_EQ( didl::objectItemVideoItemMovie, UpnpContentDirectoryParser::_file_type( "video/x-matroska" ) );
    ASSERT_EQ( didl::objectItemEBook, UpnpContentDirectoryParser::_file_type( "application/pdf" ) );
}
TEST(UpnpContentDirectoryParserTest, ParseSeries ) {
    int season_, episode_;
    std::string name_;
    ASSERT_TRUE( UpnpContentDirectoryParser::_parse_series( "The.Simpsons.S27E19.720p.HDTV.x264-AVS.[VTV]", &season_, &episode_, &name_ ) );
    ASSERT_EQ( season_, 27 );
    ASSERT_EQ( episode_, 19 );
    ASSERT_EQ( name_, "The Simpsons" );
}
TEST(UpnpContentDirectoryParserTest, GetTypeByExtension) {
  EXPECT_TRUE(http::mime::MPEG == http::mime::mime_type("mp3"));
  EXPECT_TRUE(http::mime::VORBIS == http::mime::mime_type("ogg"));
}
TEST(UpnpContentDirectoryParserTest, GetByExtension) {
  EXPECT_EQ(std::string("audio/mpeg"), http::mime::mime_type(http::mime::MPEG));
}
}//squawk


