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

#include "pcrecpp.h"

#include <gtest/gtest.h>


TEST(TestIsbnParser, TestRegexp) {
    static pcrecpp::RE re("ISBN(-10)? *:? *([\\d*-?]*)");

    std::string dummy_, isbn_;

    EXPECT_TRUE( re.PartialMatch( "xxx ISBN: 978-0-9922794-3-1 xxxx", &dummy_, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string("978-0-9922794-3-1" ) );

    EXPECT_TRUE( re.PartialMatch( "xxx ISBN-10: 1-59327-580-3 xxxx", &dummy_, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string("1-59327-580-3" ) );


    EXPECT_TRUE( re.PartialMatch( "xxx ISBN 0-415-93551-2 (pbk.)", &dummy_, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string("0-415-93551-2" ) );

    EXPECT_TRUE( re.PartialMatch( "f Congress Control Number: 2008943eISBN : 978-0-786-75107-5", &dummy_, &isbn_ ) );
    EXPECT_EQ(isbn_, std::string("978-0-786-75107-5" ) );



}
