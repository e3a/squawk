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

#include "../../src/utils/amazonfacade.h"

#include <gtest/gtest.h>

namespace amazon {

TEST(TestAmazonFacade, TestCanonicalize) {

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
    EXPECT_EQ(result, std::string("j7bZM0LXZ9eXeZruTqWm2DIvDYVUU3wxPPpp+iXxzQc=" ) );
}

TEST(TestAmazonFacade, TestParseResponse ) {
    std::string response =
    "<?xml version=\"1.0\" ?><ItemSearchResponse xmlns=\"http://webservices.amazon.com/AWSECommerceService/2011-08-01\"><OperationRequest><RequestId>60b53e81-7579-4ec9-b984-e21527c883eb</RequestId><Arguments><Argument Name=\"AWSAccessKeyId\" Value=\"AKIAIC5IGJNCXA3YR2FA\"></Argument><Argument Name=\"AssociateTag\" Value=\"AcmeBooks\"></Argument><Argument Name=\"Keywords\" Value=\"9780786751075\"></Argument><Argument Name=\"Operation\" Value=\"ItemSearch\"></Argument><Argument Name=\"ResponseGroup\" Value=\"Large\"></Argument><Argument Name=\"SearchIndex\" Value=\"Books\"></Argument><Argument Name=\"Service\" Value=\"AWSECommerceService\"></Argument><Argument Name=\"Timestamp\" Value=\"2015-06-14T11:27:46Z\"></Argument><Argument Name=\"Version\" Value=\"2009-03-31\"></Argument><Argument Name=\"Signature\" Value=\"sXjl7B9V/vNuKa1GjnarVqzMaKYBQ0uabMqCkGWTGwU=\"></Argument></Arguments><RequestProcessingTime>0.1605550000000000</RequestProcessingTime></OperationRequest><Items><Request><IsValid>True</IsValid><ItemSearchRequest><Keywords>9780786751075</Keywords><ResponseGroup>Large</ResponseGroup><SearchIndex>Books</SearchIndex></ItemSearchRequest></Request><TotalResults>1</TotalResults><TotalPages>1</TotalPages><MoreSearchResultsUrl>http://www.amazon.com/gp/redirect.html?linkCode=xm2&amp;SubscriptionId=AKIAIC5IGJNCXA3YR2FA&amp;location=http%3A%2F%2Fwww.amazon.com%2Fgp%2Fsearch%3Fkeywords%3D9780786751075%26url%3Dsearch-alias%253Dstripbooks&amp;tag=AcmeBooks&amp;creative=386001&amp;camp=2025</MoreSearchResultsUrl><Item><ASIN>B004WOPHN0</ASIN><DetailPageURL>http://www.amazon.com/Low-Slow-Master-Barbecue-Lessons-ebook/dp/B004WOPHN0%3FSubscriptionId%3DAKIAIC5IGJNCXA3YR2FA%26tag%3DAcmeBooks%26linkCode%3Dxm2%26camp%3D2025%26creative%3D165953%26creativeASIN%3DB004WOPHN0</DetailPageURL><ItemLinks><ItemLink><Description>Technical Details</Description><URL>http://www.amazon.com/Low-Slow-Master-Barbecue-Lessons-ebook/dp/tech-data/B004WOPHN0%3FSubscriptionId%3DAKIAIC5IGJNCXA3YR2FA%26tag%3DAcmeBooks%26linkCode%3Dxm2%26camp%3D2025%26creative%3D386001%26creativeASIN%3DB004WOPHN0</URL></ItemLink><ItemLink><Description>Add To Baby Registry</Description><URL>http://www.amazon.com/gp/registry/baby/add-item.html%3Fasin.0%3DB004WOPHN0%26SubscriptionId%3DAKIAIC5IGJNCXA3YR2FA%26tag%3DAcmeBooks%26linkCode%3Dxm2%26camp%3D2025%26creative%3D386001%26creativeASIN%3DB004WOPHN0</URL></ItemLink><ItemLink><Description>Add To Wedding Registry</Description><URL>http://www.amazon.com/gp/registry/wedding/add-item.html%3Fasin.0%3DB004WOPHN0%26SubscriptionId%3DAKIAIC5IGJNCXA3YR2FA%26tag%3DAcmeBooks%26linkCode%3Dxm2%26camp%3D2025%26creative%3D386001%26creativeASIN%3DB004WOPHN0</URL></ItemLink><ItemLink><Description>Add To Wishlist</Description><URL>http://www.amazon.com/gp/registry/wishlist/add-item.html%3Fasin.0%3DB004WOPHN0%26SubscriptionId%3DAKIAIC5IGJNCXA3YR2FA%26tag%3DAcmeBooks%26linkCode%3Dxm2%26camp%3D2025%26creative%3D386001%26creativeASIN%3DB004WOPHN0</URL></ItemLink><ItemLink><Description>Tell A Friend</Description><URL>http://www.amazon.com/gp/pdp/taf/B004WOPHN0%3FSubscriptionId%3DAKIAIC5IGJNCXA3YR2FA%26tag%3DAcmeBooks%26linkCode%3Dxm2%26camp%3D2025%26creative%3D386001%26creativeASIN%3DB004WOPHN0</URL></ItemLink><ItemLink><Description>All Customer Reviews</Description><URL>http://www.amazon.com/review/product/B004WOPHN0%3FSubscriptionId%3DAKIAIC5IGJNCXA3YR2FA%26tag%3DAcmeBooks%26linkCode%3Dxm2%26camp%3D2025%26creative%3D386001%26creativeASIN%3DB004WOPHN0</URL></ItemLink><ItemLink><Description>All Offers</Description><URL>http://www.amazon.com/gp/offer-listing/B004WOPHN0%3FSubscriptionId%3DAKIAIC5IGJNCXA3YR2FA%26tag%3DAcmeBooks%26linkCode%3Dxm2%26camp%3D2025%26creative%3D386001%26creativeASIN%3DB004WOPHN0</URL></ItemLink></ItemLinks><SalesRank>60602</SalesRank><SmallImage><URL>http://ecx.images-amazon.com/images/I/" \
    "51IUbMMktGL._SL75_.jpg</URL><Height Units=\"pixels\">75</Height><Width Units=\"pixels\">59</Width></SmallImage><MediumImage><URL>http://ecx.images-amazon.com/images/I/51IUbMMktGL._SL160_.jpg</URL><Height Units=\"pixels\">160</Height><Width Units=\"pixels\">126</Width></MediumImage><LargeImage><URL>http://ecx.images-amazon.com/images/I/51IUbMMktGL.jpg</URL><Height Units=\"pixels\">500</Height><Width Units=\"pixels\">394</Width></LargeImage><ImageSets><ImageSet Category=\"primary\"><SwatchImage><URL>http://ecx.images-amazon.com/images/I/51IUbMMktGL._SL30_.jpg</URL><Height Units=\"pixels\">30</Height><Width Units=\"pixels\">24</Width></SwatchImage><SmallImage><URL>http://ecx.images-amazon.com/images/I/51IUbMMktGL._SL75_.jpg</URL><Height Units=\"pixels\">75</Height><Width Units=\"pixels\">59</Width></SmallImage><ThumbnailImage><URL>http://ecx.images-amazon.com/images/I/51IUbMMktGL._SL75_.jpg</URL><Height Units=\"pixels\">75</Height><Width Units=\"pixels\">59</Width></ThumbnailImage><TinyImage><URL>http://ecx.images-amazon.com/images/I/51IUbMMktGL._SL110_.jpg</URL><Height Units=\"pixels\">110</Height><Width Units=\"pixels\">87</Width></TinyImage><MediumImage><URL>http://ecx.images-amazon.com/images/I/51IUbMMktGL._SL160_.jpg</URL><Height Units=\"pixels\">160</Height><Width Units=\"pixels\">126</Width></MediumImage><LargeImage><URL>http://ecx.images-amazon.com/images/I/51IUbMMktGL.jpg</URL><Height Units=\"pixels\">500</Height><Width Units=\"pixels\">394</Width></LargeImage></ImageSet></ImageSets><ItemAttributes><Author>Gary Wiviott</Author><Author>Colleen Rush</Author><Binding>Kindle Edition</Binding><EISBN>9780786751075</EISBN><Format>Kindle eBook</Format><Label>Running Press</Label><Languages><Language><Name>English</Name><Type>Published</Type></Language></Languages><Manufacturer>Running Press</Manufacturer><NumberOfPages>258</NumberOfPages><ProductGroup>eBooks</ProductGroup><ProductTypeName>ABIS_EBOOKS</ProductTypeName><PublicationDate>2009-08-05</PublicationDate><Publisher>Running Press</Publisher><ReleaseDate>2009-08-05</ReleaseDate><Studio>Running Press</Studio><Title>Low &amp; Slow: Master the Art of Barbecue in 5 Easy Lessons</Title></ItemAttributes><CustomerReviews><IFrameURL>http://www.amazon.com/reviews/iframe?akid=AKIAIC5IGJNCXA3YR2FA&amp;alinkCode=xm2&amp;asin=B004WOPHN0&amp;atag=AcmeBooks&amp;exp=2015-06-15T11%3A27%3A47Z&amp;v=2&amp;sig=oNHhXbh55uDr5buYFYAhb%2BQGN%2F466E1f973rtwFPjL0%3D</IFrameURL><HasReviews>true</HasReviews></CustomerReviews><EditorialReviews><EditorialReview><Source>Product Description</Source><Content>&lt;div&gt;&lt;p&gt;Step away from the propane tank. Surrender all of your notions about barbecue. Forget everything you've ever learned about cooking with charcoal and fire. It is all wrong. Get it right with the \"Five Easy Lessons\" program, which includes over 130 recipes and step-by-step instructions for setting up and cooking low and slow on a Weber Smokey Mountain, an offset smoker, or a kettle grill.&lt;P&gt;This program is guided by a singular philosophy: Keep It Simple, Stupid. Do exactly as Gary says, don't even think about opening the lid before it's time, and you will learn:&lt;P&gt;o What gear you do and, more importantly, don't need&lt;BR&gt;o Exactly how to start and maintain a proper fire (without lighter fluid)&lt;BR&gt;o All about marinades, brines, and rubs&lt;BR&gt;o To use your senses and trust your instincts (instead of thermometers)&lt;BR&gt;o How to make delicious, delicious barbecue&lt;/p&gt;&lt;/div&gt;</Content><IsLinkSuppressed>0</IsLinkSuppressed></EditorialReview><EditorialReview><Source>Product Description</Source><Content>&lt;div&gt;&lt;p&gt;Step away from the propane tank. Surrender all of your notions about barbecue. Forget everything you've ever learned about cooking with charcoal and fire. It is all wrong. Get it right with the \"Five Easy Lessons\" program, which includes over 130 recipes and step-by-step instructions for setting up and cooking low and slow on a Weber Smokey Mountain, an offset smoker, or a kettle grill.&lt;P&gt;This program is guided by a singular philosophy: Keep It Sim" \
    "ple, Stupid. Do exactly as Gary says, don't even think about opening the lid before it's time, and you will learn:&lt;P&gt;o What gear you do and, more importantly, don't need&lt;BR&gt;o Exactly how to start and maintain a proper fire (without lighter fluid)&lt;BR&gt;o All about marinades, brines, and rubs&lt;BR&gt;o To use your senses and trust your instincts (instead of thermometers)&lt;BR&gt;o How to make delicious, delicious barbecue&lt;/p&gt;&lt;/div&gt;</Content><IsLinkSuppressed>0</IsLinkSuppressed></EditorialReview></EditorialReviews><SimilarProducts><SimilarProduct><ASIN>B00VPPSRWM</ASIN><Title>Low &amp; Slow 2: The Art of Barbecue, Smoke-Roasting, and Basic Curing</Title></SimilarProduct><SimilarProduct><ASIN>B004MME6MK</ASIN><Title>Smokin' with Myron Mixon: Backyard 'Cue Made Simple from the Winningest Man in Barbecue</Title></SimilarProduct><SimilarProduct><ASIN>B007RZ07B8</ASIN><Title>Slow Fire: The Beginner's Guide to Lip-Smacking Barbecue</Title></SimilarProduct><SimilarProduct><ASIN>B00K5Y22QG</ASIN><Title>Weber's Smoke: A Guide to Smoke Cooking for Everyone and Any Grill</Title></SimilarProduct><SimilarProduct><ASIN>B00N6PFBDW</ASIN><Title>Franklin Barbecue: A Meat-Smoking Manifesto</Title></SimilarProduct></SimilarProducts><BrowseNodes><BrowseNode><BrowseNodeId>4246</BrowseNodeId><Name>Barbecuing &amp; Grilling</Name><Ancestors><BrowseNode><BrowseNodeId>4245</BrowseNodeId><Name>Outdoor Cooking</Name><Ancestors><BrowseNode><BrowseNodeId>6</BrowseNodeId><Name>Cookbooks, Food &amp; Wine</Name><Ancestors><BrowseNode><BrowseNodeId>1000</BrowseNodeId><Name>Subjects</Name><IsCategoryRoot>1</IsCategoryRoot><Ancestors><BrowseNode><BrowseNodeId>283155</BrowseNodeId><Name>Books</Name></BrowseNode></Ancestors></BrowseNode></Ancestors></BrowseNode></Ancestors></BrowseNode></Ancestors></BrowseNode><BrowseNode><BrowseNodeId>156196011</BrowseNodeId><Name>Outdoor Cooking</Name><Ancestors><BrowseNode><BrowseNodeId>156154011</BrowseNodeId><Name>Cookbooks, Food &amp; Wine</Name><Ancestors><BrowseNode><BrowseNodeId>154606011</BrowseNodeId><Name>Kindle eBooks</Name><Ancestors><BrowseNode><BrowseNodeId>133141011</BrowseNodeId><Name>Categories</Name><IsCategoryRoot>1</IsCategoryRoot><Ancestors><BrowseNode><BrowseNodeId>133140011</BrowseNodeId><Name>Kindle Store</Name></BrowseNode></Ancestors></BrowseNode></Ancestors></BrowseNode></Ancestors></BrowseNode></Ancestors></BrowseNode></BrowseNodes></Item></Items></ItemSearchResponse>";

    AmazonFacade facade;
    BookResult book = facade.parse( response );

    EXPECT_EQ(book.title(), std::string("Low & Slow: Master the Art of Barbecue in 5 Easy Lessons" ) );
    EXPECT_EQ(book.author(), std::string("Gary Wiviott" ) );
    EXPECT_EQ(book.coverUri(), std::string("http://ecx.images-amazon.com/images/I/51IUbMMktGL.jpg" ) );
    EXPECT_EQ(book.description(), std::string("<div><p>Step away from the propane tank. Surrender all of your notions about barbecue. Forget everything you've ever learned about cooking with charcoal and fire. It is all wrong. Get it right with the \"Five Easy Lessons\" program, which includes over 130 recipes and step-by-step instructions for setting up and cooking low and slow on a Weber Smokey Mountain, an offset smoker, or a kettle grill.<P>This program is guided by a singular philosophy: Keep It Simple, Stupid. Do exactly as Gary says, don't even think about opening the lid before it's time, and you will learn:<P>o What gear you do and, more importantly, don't need<BR>o Exactly how to start and maintain a proper fire (without lighter fluid)<BR>o All about marinades, brines, and rubs<BR>o To use your senses and trust your instincts (instead of thermometers)<BR>o How to make delicious, delicious barbecue</p></div>" ) );
    EXPECT_EQ(book.publicationDate(), std::string("2009-08-05" ) );
    EXPECT_EQ(book.publisher(), std::string("Running Press" ) );

}
}
