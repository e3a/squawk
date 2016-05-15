

#include "didl.h"

/* output the classes as string */
template < typename S, typename N >
struct SerializeStringImpl {

    typedef typename boost::fusion::result_of::value_at< S, N >::type current_t;
    typedef typename boost::mpl::next< N >::type next_t;
    typedef boost::fusion::extension::struct_member_name< S, N::value > name_t;

    template < typename Ostream >
    static inline void str(Ostream& os, const S & s) {
        os << name_t::call() << "=" << boost::fusion::at< N >(s);
        Comma< S, N >::comma(os);
        SerializeStringImpl< S, next_t >::str(os, s);
    };
};
template < typename S >
struct SerializeStringImpl< S, typename boost::fusion::result_of::size< S >::type > {
    template < typename Ostream > static inline void str(Ostream&, const S &) {}
};
template < typename S >
struct SerializeString : SerializeStringImpl< S, boost::mpl::int_< 0 > > {};


/**
 * \brief The DIDL Model and
 */
namespace didl {

std::ostream& operator<< ( std::ostream & os, const DidlObject & o ) {
    os << "DidlObject[";
    SerializeString<DidlObject>::str( os, o);
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlContainer & o ) {
    os << "DidlContainer[";
    SerializeString<DidlContainer>::str( os, o);
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlItem & o ) {
    os << "DidlItem[";
    SerializeString<DidlItem>::str( os, o);
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlContainerAlbum & o ) {
    os << "DidlContainerAlbum[";
    SerializeString<DidlContainerAlbum>::str( os, o);
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlContainerPhotoAlbum & o ) {
    os << "DidlContainerAlbum[";
    SerializeString<DidlContainerPhotoAlbum>::str( os, o);
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlContainerArtist & o ) {
    os << "DidlContainerArtist[";
    SerializeString<DidlContainerArtist>::str( os, o);
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlMusicTrack & o ) {
    os << "DidlMusicTrack[";
    SerializeString<DidlMusicTrack>::str( os, o);
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlPhoto & o ) {
    os << "DidlPhoto[";
    SerializeString<DidlPhoto>::str( os, o);
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlMovie & o ) {
    os << "DidlMovie[";
    SerializeString<DidlMovie>::str( os, o);
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlResource & o ) {
    os << "DidlResource[id=" << o.id() << ", ref_obj=" << o.refObj() << ", uri=" << o.uri() << ", protocolInfo=" << o.protocolInfo();
    for( auto & item : o.attributes() ) {
        os << ", " << item.first << "=" << item.second;
    }
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const std::list< DidlResource > & o ) {
    os << "DidlResources[";
    bool first = true;
    for( auto & item : o ) {
        if( first ) first = false; else os << ", ";
        os << item;
    }
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const DidlAlbumArtUri & o ) {
    os << "DidlAlbumArtUri[";
    os << "id=" << o.id() << "objectRef:" << o.objectRef() <<
        ", path:" << o.path() << ", uri:" << o.uri() << ", profile:" << o.profile();
    os << "]";
    return os;
}

std::ostream& operator<< ( std::ostream & os, const std::list< DidlAlbumArtUri > & o ) {
    os << "DidlAlbumArtUris[";
    bool first = true;
    for( auto & item : o ) {
        if( first ) first = false; else os << ", ";
        os << item;
    }
    os << "]";
    return os;
}
std::ostream& operator<< ( std::ostream & os, const DidlEBook & o ) {
    os << "DidlEBook[";
    SerializeString<DidlEBook>::str( os, o);
    os << "]";
    return os;
}
}//didl

