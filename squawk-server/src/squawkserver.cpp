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

#include <string>
#include <iostream>
#include <thread>
#include <exception>

#include "http.h"
#include "fileservlet.h"


#include "servlet/apialbumsservlet.h"
#include "servlet/apiartistservlet.h"
#include "servlet/apialbumservlet.h"
#include "servlet/apistatisticservlet.h"
#include "servlet/apialbumsbyartist.h"
#include "servlet/coverservlet.h"
#include "servlet/imageservlet.h"
#include "servlet/songservlet.h"
#include "servlet/upnpxmldescription.h"
#include "servlet/upnpcontentdirectory.h"
#include "servlet/upnpmusicdirectorymodule.h"

// #include "http/api/apiupnpdevicehandler.h"


#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"

void SquawkServer::start() {
    database = new squawk::db::Sqlite3Database();
    database->open(squawk_config->string_value(CONFIG_DATABASE_FILE));

    dao = new squawk::db::SquawkDAO(squawk_config);
    service = new squawk::SquawkServiceImpl(dao, squawk_config);

    parser = new squawk::media::FileParser(database, squawk_config);

    //Setup and start the DLNA server
    ContentDirectoryModule * musicDirectory = new squawk::servlet::UpnpMusicDirectoryModule(squawk_config, database);
    squawk::servlet::UpnpContentDirectory * content_directory = new squawk::servlet::UpnpContentDirectory("/ctl/ContentDir");
    content_directory->registerContentDirectoryModule(musicDirectory);

    //Setup and start the HTTP server
    web_server = new http::WebServer(
                squawk_config->string_value(CONFIG_HTTP_IP),
                squawk_config->string_value(CONFIG_HTTP_PORT),
                squawk_config->int_value(CONFIG_HTTP_THREADS));

    squawk::servlet::UpnpXmlDescription * xmldescription = new squawk::servlet::UpnpXmlDescription(std::string("/rootDesc.xml"), squawk_config );
    squawk::servlet::ApiStatisticServlet * statistic_servlet = new squawk::servlet::ApiStatisticServlet(std::string("/api/statistic"), database);
    squawk::servlet::ApiAlbumsServlet * albums_servlet = new squawk::servlet::ApiAlbumsServlet(std::string("/api/album"), database);
    squawk::servlet::ApiAlbumsByArtist * albumsbyartist_servlet = new squawk::servlet::ApiAlbumsByArtist(std::string("/api/artist/(\\d+)/album"), database);
    squawk::servlet::ApiArtistServlet * artists_servlet = new squawk::servlet::ApiArtistServlet(std::string("/api/artist"), database);
    squawk::servlet::ApiAlbumServlet * album_servlet = new squawk::servlet::ApiAlbumServlet(std::string("/api/album/(\\d*)"), database);

    squawk::servlet::CoverServlet * cover_servlet = new squawk::servlet::CoverServlet("/album/(\\d*)/cover.jpg", squawk_config->string_value(CONFIG_TMP_DIRECTORY));
    squawk::servlet::ImageServlet * image_servlet = new squawk::servlet::ImageServlet("/album/image/(\\d*).jpg", squawk_config->string_value(CONFIG_TMP_DIRECTORY));
    squawk::servlet::SongServlet * song_servlet = new squawk::servlet::SongServlet("/song/(\\d*).(flac|mp3)", squawk_config->string_value(CONFIG_MEDIA_DIRECTORY), database);

    http::servlet::FileServlet * fileServlet = new http::servlet::FileServlet(std::string("/.*"), squawk_config->string_value(CONFIG_HTTP_DOCROOT));

    web_server->register_servlet(content_directory);
    web_server->register_servlet(xmldescription);
    web_server->register_servlet(album_servlet);
    web_server->register_servlet(albums_servlet);
    web_server->register_servlet(albumsbyartist_servlet);
    web_server->register_servlet(artists_servlet);
    web_server->register_servlet(cover_servlet);
    web_server->register_servlet(image_servlet);
    web_server->register_servlet(song_servlet);
    web_server->register_servlet(statistic_servlet);
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
    ssdp_server = new squawk::ssdp::SSDPServerImpl(
    squawk_config->string_value(CONFIG_UUID),
    squawk_config->string_value(CONFIG_LOCAL_LISTEN_ADDRESS),
    squawk_config->string_value(CONFIG_MULTICAST_ADDRESS),
    squawk_config->int_value(CONFIG_MULTICAST_PORT));

    //register namespaces
    ssdp_server->register_namespace(NS_ROOT_DEVICE, std::string("http://") + squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) + "/rootDesc.xml");
    ssdp_server->register_namespace(NS_MEDIASERVER, std::string("http://") + squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) + "/rootDesc.xml");
    ssdp_server->register_namespace(NS_CONTENT_DIRECTORY, std::string("http://") + squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) + "/rootDesc.xml");
    ssdp_server->register_namespace(NS_MEDIA_RECEIVER_REGISTRAR, std::string("http://") + squawk_config->string_value(CONFIG_HTTP_IP) + ":" + squawk_config->string_value(CONFIG_HTTP_PORT) + "/rootDesc.xml");

    //TODO squawk::http::RequestCallback * api_upnp_device_handler = new api::ApiUpnpDeviceHandler(service, ssdp_server);
    //TODO http_server->register_handler("GET", "/api/devices", api_upnp_device_handler);

    //start the server
//TODO    http_thread = std::thread(&squawk::http::Server::run, http_server);
    ssdp_thread = std::thread(&squawk::ssdp::SSDPServerImpl::start, ssdp_server);

    //rescan the media directory
    if(squawk_config->rescan) {
     parser->parse(squawk_config->string_value(CONFIG_MEDIA_DIRECTORY));
    } else {
        sleep( 5 ); //wait for the server to start
    }

    std::cout << "server started and library rescanned" << std::endl;

    ssdp_server->announce();
}
void SquawkServer::stop() {
    delete dao, service, parser;
//TODO    http_server->stop();
    http_thread.join();
    ssdp_server->stop();
    ssdp_thread.join();
    delete web_server, ssdp_server;
}

int main(int ac, const char* av[]) {

    try {
    squawk::SquawkConfig * squawk_config = new squawk::SquawkConfig();
    if(! squawk_config->parse(ac, av)) {
        exit(1);
    }
    if(! squawk_config->load(squawk_config->string_value(CONFIG_FILE))) {
        exit(1);
    }
    if(! squawk_config->validate()) {
        exit(1);
    }
    squawk_config->save(squawk_config->string_value(CONFIG_FILE));

    //load the logger
    if (squawk_config->exist(CONFIG_LOGGER_PROPERTIES)) {
       log4cxx::PropertyConfigurator::configure(squawk_config->string_value(CONFIG_LOGGER_PROPERTIES));
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
