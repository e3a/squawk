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

#include "squawkserver.h"

#include <string>
#include <iostream>
#include <thread>
#include <exception>
#include <signal.h>

#include "fmt/format.h"
#include "fmt/time.h"

#include "fileservlet.h"

#include "ssdp.h"

#include "upnpcontentdirectory.h"
#include "upnpcontentdirectoryapi.h"
#include "upnpcontentdirectorydao.h"
#include "upnpcontentdirectoryfile.h"
#include "upnpcontentdirectoryimage.h"
#include "upnpcontentdirectorymusic.h"
#include "upnpcontentdirectoryparser.h"
#include "upnpcontentdirectoryvideo.h"
#include "upnpmediaservlet.h"
#include "upnpxmldescription.h"
#include "upnpcontentdirectorydao.h"
#include "upnpcontentdirectoryparser.h"
#include "upnpconnectionmanager.h"

namespace squawk {

void SquawkServer::start( squawk::SquawkConfig * squawk_config ) {

    _squawk_config = std::shared_ptr< squawk::SquawkConfig >(squawk_config);

    /** Create the Server **/
    _upnp_cds_dao = std::shared_ptr< squawk::UpnpContentDirectoryDao >( new squawk::UpnpContentDirectoryDao() );
    _upnp_file_parser = std::shared_ptr< squawk::UpnpContentDirectoryParser >( new squawk::UpnpContentDirectoryParser() );


    /** Register the CDS Methods **/
    squawk::UpnpContentDirectory * content_directory = new squawk::UpnpContentDirectory( "/ctl/ContentDir" );
    content_directory->registerContentDirectoryModule( std::unique_ptr< squawk::ContentDirectoryModule >( new squawk::UpnpContentDirectoryMusic() ) );
    content_directory->registerContentDirectoryModule( std::unique_ptr< squawk::ContentDirectoryModule >( new squawk::UpnpContentDirectoryVideo() ) );
    content_directory->registerContentDirectoryModule( std::unique_ptr< squawk::ContentDirectoryModule >( new squawk::UpnpContentDirectoryImage() ) );
    content_directory->registerContentDirectoryModule( std::unique_ptr< squawk::ContentDirectoryModule >( new squawk::UpnpContentDirectoryFile() ) );

    /** Setup and start the HTTP Server **/
    web_server = std::shared_ptr< http::WebServer >( new http::WebServer(
        squawk_config->httpAddress(),
        squawk_config->httpPort()
    ));

    web_server->register_servlet( std::unique_ptr< http::HttpServlet >( content_directory ) );
    web_server->register_servlet( std::unique_ptr< http::HttpServlet >( new squawk::UpnpConnectionManager( "/ctl/ConnectionMgr" ) ) );
    web_server->register_servlet( std::unique_ptr< http::HttpServlet >( new squawk::UpnpXmlDescription( "/rootDesc.xml" ) ) );
    web_server->register_servlet( std::unique_ptr< http::HttpServlet >(
        new squawk::UpnpContentDirectoryApi( "/api/(upnp/device|upnp/event|album|artist|track|browse|statistic)/?(\\d*)?") ) );
    web_server->register_servlet( std::unique_ptr< http::HttpServlet >(
        new squawk::UpnpMediaServlet( "/(video|audio|image|cover|albumArtUri|resource)/(\\d*).(flac|mp3|avi|mp4|mkv|mpeg|mov|wmv|jpg)" ) ) );
    web_server->register_servlet( std::unique_ptr< http::HttpServlet >(
        new http::servlet::FileServlet( "/bower_components/.*", squawk_config->bowerRoot() ) ) );
    web_server->register_servlet( std::unique_ptr< http::HttpServlet >(
        new http::servlet::FileServlet( "/.*", squawk_config->docRoot() ) ) );

    web_server->start();


    /** Setup and start the SSDP Server **/
    std::string device_uri_ = fmt::format( "http://{}:{}/rootDesc.xml", squawk_config->httpAddress(), squawk_config->httpPort() );
    _ssdp_server = std::shared_ptr< ssdp::SSDPServerImpl >( new ssdp::SSDPServerImpl(
        squawk_config->uuid(),
        squawk_config->multicastAddress(),
        squawk_config->multicastPort(),
        std::map< std::string, std::string >( {
            { squawk_config->uuid(), device_uri_ },
            { ssdp::NS_ROOT_DEVICE, device_uri_ },
            { ssdp::NS_MEDIASERVER, device_uri_ },
            { ssdp::NS_CONTENT_DIRECTORY, device_uri_ }
        } )
    ) );

    //add the event listener
    using namespace std::placeholders;
    _ssdp_server->subscribe(std::bind( &SquawkServer::ssdp_event, this, _1, _2, _3 ) );

    //clean the devices
    _ssdp_devices_thread = std::unique_ptr<std::thread> ( new std::thread( &SquawkServer::cleanup_upnp_devices, this ) );
    _ssdp_server->start();


    /** Rescan the Media Directory **/
    if(squawk_config->rescan) {
        import_media_directory();
    }

    _ssdp_server->announce();
    _ssdp_server->search();
}
void SquawkServer::stop() {
    _ssdp_devices_thread_run = false;
    _ssdp_devices_thread->join();
    _ssdp_server->stop();
     web_server->stop();
}

bool SquawkServer::import_media_directory() {
    if ( ! _upnp_file_parser_thread.joinable() ) {
        _upnp_file_parser_thread = std::thread( &UpnpContentDirectoryParser::parse, _upnp_file_parser, _squawk_config->mediaDirectories() );
        return true;
    } else return false;
}

void SquawkServer::cleanup_upnp_devices() {
    while ( _ssdp_devices_thread_run ) { {
            std::lock_guard<std::mutex> _ssdp_devices_guard( _ssdp_devices_mutex );

            auto iter_ = _ssdp_devices.begin();
            auto end_iter_ = _ssdp_devices.end();
            while( iter_ != end_iter_ ) {
                if ( iter_->second.timeout() ) {
                    CLOG(INFO, "upnp") << "(Device Timeout) :" << iter_->second.friendlyName();
                    _ssdp_devices.erase( iter_++ );
                } else {
                    ++iter_;
                }
            }//while iter ssdp devices
        }//context for the mutex
        std::this_thread::sleep_for ( std::chrono::milliseconds ( 5000 ) );
    }//while forever
}

void SquawkServer::ssdp_event( ssdp::SSDPEventListener::EVENT_TYPE type, std::string, ssdp::SsdpEvent device ) {

    //nt            device type
    //nts           event type
    //usn           device endpoint uuid
    //cache_control cache timer
    std::string _rootdevice_usn;
    size_t uuid_position_end_ = device.usn().find( "::" );
    if( uuid_position_end_ != std::string::npos ) {
        _rootdevice_usn = device.usn().substr(0, uuid_position_end_ );
    } else if( device.nt() == ssdp::NS_ROOT_DEVICE ) {
        _rootdevice_usn = device.usn();
    } else if( device.nt() == device.usn() ) {
        _rootdevice_usn = device.usn();
    } else if( squawk::SUAWK_SERVER_DEBUG ) {
        CLOG(ERROR, "upnp") << "can not parse usn: " << device.nt() << "=" << device.usn();
    }

    if( device.nt() == ssdp::NS_ROOT_DEVICE ) {
        if( type == ssdp::SSDPEventListener::BYE ) {
            if( _ssdp_devices.find( _rootdevice_usn ) != _ssdp_devices.end() ) {
                CLOG(INFO, "upnp") << "(remove rootdevice) : " << _ssdp_devices[ _rootdevice_usn ].friendlyName();
                std::lock_guard<std::mutex> _ssdp_devices_guard( _ssdp_devices_mutex );
                _ssdp_devices.erase( _ssdp_devices.find( _rootdevice_usn ) );
            }
        } else if( _ssdp_devices.find( _rootdevice_usn ) != _ssdp_devices.end() ) {
            //update timestamp
            if( squawk::SUAWK_SERVER_DEBUG ) {
                CLOG(DEBUG, "upnp") << "(existing rootdevice) : " << _ssdp_devices[ _rootdevice_usn ].friendlyName();
            }
            _ssdp_devices[ _rootdevice_usn ].touch();

        } else {
            //create new rootdevice
            try {
                upnp::UpnpDevice device_ = upnp::deviceDescription( device );
                device_.touch();
                device_.timeout( device.cacheControl() );

                CLOG(INFO, "upnp") << "(new rootdevice) : " << device_.friendlyName();

                std::lock_guard<std::mutex> _ssdp_devices_guard( _ssdp_devices_mutex );
                _ssdp_devices[ _rootdevice_usn ] = device_;

            } catch( commons::xml::XmlException & ex ) {
                CLOG(ERROR, "upnp") << "XML Parse Exception (" << ex.code() << ") " << ex.what();
            } catch( ... ) {
                CLOG(ERROR, "upnp") << device;
            }
        }//fi listener type
    }//fi root device
}//lambda event
}//namespace squawk
