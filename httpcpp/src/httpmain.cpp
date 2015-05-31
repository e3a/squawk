
#include <iostream>
#include <thread>

#include "http.h"

int main ( int, char** ) {
    // http::HttpClient client ( "localhost", 80, "/" );
    http::HttpClient client ( "www.bing.com", 80, "/" );

	http::HttpRequest request ( "/" );
    request.parameter ( "Host", "localhost" );

    // request << "the body of the glglgl\nalkfjlakdjf\nlakdjflkfj";

	client.invoke ( request, [] ( http::HttpResponse & response ) {

        std::cout << "---------------------------------------------------------------------------------" << std::endl;
        std::cout << std::endl << response << std::endl;
		std::cout << response.body() << std::endl;
	} );

	std::cout << "wait for character" << std::endl;

	std::this_thread::sleep_for ( std::chrono::seconds ( 60 ) );
	std::cout << "EXIT" << std::endl;
}
