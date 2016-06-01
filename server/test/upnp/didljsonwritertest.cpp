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

#include "upnp.h"
#include "didljsonwriter.h"

#include <gtest/gtest.h>


#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>

namespace didl {


TEST( DidlJsonWriterTest, JsonContainer ) {

    std::string result = "{\"cls\":\"object.container\", \"id\":1, \"parent_id\":0, \"title\":\"title\", \"child_count\":0}";
    std::stringstream ss;

    didl::serializer<DidlContainer>::serialize(ss, DidlContainer( 1, 0, "title", "/path/to", 0, 0, 0 ), std::list< std::string > {} );
    EXPECT_EQ( ss.str(), result );
}

TEST( DidlJsonWriterTest, JsonContainerList ) {

    std::string result = "[{\"cls\":\"object.container\", \"id\":1, \"parent_id\":0, \"title\":\"Music\", \"child_count\":3},"\
                         "{\"cls\":\"object.container\", \"id\":2, \"parent_id\":0, \"title\":\"Video\", \"child_count\":2},"\
                         "{\"cls\":\"object.container\", \"id\":3, \"parent_id\":0, \"title\":\"Image\", \"child_count\":2}]";
    std::stringstream ss;

    auto container =
    std::list<DidlContainer>( {
        DidlContainer( 1, 0, "Music", "/path/to", 0, 0, 3 ),
        DidlContainer( 2, 0, "Video", "/path/to", 0, 0, 2 ),
        DidlContainer( 3, 0, "Image", "/path/to", 0, 0, 2 ),
    });

    didl::serializer< std::list< DidlContainer > >::serialize(ss, container, std::list< std::string > {} );
    EXPECT_EQ( ss.str(), result );
}
TEST( DidlJsonWriterTest, JsonContainerListFilterAttributes ) {

    std::string result = "[{\"id\":1, \"title\":\"Music\"},"\
                         "{\"id\":2, \"title\":\"Video\"},"\
                         "{\"id\":3, \"title\":\"Image\"}]";
    std::stringstream ss;

    auto container =
    std::list<DidlContainer>( {
        DidlContainer( 1, 0, "Music", "/path/to", 0, 0, 3 ),
        DidlContainer( 2, 0, "Video", "/path/to", 0, 0, 2 ),
        DidlContainer( 3, 0, "Image", "/path/to", 0, 0, 2 ),
    });

    didl::serializer< std::list< DidlContainer > >::serialize(ss, container, std::list< std::string > { "id", "title" } );
    EXPECT_EQ( ss.str(), result );
}
TEST( DidlJsonWriterTest, JsonContainerListArtist ) {

    std::string result = "[{\"cls\":\"object.container.person.musicArtist\", \"id\":1, \"parent_id\":0, \"title\":\"Artist 1\", \"child_count\":3},"\
                         "{\"cls\":\"object.container.person.musicArtist\", \"id\":2, \"parent_id\":0, \"title\":\"Artist 2\", \"child_count\":3},"\
                         "{\"cls\":\"object.container.person.musicArtist\", \"id\":3, \"parent_id\":0, \"title\":\"Artist 3\", \"child_count\":3}]";
    std::stringstream ss;

    auto container =
    std::list<DidlContainerArtist>( {
        DidlContainerArtist( 1, 0, "Artist 1", "/path/to", 0, 0, 3, "artist 1", true ),
        DidlContainerArtist( 2, 0, "Artist 2", "/path/to", 0, 0, 3, "artist 2", true ),
        DidlContainerArtist( 3, 0, "Artist 3", "/path/to", 0, 0, 3, "artist 3", true ),
    });

    didl::serializer< std::list< DidlContainerArtist > >::serialize(ss, container, std::list< std::string > {} );
    EXPECT_EQ( ss.str(), result );
}
}
