/*
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

#include <string>
#include <map>

#include "../src/utils/amazonfacade.h"

#include <gtest/gtest.h>


TEST(TestAmazonFacade, TestCanonicalize) {
    std::cout << "test" << std::endl;
    std::map<std::string, std::string > map;
    map["Service"] = "AWSECommerceService";
    map["AWSAccessKeyId"] = "AKIAIOSFODNN7EXAMPLE";
    map["AssociateTag"] = "mytag-20";
    map["Operation"] = "ItemLookup";
    map["ItemId"] = "0679722769";
    map["ResponseGroup"] = "Images,ItemAttributes,Offers,Reviews";
    map["Version"] = "2013-08-01";
    map["Timestamp"] = "2014-08-18T12:00:00Z";

    AmazonFacade facade;
    EXPECT_EQ(facade.canonicalize( map ), std::string("AWSAccessKeyId=AKIAIOSFODNN7EXAMPLE&AssociateTag=mytag-20&ItemId=0679722769&Operation=ItemLookup&ResponseGroup=Images%2CItemAttributes%2COffers%2CReviews&Service=AWSECommerceService&Timestamp=2014-08-18T12%3A00%3A00Z&Version=2013-08-01" ) );

}

TEST(TestAmazonFacade, TestMac) {
    AmazonFacade facade;
    std::string message = "GET\n" \
            "webservices.amazon.com\n" \
            "/onca/xml\n" \
            "AWSAccessKeyId=AKIAIOSFODNN7EXAMPLE&AssociateTag=mytag-20&ItemId=0679722769&Operation=ItemLookup&ResponseGroup=Images%2CItemAttributes%2COffers%2CReviews&Service=AWSECommerceService&Timestamp=2014-08-18T12%3A00%3A00Z&Version=2013-08-01";
    std::string key = "1234567890";

    std::string result = facade.generate_hmac256bit_hash( message.c_str(), key.c_str() );
    std::cout << "mac:" << result << std::endl;
    EXPECT_EQ(result, std::string("j7bZM0LXZ9eXeZruTqWm2DIvDYVUU3wxPPpp+iXxzQc=" ) );
}

