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

#include "http.h"
#include <gtest/gtest.h>

class HttpServerImpl : public ::http::HttpServlet {
public:
    HttpServerImpl(std::string path) : HttpServlet(path) {};
};

TEST(HttpServlet, ParsePath) {
    HttpServerImpl servlet("/.*");
    EXPECT_TRUE(servlet.match("/foo/bar"));
}
TEST(HttpServlet, ParsePathWithElement) {
    HttpServerImpl servlet("/foo/bar/(\\d+)/file.txt");

    int result = 0;
    EXPECT_TRUE(servlet.match("/foo/bar/123/file.txt", &result));
    EXPECT_EQ(123, result);
}
TEST(HttpServlet, ParsePathWithTwoElement) {
    HttpServerImpl servlet("/(foo|bar)/(\\d+)/file.txt");

    std::string path;
    int result = 0; //new char[1024];
    EXPECT_TRUE(servlet.match("/bar/123/file.txt", &path, &result));

    EXPECT_EQ("bar", path );
    EXPECT_EQ(123, result );
}
