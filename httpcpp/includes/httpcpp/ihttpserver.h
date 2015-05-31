/*
    HttpServer Interface.
    Copyright (C) 2014  <e.knecht@netwings.ch>

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
#ifndef IHTTPSERVER
#define IHTTPSERVER

namespace http {

/**
 * @brief The HttpServer Interface.
 */
class IHttpServer {
public:
        IHttpServer () {}
        virtual ~IHttpServer() {}

        /**
         * @brief Start the server.
         */
        virtual void start() = 0;
        /**
         * @brief Stop the server.
         */
        virtual void stop() = 0;
};
} //http
#endif // IHTTPSERVER
