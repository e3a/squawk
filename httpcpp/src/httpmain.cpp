
#include <iostream>

#include "http.h"

int main ( int ac, const char* av[] ) {
	http::HttpClient client ( "localhost", 80, "/" );
//    http::HttpClient client("www.bing.com", 80, "/");

	http::HttpRequest request ( "/" );
	request << "<the body>";

	client.invoke ( request );

	getchar();
	client.invoke ( request );
	getchar();

}
