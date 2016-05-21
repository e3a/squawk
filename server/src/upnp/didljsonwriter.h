#ifndef DIDLJSONWRITER_H
#define DIDLJSONWRITER_H

#include "didl.h"

namespace didl {

// forward declaration
template < typename T >
struct serializer;

static const std::list< std::string > skiplist = {
    "_mtime", "_path", "_object_update_id", "_clean_name", "_import"
};

static inline std::string remove_underscore( const std::string & str ) {
    return str.substr( 1 );
}

/* add comma to the stream */
template < typename S, typename N >
struct Comma {
    template < typename Ostream >
    static inline void comma(Ostream& os) {
        os << ", ";
    }
};

template < typename S >
struct Comma< S, boost::mpl::bool_<true> > {
    template < typename Ostream >
    static inline void comma(Ostream&) {}
};


/* output the classes as string */
template < typename S, typename N, typename FIRST >
struct SerializeJsonImpl {

    typedef typename boost::fusion::result_of::value_at< S, N >::type current_t;
    typedef typename boost::mpl::next< N >::type next_t;
    typedef boost::fusion::extension::struct_member_name< S, N::value > name_t;

    template < typename Ostream >
    static inline void json(Ostream& os, const S & s, const std::list< std::string > & attributes ) {
        //is name is skip list
        std::string name_ = name_t::call();
        if( std::find( skiplist.begin(), skiplist.end(), name_ ) == skiplist.end() ) {
            //is name in filter
            name_ = remove_underscore( name_ );
            if( attributes.empty() || std::find( attributes.begin(), attributes.end(), name_ ) != attributes.end() ) {
                Comma< S, FIRST >::comma(os);
                os << "\"" << name_ << "\":";
                serializer< current_t >::serialize(os, boost::fusion::at< N >(s), attributes );
                SerializeJsonImpl< S, next_t, boost::mpl::bool_<false> >::json(os, s, attributes );
            } else {
                SerializeJsonImpl< S, next_t, FIRST >::json(os, s, attributes );
            }
        } else {
            SerializeJsonImpl< S, next_t, FIRST >::json(os, s, attributes );
        }
    };
};
template < typename S, typename FIRST >
struct SerializeJsonImpl< S, typename boost::fusion::result_of::size< S >::type, FIRST > {
    template < typename Ostream > static inline void json(Ostream&, const S &, const std::list< std::string >& ) {}
};
template < typename S >
struct SerializeJson : SerializeJsonImpl< S, boost::mpl::int_< 0 >, boost::mpl::bool_<true> > {};



template < typename T >
struct enum_serializer {

    template < typename Ostream >
    static inline void serialize(Ostream& os, const T& t, const std::list< std::string >& ) {
        os << "\"" << didl::className( t ) << "\"";
    }
};

template < typename T >
struct struct_serializer {
    typedef struct_serializer< T > type;

    template < typename Ostream >
    static inline void serialize( Ostream& os, const T & t, const std::list< std::string > & attributes ) {
        os << "{";
        SerializeJson< T >::json(os, t, attributes );
        os << "}";
    }
};
template <>
struct struct_serializer<std::string> {
    template < typename Ostream >
    static inline void serialize(Ostream& os, const std::string & t, const std::list< std::string >& ) {
        os << "\"" << t << "\"";
    }
};
template <>
struct struct_serializer<std::map< std::string, int > > {
    template < typename Ostream >
    static inline void serialize(Ostream& os, const std::map< std::string, int > & o, const std::list< std::string >& ) {
        os << "{";
        bool first = true;
        for( auto & item : o ) {
            if( first ) first = false; else os << ",";
            os << "\"" << item.first << "\":" << item.second;
        }
        os << "}";
    }
};
template <>
struct struct_serializer< didl::DidlResource > {
    template < typename Ostream >
    static inline void serialize(Ostream& os, const didl::DidlResource & o, const std::list< std::string >& ) {
        os << "{";
        os << "\"id\":" << o.id() << ", \"protocolInfo\":\"" << o.protocolInfo() << "\", \"refObj\":\"" << o.refObj() << "\", \"uri\":\"" << o.uri() << "\"";
        for( auto & item : o.attributes() ) {
            os << ", \"" << o.UPNP_RES_ATTRIBUES_NAMES.at( item.first ) << "\": \"" << item.second << "\"";
        }
        os << "}";
    }
};
template <>
struct struct_serializer< didl::DidlAlbumArtUri > {
    template < typename Ostream >
    static inline void serialize(Ostream& os, const didl::DidlAlbumArtUri & o, const std::list< std::string >& ) {
        os << "{";
        os << "\"id\":" << o.id() << ", \"object_ref\":" << o.objectRef() <<
              ", \"path\":\"" << o.path() << "\", \"uri\":\"" << o.uri() <<
              "\", \"profile\":\"" << o.profile() << "\"";
        os << "}";
    }
};

template <  typename T >
struct struct_serializer < std::list< T > > {
    typedef struct_serializer< T > type;

    template < typename Ostream >
    static inline void serialize(Ostream& os, const std::list< T > & t, const std::list< std::string > & attributes ) {
        os << "[";
        bool first = true;
        for( auto & item : t ) {
            if( first ) first = false; else os << ",";
            serializer< T >::serialize( os, item, attributes );
        }
        os << "]";
    }
};

template < typename T >
struct arithmetic_serializer {
    typedef arithmetic_serializer< T > type;
    template < typename Ostream >
    static inline void serialize(Ostream& os, const T& t, const std::list< std::string >& ) {
        os << t;
    }
};

template < typename T >
struct calculate_serializer {
    typedef
        typename boost::mpl::eval_if< boost::is_enum< T >,
            boost::mpl::identity< enum_serializer < T > >,
        //else
        typename boost::mpl::eval_if< boost::is_class< T >,
            boost::mpl::identity< struct_serializer < T > >,
        //else
            boost::mpl::identity< arithmetic_serializer < T > >
        >
        >::type type;

};

template < typename T >
struct serializer : public calculate_serializer < T >::type{};

}//namespace didl
#endif // DIDLJSONWRITER_H
