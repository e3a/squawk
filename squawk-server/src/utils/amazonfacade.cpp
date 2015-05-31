#include "amazonfacade.h"

#include <iostream>

#include <string>
#include <string.h>
#include <sstream>

#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include "utctimestamp.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "http.h"

// namespace amazon {

AmazonFacade::AmazonFacade() { }

void AmazonFacade::bookByIsbn( const std::string & access_key, const std::string & key, const std::string & isbn ) {

    // AWSAccessKeyId=AKIAIC5IGJNCXA3YR2FA&
    // AssociateTag=AcmeBooks&
    // Keywords=9780786751075&
    // Operation=ItemSearch&
    // ResponseGroup=Large&
    // SearchIndex=Books&
    // Service=AWSECommerceService&
    // Timestamp=2015-05-17T17%3A13%3A33Z&
    // Version=2009-03-31&
    // Signature=eDoZPJPDtYFWqzi6%2FjFVbQmI2jEqaHk%2FZy8F1moJBqQ%3D

    std::map< std::string, std::string > map;
    map["AssociateTag"] = "AcmeBooks";
    map["Keywords"] = isbn;
    map["Operation"] = "ItemSearch";
    map["ResponseGroup"] = "Large";
    map["SearchIndex"] = "Books";
    map["Service"] = "AWSECommerceService";
    map["Timestamp"] = get_utc_time_string(); //"2015-05-17T17%3A13%3A33Z";
    map["Version"] = "2009-03-31";
    map["AWSAccessKeyId"] = access_key;

    std::string query = canonicalize( map );

/*    bool first = true;
    for( auto iter : map ) {
        if(first ) { first = false; }
        else { query += "&"; }
        query += iter.first + "=" + iter.second;
    } */
    std::cout << query << std::endl;

    std::stringstream ss;
    ss << "GET\n" <<
          "ecs.amazonaws.com" << "\n" <<
          "/onca/xml" << "\n" <<
          query;

    std::stringstream buf;
    buf << "http://ecs.amazonaws.com/onca/xml?" << query << "&Signature=" <<
        http::utils::UrlEscape::urlEncode( generate_hmac256bit_hash( ss.str().c_str(), key.c_str() ).c_str() );

    //make the request
    try {
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy myRequest;

        // Set the URL.
        myRequest.setOpt<curlpp::options::Url>( buf.str() );

        // Send request and get a result.
        // By default the result goes to standard output.
        myRequest.perform();
    } catch(curlpp::RuntimeError & e) {
        std::cout << e.what() << std::endl;
    } catch(curlpp::LogicError & e) {
        std::cout << e.what() << std::endl;
    }
}
std::string AmazonFacade::generate_hmac256bit_hash(const char *message, const char *key_buf) {
    std::cout << "------------------" << message << "\n-----------------------";
    unsigned char* result;
    unsigned int result_len = 32;
    int i;
    HMAC_CTX ctx;

    result = (unsigned char*) malloc(sizeof(char) * result_len);

    ENGINE_load_builtin_engines();
    ENGINE_register_all_complete();

    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, key_buf, strlen( key_buf )/*16*/, EVP_sha256(), NULL);
    HMAC_Update(&ctx, (unsigned char *)message, strlen( message ) );
    HMAC_Final(&ctx, result, &result_len);
    HMAC_CTX_cleanup(&ctx);

/*    for (i=0; i!=result_len; i++) {
           if (expected[i]!=result[i]) {
                   printf("Got %02X instead of %02X at byte %d!\n", result[i], expected[i], i);
                   break;
           }
    }
    if (i==result_len) {
        printf("Test ok!\n");
    } */

    std::cout << "the key is:";
    for (i=0; i!=result_len; i++) {
        printf("%02x", result[i]);
    }
    std::cout << std::endl;
    return std::string( base64( result, result_len ) );
}

char * AmazonFacade::base64(const unsigned char *input, int length) {
   BIO *bmem, *b64;
   BUF_MEM *bptr;

  b64 = BIO_new(BIO_f_base64());
   bmem = BIO_new(BIO_s_mem());
   b64 = BIO_push(b64, bmem);
   BIO_write(b64, input, length);
   int res = BIO_flush(b64);
   if( res < 1 ) {
       std::cerr << "error calculating mac." << std::endl;
   }
   BIO_get_mem_ptr(b64, &bptr);

  char *buff = (char *)malloc(bptr->length);
   memcpy(buff, bptr->data, bptr->length-1);
   buff[bptr->length-1] = 0;

  BIO_free_all(b64);

  return buff;
}

/**
 * Canonicalize the query string as required by Amazon.
 *
 * @param sortedParamMap    Parameter name-value pairs in lexicographical order.
 * @return                  Canonical form of query string.
 */
std::string AmazonFacade::canonicalize( const std::map<std::string, std::string> & sortedParamMap ) {
    if ( sortedParamMap.empty() ) {
        return "";
    }

    std::stringstream ss;
    bool first = true;
    for( auto itr : sortedParamMap ) {
        if( first ) { first = false; }
        else { ss << "&"; }
        ss << http::utils::UrlEscape::urlEncode( itr.first.c_str() ) << "=" << http::utils::UrlEscape::urlEncode( itr.second.c_str() );
        // buffer.append(percentEncodeRfc3986(kvpair.getKey()));
        // buffer.append("=");
        // buffer.append(percentEncodeRfc3986(kvpair.getValue()));
    }

    return ss.str();
}
std::string AmazonFacade::get_utc_time_string() {
        char timestamp_str[20];
        time_t rawtime;
        struct tm *ptm;

        time(&rawtime);
        ptm = gmtime(&rawtime);
        sprintf(timestamp_str, "%d-%02d-%02dT%02d:%02d:%02dZ", 1900+ptm->tm_year, 1+ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        return std::string(timestamp_str);
}

// } // amazon
