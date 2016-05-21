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
#include "xml.h"

namespace amazon {

BookResult AmazonFacade::bookByIsbn( const std::string & access_key, const std::string & key, const std::string & isbn ) {

    std::map< std::string, std::string > map;
    map["AssociateTag"] = "AcmeBooks";
    map["Keywords"] = isbn;
    map["Operation"] = "ItemSearch";
    map["ResponseGroup"] = "Large";
    map["SearchIndex"] = "Books";
    map["Service"] = "AWSECommerceService";
    map["Timestamp"] = get_utc_time_string();
    map["Version"] = "2009-03-31";
    map["AWSAccessKeyId"] = access_key;

    std::string query = canonicalize( map );

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
        curlpp::Cleanup myCleanup;
        curlpp::Easy myRequest;
        myRequest.setOpt<curlpp::options::Url>( buf.str() );

        std::stringstream sstream;
        curlpp::options::WriteStream ws(&sstream);
        myRequest.setOpt(ws);

        myRequest.perform();

        return parse( sstream.str( ) );

    } catch(curlpp::RuntimeError & e) {
        std::cerr << e.what() << std::endl;
        throw;
    } catch(curlpp::LogicError & e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}
std::string AmazonFacade::generate_hmac256bit_hash(const char *message, const char *key_buf) {
    unsigned char* result;
    unsigned int result_len = 32;
    HMAC_CTX ctx;

    result = (unsigned char*) malloc(sizeof(char) * result_len);

    ENGINE_load_builtin_engines();
    ENGINE_register_all_complete();

    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, key_buf, strlen( key_buf )/*16*/, EVP_sha256(), NULL);
    HMAC_Update(&ctx, (unsigned char *)message, strlen( message ) );
    HMAC_Final(&ctx, result, &result_len);
    HMAC_CTX_cleanup(&ctx);

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
BookResult AmazonFacade::parse( const std::string & response ) {

    BookResult result;
    commons::xml::XMLReader reader( response );
    std::vector< commons::xml::Node > images = reader.getElementsByName( "LargeImage" );
    for( auto node : images[0].children() ) {
        if(node.name() == "URL" ) {
            result.coverUri( node.content() );
        }
    }

    std::vector< commons::xml::Node > attributes =  reader.getElementsByName( "ItemAttributes" );
    for( auto node : attributes[0].children() ) {
        if(node.name() == "Author" && result.author() == "" ) {
            result.author( node.content() );
        } else if(node.name() == "Publisher" ) {
            result.publisher( node.content() );
        } else if(node.name() == "PublicationDate" ) {
            result.publicationDate( node.content() );
        } else if(node.name() == "Title" ) {
            result.title( node.content() );
        }
    }

    std::vector< commons::xml::Node > reviews =  reader.getElementsByName( "EditorialReview" );
    for( auto node : reviews[0].children() ) {
        if(node.name() == "Content" && result.description() == "" ) {
            result.description( node.content() );
        }

    }
    return result;
}
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
} // amazon
