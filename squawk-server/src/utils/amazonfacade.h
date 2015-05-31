#ifndef AMAZONFACADE_H
#define AMAZONFACADE_H

#include <map>
#include <iostream>
#include <string>

#include <stdlib.h>
#include <time.h>

// namespace amazon {
class AmazonFacade {
public:
    AmazonFacade();
    void bookByIsbn( const std::string & access_key, const std::string & key, const std::string & isbn );
    std::string generate_hmac256bit_hash(const char *message, const char *key_buf);
    std::string canonicalize( const std::map<std::string, std::string> & sortedParamMap);
    char * base64(const unsigned char *input, int length);
    std::string get_utc_time_string();
};
// } // amazon
#endif // AMAZONFACADE_H
