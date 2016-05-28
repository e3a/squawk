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
#ifndef DIDLXMLWRITER_H
#define DIDLXMLWRITER_H

#include <boost/lexical_cast.hpp>

#include "gtest/gtest_prod.h"

#include "didl.h"
#include "../utils/xml.h"

namespace upnp {

/** @brief XML DIDL NAMESPACE */
const static std::string XML_NS_DIDL = "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/";
/** @brief XML PURL NAMESPACE */
const static std::string XML_NS_PURL = "http://purl.org/dc/elements/1.1/";
/** @brief XML DLNA NAMESPACE */
const static std::string XML_NS_DLNA = "urn:schemas-dlna-org:metadata-1-0/";
/** @brief XML DLNA Metadata NAMESPACE */
const static std::string XML_NS_DLNA_METADATA = "urn:schemas-dlna-org:metadata-1-0/";
/** @brief XML PV NAMESPACE */
const static std::string XML_NS_PV = "http://www.pv.com/pvns/";
/** @brief XML UPNP NAMESPACE */
const static std::string XML_NS_UPNP = "urn:schemas-upnp-org:metadata-1-0/upnp/";
}//namespace upnp

namespace didl {

/** \brief Output the didl objects to xml
<p><h4>Id Creation</h4>the id's in the xml will be the id argument strings. When the string contains
the format specifier for decimals (%d), the relating id will be inserted.</p>
<p><h4>URL Creation</h4>.</p>
*/
class DidlXmlWriter {
//TODO do not throw xml exception
public:
    /** \brief Create the xml writer */
    DidlXmlWriter ( commons::xml::XMLWriter * xmlWriter ) ;

    /** \brief container element */
    void container ( const std::string & id_prefix, const std::string & parent_prefix, DidlContainer container );

    void container ( const std::string & id_prefix, const std::string & parent_prefix, std::list<DidlContainer> container_list );

    void container ( const std::string & id, const std::string & parent, const std::string & uri, DidlContainerAlbum container );

    void container ( const std::string & id, const std::string & parent, const std::string & uri, DidlContainerPhotoAlbum container );

    void container ( const std::string & id_prefix, const std::string & parent_prefix, DidlContainerArtist container );

    void write ( const std::string & id_prefix, const std::string & parent_prefix, const std::string & uri, DidlItem item );

    void write ( const std::string & id_prefix, const std::string & parent_prefix, const std::string & uri_res, const std::string & uri_album_art, DidlMusicTrack item );

    void write ( const std::string & id_prefix, const std::string & parent_prefix, const std::string & uri, DidlPhoto item );

    void write ( const std::string & id_prefix, const std::string & parent_prefix, const std::string & uri, DidlMovie item );

private:
    commons::xml::XMLWriter * _xmlWriter;
    commons::xml::Node _didl_element;

    FRIEND_TEST( DidlXmlWriterTest, ParseDate );
    /**
     * @brief create a string  from the timestamp.
     * @param seconds the unix timestamp
     * @return The format: YYYY-mm-dd
     */
//    static std::string parse_date( long seconds ) {

//        std::array<char, 32> buffer_;
//        time_t time = seconds;
//        tm local;
//        localtime_r(&time, &local);
//        std::strftime(buffer_.data(), 32, "%Y-%m-%d", &local);

//        return std::string( buffer_.data() );
//    }

    void write ( const DidlResource & item, commons::xml::Node & item_element, const std::string & res_uri );
};

}//didl
#endif // DIDLXMLWRITER_H
