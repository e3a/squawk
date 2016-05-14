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

#include "../upnpcontentdirectorydao.h"

namespace squawk {
TEST(TestUpnpContentDirectoryDao, ClassType) {
    ASSERT_EQ(didl::object, DidlType< didl::DidlObject >::cls());
    ASSERT_EQ(didl::objectContainer, DidlType< didl::DidlContainer >::cls());
    ASSERT_EQ(didl::objectItem, DidlType< didl::DidlItem >::cls());
    ASSERT_EQ(didl::objectItemAudioItemMusicTrack, DidlType< didl::DidlMusicTrack >::cls());
    ASSERT_EQ(didl::objectItemImageItemPhoto, DidlType< didl::DidlPhoto >::cls());
    ASSERT_EQ(didl::objectItemVideoItemMovie, DidlType< didl::DidlMovie>::cls());
}

TEST(TestUpnpContentDirectoryDao, DidlObjectMemberNames) {
    std::stringstream ss;
    SqlParameters< didl::DidlObject >::serialize(ss, true, "");
    ASSERT_EQ("cls, ROWID, parent_id, title, path, mtime, object_update_id, import", ss.str() );
}
TEST(TestUpnpContentDirectoryDao, DidlContainerMemberNames) {
    std::stringstream ss;
    SqlParameters< didl::DidlContainer>::serialize(ss, true, "");
    ASSERT_EQ("cls, ROWID, parent_id, title, path, mtime, object_update_id, import", ss.str() );
}
TEST(TestUpnpContentDirectoryDao, DidlContainerMemberNamesUpdate) {
    std::stringstream ss;
    SqlParameters< didl::DidlContainer>::serialize(ss, false, "=?");
    ASSERT_EQ("cls=?, parent_id=?, title=?, path=?, mtime=?, object_update_id=?, import=?", ss.str() );
}

TEST(TestUpnpContentDirectoryDao, DidlContainerPlaceholders) {
    std::stringstream ss;
    SqlParameters< didl::DidlContainer>::placeholders(ss, false);
    ASSERT_EQ("?, ?, ?, ?, ?, ?, ?", ss.str() );
}



}//squawk



