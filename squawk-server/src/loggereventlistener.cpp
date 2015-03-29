/*
    Logger Event Listener

    Copyright (C) 2015  <etienne> <e.knecht@netwings.ch>

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

#include "loggereventlistener.h"

namespace squawk {

log4cxx::LoggerPtr LoggerEventListener::logger(log4cxx::Logger::getLogger("squawk.LoggerEventListener"));
  
void LoggerEventListener::ssdpEvent( ssdp::SSDPEventListener::EVENT_TYPE type, std::string  client_ip, ssdp::UpnpDevice device ) {
  LOG4CXX_INFO(logger, "SSDP:" << (type==ssdp::SSDPEventListener::ANNOUNCE ? "ANNOUNCE:" : "BYE:") << client_ip << ":" << device.usn );
}
}
