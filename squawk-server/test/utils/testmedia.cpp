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

#include "../../src/utils/media.h"

#include <gtest/gtest.h>


/* TEST( VideoTest, ParseFilename ) {
    commons::media::Video video( "/foo/bar/name.avi" );
    ASSERT_STREQ( "name", video.name().c_str() );
} */

TEST( MediaTest, ParseMpegLayer2Mpthreetest) {
  
    commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( std::string(TESTFILES) + "mpthreetest.mp3" );

    ASSERT_EQ( 1, media_file.getAudioStreams().size() );
    ASSERT_EQ( 128000, media_file.getAudioStreams().front().bitrate() );
    ASSERT_EQ( 1, media_file.getAudioStreams().front().channels() );
    ASSERT_EQ( 0, media_file.getAudioStreams().front().bitsPerSample() );
    ASSERT_EQ( 44100, media_file.getAudioStreams().front().sampleFrequency() );
    ASSERT_EQ( 198658, media_file.size() ); // filesize in bytes.
    ASSERT_EQ( 12, media_file.duration() ); //playlength.

    ASSERT_EQ( 6, media_file.tagNames().size() );
    ASSERT_STREQ( "Test of MP3 File", media_file.getTag( commons::media::MediaFile::TITLE ).c_str() );
    ASSERT_STREQ( "Me", media_file.getTag( commons::media::MediaFile::ALBUM ).c_str() );
    //TODO ASSERT_STREQ( "test", media_file.getTag( commons::media::MediaFile::COMMENT ).c_str() );
    ASSERT_STREQ( "Me", media_file.getTag( commons::media::MediaFile::ARTIST ).c_str() );
    ASSERT_STREQ( "2006", media_file.getTag( commons::media::MediaFile::YEAR ).c_str() );
    ASSERT_STREQ( "Other", media_file.getTag( commons::media::MediaFile::GENRE ).c_str() );
    ASSERT_STREQ( "1", media_file.getTag( commons::media::MediaFile::TRACK ).c_str() );
}
TEST( MediaTest, ParseMpegLayer2Sample) {
    commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( std::string(TESTFILES) + "sample.mp3" );

    ASSERT_EQ( 1, media_file.getAudioStreams().size() );
    ASSERT_EQ( 64075, media_file.getAudioStreams().front().bitrate() );
    ASSERT_EQ( 2, media_file.getAudioStreams().front().channels() );
    ASSERT_EQ( 0, media_file.getAudioStreams().front().bitsPerSample() );
    ASSERT_EQ( 44100, media_file.getAudioStreams().front().sampleFrequency() );
    ASSERT_EQ( 35952, media_file.size() ); // filesize in bytes.
    ASSERT_EQ( 4, media_file.duration() ); //playlength.

    ASSERT_EQ( 1, media_file.tagNames().size() );
    ASSERT_STREQ( "Other", media_file.getTag( commons::media::MediaFile::GENRE ).c_str() );
}
TEST( MediaTest, ParseFlacSample) {
    commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( std::string(TESTFILES) + "sample.flac" );

    ASSERT_EQ( 1, media_file.getAudioStreams().size() );
    ASSERT_EQ( 0, media_file.getAudioStreams().front().bitrate() );
    ASSERT_EQ( 1, media_file.getAudioStreams().front().channels() );
    ASSERT_EQ( 16, media_file.getAudioStreams().front().bitsPerSample() );
    ASSERT_EQ( 8000, media_file.getAudioStreams().front().sampleFrequency() );
    ASSERT_EQ( 61522, media_file.size() ); // filesize in bytes.
    ASSERT_EQ( 4, media_file.duration() ); //playlength.

    ASSERT_EQ( 0, media_file.tagNames().size() );
}

TEST( MediaTest, ParseAviSample) {
    commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( std::string(TESTFILES) +  "sample.avi" );

    ASSERT_EQ( 0, media_file.getAudioStreams().size() );

    ASSERT_EQ( 1, media_file.getVideoStreams().size() );
    ASSERT_EQ( 256, media_file.getVideoStreams()[0].width() );
    ASSERT_EQ( 240, media_file.getVideoStreams()[0].height() );

    ASSERT_EQ( 6, media_file.duration() );
    ASSERT_EQ( 675840, media_file.size() );
    ASSERT_EQ( 0, media_file.tagNames().size() );

}
TEST( MediaTest, ParseDivxMicayala_DivX1080p_ASP) {
    commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( std::string(TESTFILES) +  "Micayala_DivX1080p_ASP.divx" );

    ASSERT_EQ( 1, media_file.getAudioStreams().size() );
    ASSERT_EQ( 192000, media_file.getAudioStreams().front().bitrate() );
    ASSERT_EQ( 2, media_file.getAudioStreams().front().channels() );
    ASSERT_EQ( 0, media_file.getAudioStreams().front().bitsPerSample() );
    ASSERT_EQ( 44100, media_file.getAudioStreams().front().sampleFrequency() );

    ASSERT_EQ( 1, media_file.getVideoStreams().size() );
    ASSERT_EQ( 1920, media_file.getVideoStreams()[0].width() );
    ASSERT_EQ( 768, media_file.getVideoStreams()[0].height() );

    ASSERT_EQ( 137, media_file.duration() );
    ASSERT_EQ( 0, media_file.tagNames().size() );

}
TEST( MediaTest, ParseDivxWiegelesHeliSki_DivXPlus_19Mbps) {
    commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( std::string(TESTFILES) +  "WiegelesHeliSki_DivXPlus_19Mbps.mkv" );

    ASSERT_EQ( 1, media_file.getAudioStreams().size() );
    ASSERT_EQ( 192000, media_file.getAudioStreams().front().bitrate() );
    ASSERT_EQ( 2, media_file.getAudioStreams().front().channels() );
    ASSERT_EQ( 0, media_file.getAudioStreams().front().bitsPerSample() );
    ASSERT_EQ( 44100, media_file.getAudioStreams().front().sampleFrequency() );

    ASSERT_EQ( 1, media_file.getVideoStreams().size() );
    ASSERT_EQ( 1920, media_file.getVideoStreams()[0].width() );
    ASSERT_EQ( 1080, media_file.getVideoStreams()[0].height() );

    ASSERT_EQ( 220, media_file.duration() );

    ASSERT_EQ( 2, media_file.tagNames().size() );
    ASSERT_STREQ( "Alterna Films 2012", media_file.getTag( commons::media::MediaFile::TITLE ).c_str() );
//TODO    ASSERT_STREQ( "Wiegeles HeliSki", media_file.getTag( commons::media::MediaFile::TITLE ).c_str() );
    ASSERT_STREQ( "Encoded in DivX Plus HD!", media_file.getTag( commons::media::MediaFile::COMMENT ).c_str() );

}
TEST( MediaTest, ParseMkvCover ) {
    commons::media::MediaFile media_file = commons::media::MediaParser::parseFile( std::string(TESTFILES) +  "cover_art.mkv" );

    ASSERT_EQ( 1, media_file.getAudioStreams().size() );
    ASSERT_EQ( 0, media_file.getAudioStreams().front().bitrate() );
    ASSERT_EQ( 2, media_file.getAudioStreams().front().channels() );
    ASSERT_EQ( 0, media_file.getAudioStreams().front().bitsPerSample() );
    ASSERT_EQ( 44100, media_file.getAudioStreams().front().sampleFrequency() );

    ASSERT_EQ( 1, media_file.getVideoStreams().size() );
    ASSERT_EQ( 1272, media_file.getVideoStreams()[0].width() );
    ASSERT_EQ( 720, media_file.getVideoStreams()[0].height() );

    ASSERT_EQ( 156, media_file.duration() );

    ASSERT_EQ( 2, media_file.tagNames().size() );
    ASSERT_STREQ( "Dexter Season 5 trailer", media_file.getTag( commons::media::MediaFile::TITLE ).c_str() );
//TODO    ASSERT_STREQ( "Wiegeles HeliSki", media_file.getTag( commons::media::MediaFile::TITLE ).c_str() );
    ASSERT_STREQ( "", media_file.getTag( commons::media::MediaFile::COMMENT ).c_str() );

}
