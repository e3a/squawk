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
#include <map>
#include <string>
#include <sstream>

#include "upnp2.h"

#include <gtest/gtest.h>


#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>

namespace didl {

#define SEQ (w)(x)(y)(z)

#define MACRO(r, data, elem) \
    BOOST_PP_STRINGIZE ( BOOST_PP_ITERATION_DEPTH() ) \
    BOOST_PP_STRINGIZE (elem) /*, data) */

#define RESULT BOOST_PP_SEQ_FOR_EACH(MACRO, _, SEQ)
 // expands to w_ x_ y_ z_

TEST( DidlTest, DidlObjectMacro ) {
//    std::cout << RESULT << std::endl;
}

TEST( DidlTest, ClassTypes ) {
    EXPECT_EQ( objectContainer, cls<DidlContainer>::type() );
}

TEST( DidlTest, GetNamesObject ) {
    std::ostringstream buffer_;
    DidlObject obj = DidlObject( object, 1, 0, "title", "/path/to", 123, 1, true);
    Struct< DidlObject >::serialize(buffer_, obj);
    EXPECT_STREQ( buffer_.str().c_str(), "cls, ROWID, parent_id, title, path, mtime, object_update_id, import" );
}

TEST( DidlTest, GetNamesContainer ) {
    std::ostringstream buffer_;
    DidlContainer obj = DidlContainer( 1, 0, "title", "/path/to", 123, 1, 13, true);
    Struct< DidlContainer >::serialize(buffer_, obj);
    EXPECT_STREQ( buffer_.str().c_str(), "cls, ROWID, parent_id, title, path, mtime, object_update_id, child_count, import" );
}
TEST( DidlTest, CreateContainer ) {
//    DidlContainer obj = DIDL_CONTAINER_EMPTY;

    std::ostringstream buffer_;
    DidlContainer obj = DidlContainer( 1, 0, "title", "/path/to", 123, 1, 13, true);
    Struct< DidlContainer >::bind(buffer_, obj);
    std::cout << buffer_.str() << std::endl;

    EXPECT_STREQ( buffer_.str().c_str(), "cls, ROWID, parent_id, title, path, mtime, object_update_id, child_count, import" );
}


TEST( DidlTest, RootElement ) {

    commons::xml::XMLWriter writer;
    DidlWriter element( &writer );

    commons::xml::XMLReader reader( writer.str() );
    EXPECT_TRUE( reader.next() );
    commons::xml::Node node = reader.node();

    EXPECT_EQ( node.name(), "DIDL-Lite" );
    EXPECT_EQ( node.children().size(), 0 );

    std::string result = "<?xml version=\"1.0\"?>\n<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:pv=\"http://www.pv.com/pvns/\"/>\n";
    EXPECT_EQ( writer.str(), result );
}
TEST( DidlTest, ClassContainer ) {

    commons::xml::XMLWriter writer;
    DidlWriter element( &writer );
    element.container( "", "", DidlContainer( 1, 0, "title", "/path/to", 0, 0, 0 ) );

    commons::xml::XMLReader reader( writer.str() );
    EXPECT_TRUE( reader.next() );
    commons::xml::Node node = reader.node();

    EXPECT_EQ( node.name(), "DIDL-Lite" );
    EXPECT_EQ( node.children().size(), 1 );

}
TEST( DidlTest, ClassContainerList ) {

    commons::xml::XMLWriter writer;
    DidlWriter element( &writer );
    element.container( "/music/%d", "/music", std::list<DidlContainer>( {
        DidlContainer( 1, 0, "Music", "/path/to", 0, 0, 3 ),
        DidlContainer( 2, 0, "Video", "/path/to", 0, 0, 2 ),
        DidlContainer( 3, 0, "Image", "/path/to", 0, 0, 2 ),
    }));

    commons::xml::XMLReader reader( writer.str() );
    EXPECT_TRUE( reader.next() );
    commons::xml::Node node = reader.node();

    EXPECT_EQ( node.name(), "DIDL-Lite" );
    EXPECT_EQ( node.children().size(), 3 );

    std::string result = "<?xml version=\"1.0\"?>\n" \
            "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:pv=\"http://www.pv.com/pvns/\"><container id=\"/music/1\" parentID=\"/music\" restricted=\"1\" childCount=\"3\"><dc:title>Music</dc:title><upnp:class>object.container</upnp:class></container><container id=\"/music/2\" parentID=\"/music\" restricted=\"1\" childCount=\"2\"><dc:title>Video</dc:title><upnp:class>object.container</upnp:class></container><container id=\"/music/3\" parentID=\"/music\" restricted=\"1\" childCount=\"2\"><dc:title>Image</dc:title><upnp:class>object.container</upnp:class></container></DIDL-Lite>\n";

    EXPECT_EQ( writer.str(), result );
}

TEST( DidlTest, ClassContainerAlbum ) {

    commons::xml::XMLWriter writer;
    DidlWriter element( &writer );

    element.container( "/album/%d", "/album", DidlContainerAlbum( 4890, 0, "Impur", "/path/to", 0, 0, 14, 0, 2006, 0, "Fred Frith", "Fred Frith", "Jazz",
                       std::list< DidlAlbumArtUri >( {DidlAlbumArtUri( 0, 1, "/path", "http://192.168.0.13:8080/api/album/4890/jpeg_tn.jpg", "JPEG_TN" ) } ) ) );

    commons::xml::XMLReader reader( writer.str() );
    EXPECT_TRUE( reader.next() );
    commons::xml::Node node = reader.node();

    EXPECT_EQ( node.name(), "DIDL-Lite" );
    EXPECT_EQ( node.children().size(), 1 );

    std::string result = "<?xml version=\"1.0\"?>\n" \
            "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:pv=\"http://www.pv.com/pvns/\"><container id=\"/album/4890\" parentID=\"/album\" restricted=\"1\" childCount=\"14\"><dc:title>Impur</dc:title><upnp:class>object.container.album.musicAlbum</upnp:class><upnp:artist>Fred Frith</upnp:artist><dc:creator>Fred Frith</dc:creator><dc:date>2006-01-01</dc:date><upnp:albumArtURI xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" dlna:profileID=\"JPEG_TN\">http://192.168.0.13:8080/api/album/4890/jpeg_tn.jpg</upnp:albumArtURI></container></DIDL-Lite>\n";

    EXPECT_EQ( writer.str(), result );
}
TEST( DidlTest, ClassContainerArtist ) {

    commons::xml::XMLWriter writer;
    DidlWriter element( &writer );
    element.container( "/artist/%d", "/artist", DidlContainerArtist( 872, 0, "Bliss", "/path/to/", 0, 0, 1, "clean name" ) );

    commons::xml::XMLReader reader( writer.str() );
    EXPECT_TRUE( reader.next() );
    commons::xml::Node node = reader.node();

    EXPECT_EQ( node.name(), "DIDL-Lite" );
    EXPECT_EQ( node.children().size(), 1 );

    std::string result = "<?xml version=\"1.0\"?>\n" \
            "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:pv=\"http://www.pv.com/pvns/\"><container id=\"/artist/872\" parentID=\"/artist\" restricted=\"1\" childCount=\"1\"><dc:title>Bliss</dc:title><upnp:class>object.container.person.musicArtist</upnp:class></container></DIDL-Lite>\n";

    EXPECT_EQ( writer.str(), result );
}
TEST( DidlTest, ClassMusicTrack ) {

    commons::xml::XMLWriter writer;
    DidlWriter element( &writer );
    element.write( "/file/%d", "/album/%d", "http://192.168.0.13:8080/audio/&d.%s",
                   DidlMusicTrack( 89904, 4890, "Impur", "/path/to", 0, 0, 1450022766, "audio/x-flac", 0, 2006, 1, 0, "Fred Frith", "Fred Frith", "Avant Garde", "Impur", 0,
                                   std::list< DidlResource >( { DidlResource(0, 0, "http://192.168.0.13:8080/audio/89904.flac",
                                                                    "http-get:*:audio/x-flac:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000",
                                                                    std::map< DidlResource::UPNP_RES_ATTRIBUTES, std::string > ( {
                                                                        { DidlResource::duration, "3296" },
                                                                        { DidlResource::size, "344180785" },
                                                                        { DidlResource::bitrate, "0" },
                                                                        { DidlResource::bitsPerSample, "16" },
                                                                        { DidlResource::sampleFrequency, "44100" }
                                                                         } ) ) } ) ) );


    commons::xml::XMLReader reader( writer.str() );
    EXPECT_TRUE( reader.next() );
    commons::xml::Node node = reader.node();

    EXPECT_EQ( node.name(), "DIDL-Lite" );
    EXPECT_EQ( node.children().size(), 1 );

    std::string result = "<?xml version=\"1.0\"?>\n" \
            "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:pv=\"http://www.pv.com/pvns/\"><item id=\"/file/89904\" parentID=\"/album/4890\" restricted=\"1\"><upnp:class>object.item.audioItem.musicTrack</upnp:class><dc:title>Impur</dc:title><dc:originalTrackNumber>1</dc:originalTrackNumber><upnp:album>Impur</upnp:album><dc:date>2006-01-01</dc:date><upnp:genre>Avant Garde</upnp:genre><upnp:artist>Fred Frith</upnp:artist><dc:contributor>Fred Frith</dc:contributor><res protocolInfo=\"http-get:*:audio/x-flac:DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000\" size=\"344180785\" duration=\"3296\" bitrate=\"0\" bitsPerSample=\"16\" sampleFrequency=\"44100\">http://192.168.0.13:8080/audio/89904.flac</res></item></DIDL-Lite>\n";

    EXPECT_EQ( writer.str(), result );
}
TEST( DidlTest, ClassContainerVideo ) {

    commons::xml::XMLWriter writer;
    DidlWriter element( &writer );
    element.write( "/video/%d", "/video", DidlMovie( 76269, 0, "AVATAR 3D", "", 0, 0,
                                                    1,"") ); /*,  "2014-01-01", "02:41:41.000",
                                   std::list< DidlAlbumArtUri >( ),
                                   std::list< DidlAudioItemRes >( { DidlAudioItemRes("http://192.168.0.13:8080/video/76269.mkv",
                                                                    "http-get:*:video/x-matroska:*",
                                                                    std::map< DidlAudioItemRes::UPNP_RES_ATTRIBUTES, std::string > ( {
                                                                        { DidlAudioItemRes::duration, "3296" },
                                                                        { DidlAudioItemRes::size, "1511813286" },
                                                                        { DidlAudioItemRes::bitrate, "0" },
                                                                        { DidlAudioItemRes::sampleFrequency, "48000" },
                                                                        { DidlAudioItemRes::resolution, "1920x1080" },
                                                                        { DidlAudioItemRes::channels, "6" }
                                                                         } ) ) } ) ) ); */

    commons::xml::XMLReader reader( writer.str() );
    EXPECT_TRUE( reader.next() );
    commons::xml::Node node = reader.node();

    EXPECT_EQ( node.name(), "DIDL-Lite" );
    EXPECT_EQ( node.children().size(), 1 );

    std::string result = "<?xml version=\"1.0\"?>\n" \
            "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:pv=\"http://www.pv.com/pvns/\"><item id=\"/video/76269\" parentID=\"/video\" restricted=\"1\"><upnp:class>object.item.videoItem.movie</upnp:class><dc:title>AVATAR 3D</dc:title><dc:date>2014-01-01</dc:date><pv:modificationTime>02:41:41.000</pv:modificationTime><res protocolInfo=\"http-get:*:video/x-matroska:*\" size=\"1511813286\" duration=\"3296\" bitrate=\"0\" sampleFrequency=\"48000\" nrAudioChannels=\"6\" resolution=\"1920x1080\">http://192.168.0.13:8080/video/76269.mkv</res></item></DIDL-Lite>\n";

    EXPECT_EQ( writer.str(), result );
}
//TEST( DidlTest, ClassContainerImage ) {

//    commons::xml::XMLWriter writer;
//    DidlElement element( &writer );
//    element.write( DidlPhoto( "718", "50", "Cover.jpg", 1, "", "1450022766",
//                                   std::list< DidlAlbumArtUri >( ),
//                                   std::list< DidlAudioItemRes >( { DidlAudioItemRes("http://192.168.0.8:49152/content/media/object_id/718/res_id/0/ext/file.jpg",
//                                                                    "http-get:*:image/jpeg:*",
//                                                                    std::map< UPNP_RES_ATTRIBUTES, std::string > ( {
//                                                                        { resolution, "3371x2144" },
//                                                                        { size, "1581341" },
//                                                                         } ) ),
//                                                                    DidlAudioItemRes("http://192.168.0.8:49152/content/media/object_id/718/res_id/1/rct/EX_TH/ext/file.jpg",
//                                                                    "http-get:*:image/jpeg:*",
//                                                                    std::map< UPNP_RES_ATTRIBUTES, std::string > ( {
//                                                                        { resolution, "160x102" },
//                                                                         } ) )
//                                                                  } ) ) );

//    commons::xml::XMLReader reader( writer.str() );
//    EXPECT_TRUE( reader.next() );
//    commons::xml::Node node = reader.node();

//    EXPECT_EQ( node.name(), "DIDL-Lite" );
//    EXPECT_EQ( node.children().size(), 1 );

//    std::string result = "<?xml version=\"1.0\"?>\n" \
//            "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:pv=\"http://www.pv.com/pvns/\"><item id=\"718\" parentID=\"50\" restricted=\"1\"><upnp:class>object.item.imageItem.photo</upnp:class><dc:title>Cover.jpg</dc:title><dc:date>2014-01-01</dc:date><pv:modificationTime>1450022766</pv:modificationTime><res protocolInfo=\"http-get:*:image/jpeg:*\" size=\"1581341\" resolution=\"3371x2144\">http://192.168.0.8:49152/content/media/object_id/718/res_id/0/ext/file.jpg</res><res protocolInfo=\"http-get:*:image/jpeg:*\" resolution=\"160x102\">http://192.168.0.8:49152/content/media/object_id/718/res_id/1/rct/EX_TH/ext/file.jpg</res></item></DIDL-Lite>\n";

//    EXPECT_EQ( writer.str(), result );
//}
}
