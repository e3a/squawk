#ifndef AMAZONFACADE_H
#define AMAZONFACADE_H

#include <map>
#include <iostream>
#include <string>

#include <stdlib.h>
#include <time.h>

#include "gtest/gtest_prod.h"

/**
 * @bried amazon aws utils.
 */
namespace amazon {
/**
 * @brief The BookResult struct
 */
struct BookResult {
public:
    std::string title() const { return title_; }
    void title ( const std::string & title ) { title_ = title; }
    std::string description() const { return description_; }
    void description ( const std::string & description ) { description_ = description; }
    std::string isbn() const { return isbn_; }
    void isbn ( const std::string & isbn ) { isbn_ = isbn; }
    std::string coverUri() const { return coverUri_; }
    void coverUri ( const std::string & coverUri ) { coverUri_ = coverUri; }
    std::string author() const { return author_; }
    void author ( const std::string & author ) { author_ = author; }
    std::string publicationDate() const { return publicationDate_; }
    void publicationDate ( const std::string & publicationDate ) { publicationDate_ = publicationDate; }
    std::string publisher() const { return publisher_; }
    void publisher ( const std::string & publisher ) { publisher_ = publisher; }
private:
    std::string title_, description_, isbn_, coverUri_, author_, publicationDate_, publisher_;
};

/**
 * @brief The AmazonFacade class
 */
class AmazonFacade {
public:
    AmazonFacade() { }

    /**
     * @brief bookByIsbn
     * @param access_key
     * @param key
     * @param isbn
     */
    static BookResult bookByIsbn ( const std::string & access_key, const std::string & key, const std::string & isbn );
    /**
     * @brief parse the response xml
     * @param response
     * @return
     */
    static BookResult parse ( const std::string & response );

private:
    FRIEND_TEST ( TestAmazonFacade, TestCanonicalize );
    FRIEND_TEST ( TestAmazonFacade, TestMac );
    FRIEND_TEST ( TestAmazonFacade, TestParseResponse );

    /**
     * Canonicalize the query string as required by Amazon.
     *
     * @param sortedParamMap    Parameter name-value pairs in lexicographical order.
     * @return                  Canonical form of query string.
     */
    static std::string canonicalize ( const std::map<std::string, std::string> & sortedParamMap );
    /**
     * @brief get_utc_time_string
     * @return
     */
    static std::string get_utc_time_string();
    /**
     * @brief generate_hmac256bit_hash
     * @param message
     * @param key_buf
     * @return
     */
    static std::string generate_hmac256bit_hash ( const char *message, const char *key_buf );
    /**
     * @brief base64
     * @param input
     * @param length
     * @return
     */
    static char * base64 ( const unsigned char *input, int length );
};
}//namespace amazon
#endif // AMAZONFACADE_H
