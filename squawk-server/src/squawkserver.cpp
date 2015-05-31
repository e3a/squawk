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

#include <squawk.h>
#include <upnp.h>

#include <string>
#include <iostream>
#include <thread>
#include <exception>
#include <signal.h>

#include "http.h"
#include "fileservlet.h"


#include "api/apialbumlistservlet.h"
#include "api/apiartistlistservlet.h"
#include "api/apialbumitemservlet.h"
#include "servlet/apistatisticservlet.h"
// TODO #include "api/apialbumsletterservlet.h"
#include "api/apivideolistservlet.h"
#include "api/mediaservlet.h"
#include "api/coverservlet.h"
#include "api/apibrowseservlet.h"
#include "servlet/imageservlet.h"
#include "servlet/songservlet.h"

#include "upnp/upnpxmldescription.h"
#include "upnp/upnpcontentdirectory.h"
#include "upnp/upnpmusicdirectorymodule.h"
#include "upnp/upnpvideodirectory.h"
#include "upnp/upnpimagedirectory.h"
#include "upnp/upnpconnectionmanager.h"
#include "upnp/upnpmediaservlet.h"

// #include "http/api/apiupnpdevicehandler.h"

#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"

void SquawkServer::start() {
    ssdp_event_logger = 
      std::unique_ptr<squawk::LoggerEventListener>( new squawk::LoggerEventListener() );
  
    /*
    database = new squawk::db::Sqlite3Database();
    database->open( squawk_config->databaseFile() );
    */

    http::HttpServletContext context( squawk_config->getMap() );
    parser = new squawk::media::FileParser(squawk_config->databaseFile(), squawk_config->tmpDirectory());


    //Setup and start the DLNA server
    commons::upnp::ContentDirectoryModule * musicDirectory = new squawk::upnp::UpnpMusicDirectoryModule( context );
    commons::upnp::ContentDirectoryModule * videoDirectory = new squawk::upnp::UpnpVideoDirectory( context );
    commons::upnp::ContentDirectoryModule * imageDirectory = new squawk::upnp::UpnpImageDirectory( context );
    squawk::upnp::UpnpContentDirectory * content_directory = new squawk::upnp::UpnpContentDirectory("/ctl/ContentDir", context );
    content_directory->registerContentDirectoryModule(musicDirectory);
    content_directory->registerContentDirectoryModule(videoDirectory);
    content_directory->registerContentDirectoryModule(imageDirectory);

    squawk::upnp::UpnpConnectionManager * connection_manager = new squawk::upnp::UpnpConnectionManager("/ctl/ConnectionMgr");


    //Setup and start the HTTP server
    web_server = new http::WebServer(
                squawk_config->httpAddress(),
                squawk_config->httpPort() /*,
                squawk_config->int_value(CONFIG_HTTP_THREADS) */ );

    squawk::upnp::UpnpXmlDescription * xmldescription = new squawk::upnp::UpnpXmlDescription(std::string("/rootDesc.xml"), context );
    squawk::servlet::ApiStatisticServlet * statistic_servlet = new squawk::servlet::ApiStatisticServlet(std::string("/api/statistic"), database);
    squawk::api::ApiAlbumListServlet * albums_servlet = new squawk::api::ApiAlbumListServlet(std::string("/api/album"), context );
    squawk::api::ApiVideoListServlet * videos_servlet = new squawk::api::ApiVideoListServlet(std::string("/api/video"), context );
//    squawk::servlet::ApiAlbumsByArtist * albumsbyartist_servlet = new squawk::servlet::ApiAlbumsByArtist(std::string("/api/artist/(\\d+)/album"), database);
    squawk::api::ApiArtistListServlet * artists_servlet = new squawk::api::ApiArtistListServlet(std::string("/api/artist"), context );
    squawk::api::ApiAlbumItemServlet * album_servlet = new squawk::api::ApiAlbumItemServlet(std::string("/api/album/(\\d*)"), context );
    // TODO squawk::api::ApiAlbumsLetterServlet * letter_servlet = new squawk::api::ApiAlbumsLetterServlet(std::string("/api/album/letter"), context );

    squawk::api::CoverServlet * cover_servlet = new squawk::api::CoverServlet("/api/album/(\\d*)/cover.jpg", context );
    squawk::servlet::ImageServlet * image_servlet = new squawk::servlet::ImageServlet("/album/image/(\\d*).jpg", squawk_config->tmpDirectory());
    squawk::servlet::SongServlet * song_servlet = new squawk::servlet::SongServlet("/song/(\\d*).(flac|mp3)", database);

    squawk::api::MediaServlet * media_servlet = new squawk::api::MediaServlet("/file/(video|audio|image|cover)/(\\d*).(flac|mp3|avi|mp4|mkv|jpg)", context );
    squawk::api::ApiBrowseServlet * browse_servlet = new squawk::api::ApiBrowseServlet("/(video|image|book)/?([0-9]+)?", context );

    squawk::upnp::UpnpMediaServlet * upnp_media_servlet = new squawk::upnp::UpnpMediaServlet("/(video|audio|image)/(\\d*).(flac|mp3|avi|mp4|mkv)", context );

    http::servlet::FileServlet * fileServlet = new http::servlet::FileServlet(std::string("/.*"), squawk_config->docRoot());

    web_server->register_servlet(content_directory);
    web_server->register_servlet(connection_manager);
    web_server->register_servlet(xmldescription);
    // TODO web_server->register_servlet(letter_servlet);
    web_server->register_servlet(album_servlet);
    web_server->register_servlet(albums_servlet);
    web_server->register_servlet(videos_servlet);
//    web_server->register_servlet(albumsbyartist_servlet);
    web_server->register_servlet(artists_servlet);
    web_server->register_servlet(cover_servlet);
    web_server->register_servlet(image_servlet);
    web_server->register_servlet(song_servlet);
    web_server->register_servlet(browse_servlet);
    web_server->register_servlet(statistic_servlet);
    web_server->register_servlet(media_servlet);
    web_server->register_servlet(upnp_media_servlet);
    web_server->register_servlet(fileServlet);
    web_server->start();


/*    squawk::http::RequestCallback * api_albums_handler = new api::ApiAlbumsHandler(service);
    squawk::http::RequestCallback * api_artist_handler = new api::ApiArtistHandler(service);
    squawk::http::RequestCallback * api_album_handler = new api::ApiAlbumHandler(service);
    squawk::http::RequestCallback * api_album_by_artist_handler = new api::ApiAlbumsByArtist(service);
    squawk::http::RequestCallback * cover_handler = new squawk::http::files::CoverHandler(squawk_config);
    squawk::http::RequestCallback * image_handler = new squawk::http::files::ImageHandler(dao, squawk_config);
    squawk::http::RequestCallback * song_handler = new squawk::http::files::SongHandler(dao, squawk_config);
    squawk::http::RequestCallback * xmldescriptions = new squawk::http::upnp::XmlDescriptions(squawk_config);
*/
    /* / run server in background thread.
    http_server = new squawk::http::Server(
      squawk_config->string_value(CONFIG_HTTP_IP),
      squawk_config->string_value(CONFIG_HTTP_PORT),
      squawk_config->string_value(CONFIG_HTTP_DOCROOT),
      squawk_config->int_value(CONFIG_HTTP_THREADS));

    http_server->register_handler("GET", "/api/album", api_albums_handler);
    http_server->register_handler("GET", "/api/artist", api_artist_handler);
    http_server->register_handler("GET", "/api/album/(\\d*)", api_album_handler);
    http_server->register_handler("GET", "/api/artist/(\\d*)/album", api_album_by_artist_handler);
    http_server->register_handler("POST", "/ctl/ContentDir", content_directory);
    http_server->register_handler("GET", "/rootDesc.xml", xmldescriptions);
    http_server->register_handler("GET", "/album/image/(\\d*).jpg", image_handler);
    http_server->register_handler("GET", "/album/(\\d*)/cover.jpg", cover_handler);
    http_server->register_handler("GET", "/song/(\\d*).(flac|mp3)", song_handler);
*/
    //Setup and start the SSDP server
    ssdp_server = new ssdp::SSDPServerImpl(
    squawk_config->uuid(),
    squawk_config->multicastAddress(),
    squawk_config->multicastPort() );

    //register namespaces
    ssdp_server->register_namespace(squawk_config->uuid(), std::string("http://") + squawk_config->httpAddress() + ":" + commons::string::to_string( squawk_config->httpPort() ) + "/rootDesc.xml");
    ssdp_server->register_namespace(ssdp::NS_ROOT_DEVICE, std::string("http://") + squawk_config->httpAddress() + ":" + commons::string::to_string( squawk_config->httpPort() ) + "/rootDesc.xml");
    ssdp_server->register_namespace(ssdp::NS_MEDIASERVER, std::string("http://") + squawk_config->httpAddress() + ":" + commons::string::to_string( squawk_config->httpPort() ) + "/rootDesc.xml");
    ssdp_server->register_namespace(ssdp::NS_CONTENT_DIRECTORY, std::string("http://") + squawk_config->httpAddress() + ":" + commons::string::to_string( squawk_config->httpPort() ) + "/rootDesc.xml");
    ssdp_server->register_namespace(ssdp::NS_MEDIA_RECEIVER_REGISTRAR, std::string("http://") + squawk_config->httpAddress() + ":" + commons::string::to_string( squawk_config->httpPort() ) + "/rootDesc.xml");

    ssdp_server->subscribe( ssdp_event_logger.get() );
    
    //TODO squawk::http::RequestCallback * api_upnp_device_handler = new api::ApiUpnpDeviceHandler(service, ssdp_server);
    //TODO http_server->register_handler("GET", "/api/devices", api_upnp_device_handler);

    //start the server
//TODO    http_thread = std::thread(&squawk::http::Server::run, http_server);
    ssdp_server->start();

    //rescan the media directory
    if(squawk_config->rescan) {
        parser->parse( squawk_config->mediaDirectories() );
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000)); //wait for the server to start
    }

    std::cout << "server started and library rescanned" << std::endl;

    ssdp_server->announce();
    ssdp_server->search();
}
void SquawkServer::stop() {
    std::cout << "stop ssdp_server" << std::endl;
    ssdp_server->stop();
    delete ssdp_server;
  
    std::cout << "stop web_server" << std::endl;
    web_server->stop();
    delete web_server;

    std::cout << "delete parser" << std::endl;
    delete parser;
}

int main(int ac, const char* av[]) {

    try {
    squawk::SquawkConfig * squawk_config = new squawk::SquawkConfig();
    if(! squawk_config->parse(ac, av)) {
        exit(1);
    }
    squawk_config->load(squawk_config->configFile());
    if(! squawk_config->validate()) {
        exit(1);
    }
    squawk_config->save(squawk_config->configFile());

    //load the logger
    if ( squawk_config->logger() != "" ) {
        log4cxx::PropertyConfigurator::configure( squawk_config->logger() );
    } else {
       log4cxx::BasicConfigurator::configure();
    }
     
    // Block all signals for background thread.
    sigset_t new_mask;
    sigfillset(&new_mask);
    sigset_t old_mask;
    pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask);

    SquawkServer * server = new SquawkServer(squawk_config);
    server->start();

    // Restore previous signals.
    pthread_sigmask(SIG_SETMASK, &old_mask, 0);

    // Wait for signal indicating time to shut down.
    sigset_t wait_mask;
    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGINT);
    sigaddset(&wait_mask, SIGQUIT);
    sigaddset(&wait_mask, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &wait_mask, 0);
    int sig = 0;

    std::cout << "wait for signal" << std::endl;

    sigwait(&wait_mask, &sig);

    std::cout << "shutdown server" << std::endl;

    server->stop();
    delete squawk_config;
    delete server;
    return 0;

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    } catch (int & e) {
        std::cerr << "int:" << e << std::endl;
    } catch (char * e) {
      std::cerr << "char*:" << e << std::endl;
    } catch (std::string * e) {
      std::cerr << "std:" << e << std::endl;
  } catch (...) {
        try {
             std::exception_ptr eptr = std::current_exception();
            if (eptr != std::exception_ptr()) {
                std::rethrow_exception(eptr);
            }
        } catch(const std::exception& e) {
            std::cout << "Caught exception \"" << e.what() << "\"\n";
        }
    }
  return 1;
}

