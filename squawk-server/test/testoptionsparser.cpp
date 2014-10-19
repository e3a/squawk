
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
#include "commons.h"
#include "../src/squawkconfig.h"
#include <gtest/gtest.h>

TEST(SquawkParseOptions, TestConfigfile) {
    const char * options[3];
    options[0] = "-r";
    options[1] = "--config-file";
    options[2] = "/foo/bar.properties";
    squawk::SquawkConfig config;

    ASSERT_TRUE(config.parse(3, options));
    EXPECT_EQ(std::string("/foo/bar.properties"), config.string_value(CONFIG_FILE));
    ASSERT_TRUE(config.rescan);
}

TEST(SquawkParseOptions, TestAllOptions) {
    const char * options[25];
    options[0] = "-r";
    options[1] = "--config-file";
    options[2] = "/foo/bar.properties";
    options[3] = "--logger";
    options[4] = "/foo/bar.logger";
    options[5] = "--media-directory";
    options[6] = "/foo/bar";
    options[7] = "--http-ip";
    options[8] = "127.0.0.1";
    options[9] = "--http-port";
    options[10] = "8080";
    options[11] = "--http-docroot";
    options[12] = "/foo/bar/docroot";
    options[13] = "--http-threads";
    options[14] = "20";
    options[15] = "--database-file";
    options[16] = "/foo/bar.db";
    options[17] = "--tmp-directory";
    options[18] = "/foo/bar/tmp";
    options[19] = "--local-address";
    options[20] = "127.0.0.1";
    options[21] = "--multicast-address";
    options[22] = "254.0.0.2";
    options[23] = "--multicast-port";
    options[24] = "1900";

    squawk::SquawkConfig config;

    ASSERT_TRUE(config.parse(25, options));
    EXPECT_EQ(std::string("/foo/bar.properties"), config.string_value(CONFIG_FILE));
    ASSERT_TRUE(config.rescan);
    EXPECT_EQ(std::string("/foo/bar.logger"), config.string_value(CONFIG_LOGGER_PROPERTIES));
    EXPECT_EQ(std::string("/foo/bar"), config.string_value(CONFIG_MEDIA_DIRECTORY));
    EXPECT_EQ(std::string("127.0.0.1"), config.string_value(CONFIG_HTTP_IP));
    EXPECT_EQ(std::string("8080"), config.string_value(CONFIG_HTTP_PORT));
    EXPECT_EQ(std::string("/foo/bar/docroot"), config.string_value(CONFIG_HTTP_DOCROOT));
    EXPECT_EQ(std::string("20"), config.string_value(CONFIG_HTTP_THREADS));
    EXPECT_EQ(std::string("/foo/bar.db"), config.string_value(CONFIG_DATABASE_FILE));
    EXPECT_EQ(std::string("/foo/bar/tmp"), config.string_value(CONFIG_TMP_DIRECTORY));
    EXPECT_EQ(std::string("127.0.0.1"), config.string_value(CONFIG_LOCAL_LISTEN_ADDRESS));
    EXPECT_EQ(std::string("254.0.0.2"), config.string_value(CONFIG_MULTICAST_ADDRESS));
    EXPECT_EQ(std::string("1900"), config.string_value(CONFIG_MULTICAST_PORT));
}

TEST(SquawkParseOptions, TestIncompleteOptions) {
    const char * options[23];
    options[0] = "-r";
    options[1] = "--config-file";
    options[2] = "/foo/bar.properties";
    options[3] = "--logger";
    options[4] = "/foo/bar.logger";
    options[5] = "--media-directory";
    options[6] = "/foo/bar";
    options[7] = "--http-ip";
    options[8] = "127.0.0.1";
    options[9] = "--http-port";
    options[10] = "8080";
    options[11] = "--http-docroot";
    options[12] = "/foo/bar/docroot";
    options[13] = "--http-threads";
    options[14] = "20";
    options[15] = "--database-file";
    options[16] = "/foo/bar.db";
    options[17] = "--local-address";
    options[18] = "127.0.0.1";
    options[19] = "--multicast-address";
    options[20] = "254.0.0.2";
    options[21] = "--multicast-port";
    options[22] = "1900";

    squawk::SquawkConfig config;

    ASSERT_TRUE(config.parse(23, options));
    ASSERT_FALSE(config.validate());
}

TEST(SquawkParseOptions, TestDefaultOptions) {
    const char * options[14];
    options[0] = "--media-directory";
    options[1] = "/foo/bar";
    options[2] = "--http-docroot";
    options[3] = "/foo/bar/docroot";
    options[4] = "--database-file";
    options[5] = "/foo/bar.db";
    options[6] = "--tmp-directory";
    options[7] = "/foo/bar/tmp";
    options[8] = "--config-file";
    options[9] = "/foo/bar.properties";
    options[10] = "--http-ip";
    options[11] = "127.0.0.1";
    options[12] = "--local-address";
    options[13] = "127.0.0.1";

    squawk::SquawkConfig config;

    ASSERT_TRUE(config.parse(14, options));
    ASSERT_TRUE(config.validate());
    EXPECT_EQ(std::string("/foo/bar.properties"), config.string_value(CONFIG_FILE));

    EXPECT_EQ(std::string("/foo/bar"), config.string_value(CONFIG_MEDIA_DIRECTORY));
    EXPECT_EQ(std::string("127.0.0.1"), config.string_value(CONFIG_HTTP_IP));
    EXPECT_EQ(std::string("8080"), config.string_value(CONFIG_HTTP_PORT));
    EXPECT_EQ(std::string("/foo/bar/docroot"), config.string_value(CONFIG_HTTP_DOCROOT));
    EXPECT_EQ(std::string("20"), config.string_value(CONFIG_HTTP_THREADS));
    EXPECT_EQ(std::string("/foo/bar.db"), config.string_value(CONFIG_DATABASE_FILE));
    EXPECT_EQ(std::string("/foo/bar/tmp"), config.string_value(CONFIG_TMP_DIRECTORY));
    EXPECT_EQ(std::string("127.0.0.1"), config.string_value(CONFIG_LOCAL_LISTEN_ADDRESS));
    EXPECT_EQ(std::string("239.255.255.250"), config.string_value(CONFIG_MULTICAST_ADDRESS));
    EXPECT_EQ(std::string("1900"), config.string_value(CONFIG_MULTICAST_PORT));
}

TEST(SquawkParseOptions, TestMergedOptions) {
    const char * options[4];
    options[0] = "--http-ip";
    options[1] = "127.0.0.1";
    options[2] = "--config-file";
    options[3] = "/foo/bar.properties";

    squawk::SquawkConfig config;

    ASSERT_TRUE(config.parse(4, options));
    ASSERT_TRUE(config.load("../../squawk-server/config/squawk.properties"));
    ASSERT_TRUE(config.validate());

    EXPECT_EQ(std::string("127.0.0.1"), config.string_value(CONFIG_HTTP_IP));

    EXPECT_EQ(std::string("/home/media"), config.string_value(CONFIG_MEDIA_DIRECTORY));
    EXPECT_EQ(std::string("8080"), config.string_value(CONFIG_HTTP_PORT));
    EXPECT_EQ(std::string("../docroot"), config.string_value(CONFIG_HTTP_DOCROOT));
    EXPECT_EQ(std::string("20"), config.string_value(CONFIG_HTTP_THREADS));
    EXPECT_EQ(std::string("/var/lib/squawk/squawk.db"), config.string_value(CONFIG_DATABASE_FILE));
    EXPECT_EQ(std::string("/var/tmp/squawk"), config.string_value(CONFIG_TMP_DIRECTORY));
    EXPECT_EQ(std::string("0.0.0.0"), config.string_value(CONFIG_LOCAL_LISTEN_ADDRESS));
    EXPECT_EQ(std::string("239.255.255.250"), config.string_value(CONFIG_MULTICAST_ADDRESS));
    EXPECT_EQ(std::string("1900"), config.string_value(CONFIG_MULTICAST_PORT));
}
