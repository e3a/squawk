#ifndef DIDL_H
#define DIDL_H

#include <cstdio>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <http.h>

#include <boost/type_traits.hpp> // is_array, is_class, remove_bounds

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/next_prior.hpp>

#include <boost/fusion/mpl.hpp>
#include <boost/fusion/adapted.hpp> // BOOST_FUSION_ADAPT_STRUCT

// boost::fusion::result_of::value_at
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/include/value_at.hpp>

// boost::fusion::result_of::size
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/include/size.hpp>

// boost::fusion::at
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/include/at.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "xml.h"

namespace upnp {

/**
 * @brief XML DIDL NAMESPACE
 */
const static std::string XML_NS_DIDL = "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/";
/**
 * @brief XML PURL NAMESPACE
 */
const static std::string XML_NS_PURL = "http://purl.org/dc/elements/1.1/";
/**
 * @brief XML DLNA NAMESPACE
 */
const static std::string XML_NS_DLNA = "urn:schemas-dlna-org:metadata-1-0/";
/**
 * @brief XML DLNA Metadata NAMESPACE
 */
const static std::string XML_NS_DLNA_METADATA = "urn:schemas-dlna-org:metadata-1-0/";
/**
 * @brief XML PV NAMESPACE
 */
const static std::string XML_NS_PV = "http://www.pv.com/pvns/";
/**
 * @brief XML UPNP NAMESPACE
 */
const static std::string XML_NS_UPNP = "urn:schemas-upnp-org:metadata-1-0/upnp/";
}//namespace upnp

//forward declaration of class - make it a friend in DidlObject.
namespace squawk {
class UpnpContentDirectoryDao;
}

/**
 * \brief The DIDL Model and
 */
namespace didl {

/** \brief sprintf to string wrapper */
template<typename ... Args>
std::string stringFormat( const std::string& format, Args ... args ) {
    size_t size = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 );
}

/**
 * @brief The DidlException class
 */
class DidlException : public std::exception {
public:
	explicit DidlException ( int _code, std::string _what ) throw() : _code ( _code ), _what ( _what ) {}
	virtual ~DidlException() throw() {}

	/** \brief The DIDL exception codes */
	enum ExceptionTypes {
		/** \brief The DIDL object type can not be found */
		DIDL_ELEMENT_NOT_FouND = 0,
		/** \brief Unknown DIDL exception */
		DIDL_OTHER_EXCEPTION = 254
	};

        virtual const char* what() const throw() { return _what.c_str(); }
        int code() throw() { return _code; }

private:
    int _code;
    std::string _what;
};

enum DIDL_CLASS {
    object = 0,
	objectContainer = 1,
	objectContainerAlbumMusicAlbum = 2,
	objectContainerPersonMusicArtist = 3,
	objectItem = 4,
	objectItemAudioItem = 5,
	objectItemAudioItemMusicTrack = 6,
	objectItemImageItemPhoto = 7,
	objectItemVideoItemMovie = 8
};

/* \brief Get the class name as string */
inline std::string className ( DIDL_CLASS cls ) {
	switch ( cls ) {
	case objectContainer:
		return "object.container";

	case objectContainerAlbumMusicAlbum:
		return "object.container.album.musicAlbum";

	case objectContainerPersonMusicArtist:
		return "object.container.person.musicArtist";

	case objectItem:
		return "object.item";

	case objectItemAudioItem:
		return "object.item.audioItem";

	case objectItemAudioItemMusicTrack:
		return "object.item.audioItem.musicTrack";

	case objectItemImageItemPhoto:
		return "object.item.imageItem.photo";

	case objectItemVideoItemMovie:
		return "object.item.videoItem.movie";

	default:
		return "object";
	}
}

/** \brief the album art */
struct DidlAlbumArtUri {
	DidlAlbumArtUri ( const size_t id, const size_t object_ref, const std::string & path, const std::string & uri, const std::string & profile ) :
		_id ( id ), _object_ref ( object_ref ), _path ( path ), _uri ( uri ), _profile ( profile ) {}

	DidlAlbumArtUri ( const DidlAlbumArtUri& ) = default;
	DidlAlbumArtUri ( DidlAlbumArtUri&& ) = default;
	DidlAlbumArtUri& operator= ( const DidlAlbumArtUri& ) = default;
	DidlAlbumArtUri& operator= ( DidlAlbumArtUri&& ) = default;
	~DidlAlbumArtUri() {}

        /** \brief Identifier of the object. */
        size_t id() const { return _id; }
        /** \brief Object id reference */
        size_t objectRef() const { return _object_ref; }
        /** \brief Path of the album art */
        std::string path() const { return _path; }
        /** \brief http uri for the album art */
        std::string uri() const { return _uri; }
        /** \brief Image profile */
        std::string profile() const { return _profile; }

private:
	size_t _id, _object_ref;
	const std::string _path, _uri, _profile;
};

/** \brief  Media resource reference
 * <p>The res property indicates a resource, typically a media file, associated with the object.</p>
 */
struct DidlResource {
public:
    enum UPNP_RES_ATTRIBUTES {
        /** \brief the size of the file in bytes */ size,
        /** \brief playback time of the media in seconds */ duration,
        /** \brief the bitrate of the media */ bitrate,
        /** \brief bit per sample */ bitsPerSample,
        /** \brief sample frequency of the media */ sampleFrequency,
        /** \brief the number of channels of the media */ nrAudioChannels,
        /** \brief the resolution of the media XxY (320x200) */ resolution,
        /** \brief color depth of the media */ colorDepth,
        /** \brief frame rate of the video */ framerate,
        /** \brief the dlna profile id of this media */ dlnaProfile,
        /** \brief the mime-type */ mimeType  };

    DidlResource ( const size_t id, const size_t ref_id,
                   const std::string uri, const std::string protocol_info,
                   const std::map< UPNP_RES_ATTRIBUTES, std::string > attributes ) :
        _id(id), _ref_obj(ref_id), _uri ( uri ), _protocol_info ( protocol_info ), _attributes ( attributes ) {}


    /** \brief the id this resource */
    size_t id() const { return _id; }
    /** \brief the reference to the ObjectItem */
    size_t refObj() const { return _ref_obj; }
    /** \brief the uri where this resource is located */
	std::string uri() const { return _uri; }
    /** \brief the dlna protocol info of this resource */
	std::string protocolInfo() const { return _protocol_info; }
    /** \brief get the attributes of the resource */
    std::map< UPNP_RES_ATTRIBUTES, std::string > attributes () const { return _attributes; }

private:
    const size_t _id, _ref_obj;
	const std::string _uri;
	const std::string _protocol_info;
	std::map< UPNP_RES_ATTRIBUTES, std::string > _attributes;
};

#define DIDL_OBJECT_ATTRIBUTES \
    const size_t id, size_t parent_id, const std::string & title, \
    const std::string & path, const unsigned long mtime, const size_t object_update_id
#define DIDL_OBJECT_ATTRiBUTES_NAMES id, parent_id, title, path, mtime, object_update_id
#define DIDL_OBJECT_ASSIGNMENTS _id(id), _parent_id(parent_id), _title(title), _path(path), _mtime(mtime), _object_update_id(object_update_id)

#define DIDL_CONTAINER_ATTRIBUTES const size_t child_count
#define DIDL_CONTAINER_ATTRiBUTES_NAMES child_count
#define DIDL_CONTAINER_ATTRIBUTES_ASSIGNMENTS _child_count(child_count)
#define DIDL_CONTAINER_ATTRIBUTES_ASSIGNMENTS_EMPTY _child_count(0)
#define DIDL_CONTAINER_ATTRiBUTES_NAMES_EMPTY 0

#define DIDL_ITEM_ATTRIBUTES const size_t size, const std::string & mime_type
#define DIDL_ITEM_ATTRiBUTES_NAMES size, mime_type
#define DIDL_ITEM_ATTRiBUTES_ASSIGNMENTS _size(size), _mime_type(mime_type)
#define DIDL_ITEM_ATTRiBUTES_ASSIGNMENTS_EMPTY _size(0), _mime_type("")

#define DIDL_OBJECT_ALL_ATTRIBUTES \
    DIDL_OBJECT_ATTRIBUTES, /*Object attributes*/\
    DIDL_CONTAINER_ATTRIBUTES, /*container attributes*/\
    DIDL_ITEM_ATTRIBUTES, /*Item attributes */\
    const size_t rating, const size_t year, const size_t track,const size_t playback_count, const std::string & contributor,\
    const std::string & artist, const std::string & author, const std::string & publisher, const std::string & genre, const std::string & album,\
    const std::string & series, const std::string & dlna_profile,\
    const unsigned long last_playback_time, std::list< DidlAlbumArtUri > album_art_uri = std::list< DidlAlbumArtUri >(),\
    std::list< DidlResource > item_resource = std::list< DidlResource >()
#define DIDL_OBJECT_INITIALIZER \
    DIDL_OBJECT_ASSIGNMENTS,\
    DIDL_CONTAINER_ATTRIBUTES_ASSIGNMENTS,\
    DIDL_ITEM_ATTRiBUTES_ASSIGNMENTS,\
    _rating(rating), _year(year), _track(track), _playback_count(playback_count),\
    _contributor(contributor), _artist(artist), _author(author),\
    _publisher(publisher), _genre(genre), _album(album), _series(series), _dlna_profile(dlna_profile),\
    _last_playback_time(last_playback_time), _album_art_uri(album_art_uri), _item_resource(item_resource)
#define DIDL_OBJECT_OBJECT_ATTRIBUTES_CREATE \
    DIDL_OBJECT_ATTRiBUTES_NAMES,\
    DIDL_CONTAINER_ATTRiBUTES_NAMES,\
    DIDL_ITEM_ATTRiBUTES_NAMES,\
    rating, year, track, playback_count,\
    contributor, artist, author, publisher, genre, album, series, dlna_profile, last_playback_time, album_art_uri, item_resource

#define DIDL_OBJECT_CTOR() \
    DidlObject( DIDL_CLASS cls, DIDL_OBJECT_ALL_ATTRIBUTES, const bool import = true ) :\
            _cls ( cls ), DIDL_OBJECT_INITIALIZER, _import ( import ) {}

#define DIDL_CTOR(className) \
    className( DIDL_CLASS cls, DIDL_OBJECT_ALL_ATTRIBUTES, const bool import = true ) :\
            DidlObject ( cls, DIDL_OBJECT_OBJECT_ATTRIBUTES_CREATE, import ) {}

struct DidlObject {
public:
    DidlObject() {} //TODO
    DidlObject ( DIDL_CLASS cls, DIDL_OBJECT_ATTRIBUTES, const bool import = true ) :
            _cls ( cls ), DIDL_OBJECT_ASSIGNMENTS, DIDL_CONTAINER_ATTRIBUTES_ASSIGNMENTS_EMPTY, DIDL_ITEM_ATTRiBUTES_ASSIGNMENTS_EMPTY,
                _rating ( 0 ), _year ( 0 ), _track ( 0 ), _playback_count ( 0 ),
                _contributor ( "" ), _artist ( "" ), _author ( "" ),
                _publisher ( "" ), _genre ( "" ), _album(""), _series ( "" ),
                _dlna_profile ( "" ), _last_playback_time ( 0 ), _import ( import ) {}

	DidlObject ( const DidlObject& ) = default;
	DidlObject ( DidlObject&& ) = default;
	DidlObject& operator= ( const DidlObject& ) = default;
	DidlObject& operator= ( DidlObject&& ) = default;
	~DidlObject() {}

    DIDL_CLASS cls() const { return _cls; }
	size_t id() const { return _id; }
	size_t parentId() const { return _parent_id; }
	std::string title() const { return _title; }
	std::string path() const { return _path; }
	unsigned long mtime () const { return _mtime; }

	size_t objectUdpateId() const { return _object_update_id; }
	bool import() const { return _import; }

	friend std::ostream& operator<< ( std::ostream & os, const DidlObject & o );
	friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;

protected:
    DIDL_OBJECT_CTOR()

    DIDL_CLASS _cls;
    size_t _id, _parent_id;
    std::string _title,  _path;
    unsigned long _mtime;
    size_t _object_update_id;

    size_t _child_count;

    size_t _size;
    std::string _mime_type;

    size_t _rating, _year, _track, _playback_count;
    std::string _contributor, _artist, _author, _publisher, _genre, _album, _series, _dlna_profile;
    unsigned long _last_playback_time;

	std::list< DidlAlbumArtUri > _album_art_uri;
    std::list< DidlResource > _item_resource;
    bool _import;
};

struct DidlContainer : public DidlObject {
public:
    DidlContainer ( DIDL_OBJECT_ATTRIBUTES, DIDL_CONTAINER_ATTRIBUTES, const bool import = true ) :
                DidlObject ( objectContainer, DIDL_OBJECT_ATTRiBUTES_NAMES, DIDL_CONTAINER_ATTRiBUTES_NAMES, 0, "",
                     0, 0, 0, 0, "", "", "", "", "", "", "", "", 0,
					 std::list< DidlAlbumArtUri >(),
                     std::list< DidlResource >(), import ) {}

    DidlContainer ( const DidlContainer& ) = default;
	DidlContainer ( DidlContainer&& ) = default;
	DidlContainer& operator= ( const DidlContainer& ) = default;
	DidlContainer& operator= ( DidlContainer&& ) = default;
	~DidlContainer() {}

	/** \brief The number of childs for this container */
	size_t childCount() const { return _child_count; }

    friend class squawk::UpnpContentDirectoryDao;

protected:
    DIDL_CTOR(DidlContainer)
};

static didl::DidlContainer DIDL_ROOT_NODE( 0, 0, "Root", "", 0, 0, 0 );
static didl::DidlContainer DIDL_CONTAINER_EMPTY( 0, 0, "", "", 0, 0, 0 );

struct DidlItem : public DidlObject {
public:
    DidlItem ( DIDL_CLASS cls, DIDL_OBJECT_ATTRIBUTES, DIDL_ITEM_ATTRIBUTES, const bool import = true ) :
                DidlObject ( cls, DIDL_OBJECT_ATTRiBUTES_NAMES, DIDL_CONTAINER_ATTRiBUTES_NAMES_EMPTY, DIDL_ITEM_ATTRiBUTES_NAMES,
                             0, 0, 0, 0, "", "", "", "", "", "", "", "", 0,
                             std::list< DidlAlbumArtUri>(), std::list< DidlResource >(), import ) {}

	DidlItem ( const DidlItem& ) = default;
	DidlItem ( DidlItem&& ) = default;
	DidlItem& operator= ( const DidlItem& ) = default;
	DidlItem& operator= ( DidlItem&& ) = default;
	~DidlItem() {}

	/** \brief Size of the file represented by this item */
	size_t size() const { return _size; }
	/** \brief  mime-type of this item */
	std::string mimeType() const { return _mime_type; }

    friend class squawk::UpnpContentDirectoryDao;

protected:
    DIDL_CTOR(DidlItem)

};
/* \brief The didl music album */
struct DidlContainerAlbum : public DidlContainer {
public:
    DidlContainerAlbum ( DIDL_OBJECT_ATTRIBUTES, DIDL_CONTAINER_ATTRIBUTES,
						 const size_t & rating, const size_t & year, const size_t playback_count,
                         const std::string & contributor,const std::string & artist, const std::string & genre,
						 std::list< DidlAlbumArtUri > album_art_uri = std::list< DidlAlbumArtUri >(),
						 const bool import = true ) :
		DidlContainer ( objectContainerAlbumMusicAlbum, id, parent_id, title, path, mtime, object_update_id,
						child_count,
                        0, "", rating, year, 0, playback_count, contributor, artist, "", "", genre, "", "", "", 0,
                        album_art_uri, std::list< DidlResource >(), import ) {}

	DidlContainerAlbum ( const DidlContainerAlbum& ) = default;
	DidlContainerAlbum ( DidlContainerAlbum&& ) = default;
	DidlContainerAlbum& operator= ( const DidlContainerAlbum& ) = default;
	DidlContainerAlbum& operator= ( DidlContainerAlbum&& ) = default;
	~DidlContainerAlbum() {}

	/* TODO actor?, creator? */
    /** \brief Album contributor */
    std::string contributor() const { return _contributor; }
	/** \brief Album artist */
	std::string artist() const { return _artist; }
	/* \brief The music genre */
	std::string genre() const { return _genre; }
	/** \brief Release date */
	size_t year () const { return _year; }
	/** \brief Track on medium */
	size_t track() const { return _track; } //TODO NO TRACK FOR ALBUM
	/** \brief number of times this song was played */
	size_t playbackCount() const { return _playback_count; }
	/* TODO LAST PLAYBACK TIME */
	/** \brief User rating of this album */
    size_t rating() const { return _rating; }
	/** \brief The album art object */
	std::list< DidlAlbumArtUri > albumArtUri() const { return _album_art_uri; }
};

/**
 * @brief The Artist Container struct
 */
struct DidlContainerArtist : public DidlContainer {
public:
    DidlContainerArtist ( DIDL_OBJECT_ATTRIBUTES,  DIDL_CONTAINER_ATTRIBUTES, const std::string & clean_name, const bool import = true ) :
        DidlContainer ( objectContainerPersonMusicArtist,
                        DIDL_OBJECT_ATTRiBUTES_NAMES,
                        DIDL_CONTAINER_ATTRiBUTES_NAMES,
                        0, "", /*item: size, mime-type*/
                        0, 0, 0, 0, "", "", "", "", "", "", "", "", 0,
                        std::list< DidlAlbumArtUri >(),
                        std::list< DidlResource >(), import ), _clean_name(clean_name) {}

	DidlContainerArtist ( const DidlContainerArtist& ) = default;
	DidlContainerArtist ( DidlContainerArtist&& ) = default;
	DidlContainerArtist& operator= ( const DidlContainerArtist& ) = default;
	DidlContainerArtist& operator= ( DidlContainerArtist&& ) = default;
	~DidlContainerArtist() {}


    /** \brief the normalized artist name */
    std::string cleanName() const { return _clean_name; }

private:
    const std::string _clean_name;
};
struct DidlMusicTrack : public DidlItem {
public:
	DidlMusicTrack ( DIDL_OBJECT_ATTRIBUTES,
                     DIDL_ITEM_ATTRIBUTES,
                     const size_t rating, const size_t year, const size_t track,
                     const size_t playback_count, const std::string & contributor,
                     const std::string & artist, const std::string & genre, const std::string & album,
					 const unsigned long last_playback_time,
                     const std::list< DidlResource > item_resource,
					 const bool import = true ) :
                DidlItem ( objectItemAudioItemMusicTrack, DIDL_OBJECT_ATTRiBUTES_NAMES, DIDL_CONTAINER_ATTRiBUTES_NAMES_EMPTY, DIDL_ITEM_ATTRiBUTES_NAMES,
                           rating, year, track, playback_count, contributor, artist, "", "", genre, album, "", "" /* DLNA_PROFILE */,
                           last_playback_time, std::list< DidlAlbumArtUri >(), item_resource, import ) {}

	DidlMusicTrack ( const DidlMusicTrack& ) = default;
	DidlMusicTrack ( DidlMusicTrack&& ) = default;
	DidlMusicTrack& operator= ( const DidlMusicTrack& ) = default;
	DidlMusicTrack& operator= ( DidlMusicTrack&& ) = default;
	~DidlMusicTrack() {}

    /* \brief The music genre */
    std::string genre() const { return _genre; }

    /** \brief Album name */
    std::string album() const { return _album; }
    /** \brief Album contributor */
    std::string contributor() const { return _contributor; }
	/** \brief Album artist */
	std::string artist() const { return _artist; }
	/** \brief Release date */
	size_t year () const { return _year; }
	/** \brief Track on medium */
	size_t track() const { return _track; }
	/** \brief number of times this song was played */
	size_t playbackCount() const { return _playback_count; }
	/** \brief number of times this song was played */
	unsigned long lastPlaybackTime() const { return _last_playback_time; }
	/** \brief User rating of this album */
	size_t rating() const { return _rating; }
    /** \brief The track ressource */
    std::list< DidlResource > const audioItemRes() { return _item_resource; }
};
struct DidlPhoto: public DidlItem {
public:

    DidlPhoto ( DIDL_OBJECT_ATTRIBUTES, DIDL_ITEM_ATTRIBUTES, const bool import = true ) :
                DidlItem ( objectItemImageItemPhoto, DIDL_OBJECT_ATTRiBUTES_NAMES, DIDL_CONTAINER_ATTRiBUTES_NAMES_EMPTY, DIDL_ITEM_ATTRiBUTES_NAMES,
                           0, 0, 0, 0, "", "", "", "", "", "", "", "", 0, std::list< DidlAlbumArtUri>(), std::list< DidlResource >(), import ) {}

	DidlPhoto ( const DidlPhoto& ) = default;
	DidlPhoto ( DidlPhoto&& ) = default;
	DidlPhoto& operator= ( const DidlPhoto& ) = default;
	DidlPhoto& operator= ( DidlPhoto&& ) = default;
	~DidlPhoto() {}
};
struct DidlMovie: public DidlItem {
public:

    DidlMovie ( DIDL_OBJECT_ATTRIBUTES, DIDL_ITEM_ATTRIBUTES, const bool import = true ) :
                DidlItem ( objectItemVideoItemMovie, DIDL_OBJECT_ATTRiBUTES_NAMES, DIDL_CONTAINER_ATTRiBUTES_NAMES_EMPTY, DIDL_ITEM_ATTRiBUTES_NAMES,
                           0, 0, 0, 0, "", "", "", "", "", "", "", "", 0,
                           std::list< DidlAlbumArtUri>(),
                           std::list< DidlResource >(),
                           import ) {}

	DidlMovie ( const DidlMovie& ) = default;
	DidlMovie ( DidlMovie&& ) = default;
	DidlMovie& operator= ( const DidlMovie& ) = default;
	DidlMovie& operator= ( DidlMovie&& ) = default;
	~DidlMovie() {}
};

/** \brief Get typename for object */
template <typename T>
struct cls {
    static DIDL_CLASS type() {
        return object;
    }
};

template <>
struct cls<DidlObject> {
    static DIDL_CLASS type() {
        return object;
    }
};

template <>
struct cls<DidlContainer> {
    static DIDL_CLASS type() {
        return objectContainer;
    }
};


/** \brief Output the didl objects to xml
<p><h4>Id Creation</h4>the id's in the xml will be the id argument strings. When the string contains
the format specifier for decimals (%d), the relating id will be inserted.</p>
<p><h4>URL Creation</h4>.</p>
*/
class DidlWriter {
//TODO do not throw xml exception
public:
    /** \brief Create the xml writer */
    DidlWriter ( commons::xml::XMLWriter * xmlWriter ) :
		_xmlWriter ( xmlWriter ),
		_didl_element ( _xmlWriter->element ( "DIDL-Lite" ) ) {

		_xmlWriter->ns ( _didl_element, upnp::XML_NS_DIDL );
		_xmlWriter->ns ( _didl_element, upnp::XML_NS_PURL, "dc" );
		_xmlWriter->ns ( _didl_element, upnp::XML_NS_DLNA, "dlna" );
		_xmlWriter->ns ( _didl_element, upnp::XML_NS_UPNP, "upnp" );
		_xmlWriter->ns ( _didl_element, upnp::XML_NS_PV, "pv" );
	}


    /** \brief container element */
	void container ( const std::string & id_prefix, const std::string & parent_prefix, DidlContainer container ) {
		commons::xml::Node container_element = _xmlWriter->element ( _didl_element, "", "container", "" );
        _xmlWriter->attribute ( container_element, "id", stringFormat( id_prefix, container.id() ) );
        _xmlWriter->attribute ( container_element, "parentID", didl::stringFormat( parent_prefix, container.parentId() ) );
		_xmlWriter->attribute ( container_element, "restricted", "1" );
		_xmlWriter->attribute ( container_element, "childCount", std::to_string ( container.childCount() ) );

		_xmlWriter->element ( container_element, upnp::XML_NS_PURL, "title", container.title() );
		_xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "class", className ( container.cls() ) );
	}
	void container ( const std::string & id_prefix, const std::string & parent_prefix, std::list<DidlContainer> container_list ) {
		for ( auto & _container : container_list ) {
			container ( id_prefix, parent_prefix, _container );
		}
	}
    void container ( const std::string & id, const std::string & parent, std::string uri, DidlContainerAlbum container ) {
		commons::xml::Node container_element = _xmlWriter->element ( _didl_element, "", "container", "" );
        _xmlWriter->attribute ( container_element, "id", didl::stringFormat( id, container.id() ) );
        _xmlWriter->attribute ( container_element, "parentID", didl::stringFormat( parent, container.parentId() ) );
		_xmlWriter->attribute ( container_element, "restricted", "1" );
		_xmlWriter->attribute ( container_element, "childCount", std::to_string ( container.childCount() ) );

		_xmlWriter->element ( container_element, upnp::XML_NS_PURL, "title", container.title() );
		_xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "class", className ( container.cls() ) );
		_xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "artist", container.artist() );
        _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "creator", container.contributor() );

        _xmlWriter->element ( container_element, upnp::XML_NS_PURL, "date", /* std::to_string ( container.year() ) + */ "-01-01" );

		for ( auto & cover : container.albumArtUri() ) {
			commons::xml::Node dlna_album_art_node =
                _xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "albumArtURI", didl::stringFormat( uri, cover.id() ) );
			_xmlWriter->ns ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "dlna", false );
			_xmlWriter->attribute ( dlna_album_art_node, upnp::XML_NS_DLNA_METADATA, "profileID", cover.profile() );
		}
	}
	void container ( const std::string & id_prefix, const std::string & parent_prefix, DidlContainerArtist container ) {
		commons::xml::Node container_element = _xmlWriter->element ( _didl_element, "", "container", "" );
        _xmlWriter->attribute ( container_element, "id", didl::stringFormat( id_prefix , container.id() ) );
        _xmlWriter->attribute ( container_element, "parentID", didl::stringFormat( parent_prefix, container.parentId() ) );
		_xmlWriter->attribute ( container_element, "restricted", "1" );
		_xmlWriter->attribute ( container_element, "childCount", std::to_string ( container.childCount() ) );

		_xmlWriter->element ( container_element, upnp::XML_NS_PURL, "title", container.title() );
		_xmlWriter->element ( container_element, upnp::XML_NS_UPNP, "class", className ( container.cls() ) );
	}
    void write ( const std::string & id_prefix, const std::string & parent_prefix, const std::string & uri, DidlMusicTrack item ) {
		commons::xml::Node item_element = _xmlWriter->element ( _didl_element, "", "item", "" );
        _xmlWriter->attribute ( item_element, "id", didl::stringFormat( id_prefix, item.id() ) );
        _xmlWriter->attribute ( item_element, "parentID", didl::stringFormat( parent_prefix, item.parentId() ) );
		_xmlWriter->attribute ( item_element, "restricted", "1" );
		_xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "class", className ( item.cls() ) );
		_xmlWriter->element ( item_element, upnp::XML_NS_PURL, "title",item.title() );

		_xmlWriter->element ( item_element, upnp::XML_NS_PURL, "originalTrackNumber", std::to_string ( item.track() ) );
        _xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "album", item.album() );

        //Get the date string, TODO utils...
        char buffer_year_ [11];
        time_t year_t_ = static_cast<time_t>( item.year() );

        struct tm * timeinfo_ = localtime ( &year_t_ );
        strftime (buffer_year_,11,"%Y-%m-%d", timeinfo_ );

        _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "date", buffer_year_ );
		_xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "genre", item.genre() );

		_xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "artist", item.artist() );
        _xmlWriter->element ( item_element, upnp::XML_NS_PURL, "contributor", item.contributor() );

		for ( auto & track : item.audioItemRes() ) {
            write ( track, item_element, uri );
		}
	}
	void write ( const std::string & id_prefix, const std::string & parent_prefix, DidlPhoto item ) {


		commons::xml::Node item_element = _xmlWriter->element ( _didl_element, "", "item", "" );
        _xmlWriter->attribute ( item_element, "id", didl::stringFormat( id_prefix, item.id() ) );
        _xmlWriter->attribute ( item_element, "parentID", didl::stringFormat( parent_prefix, item.parentId() ) );
		_xmlWriter->attribute ( item_element, "restricted", "1" );
		_xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "class", className ( item.cls() ) );
		_xmlWriter->element ( item_element, upnp::XML_NS_PURL, "title",item.title() );


		_xmlWriter->element ( item_element, upnp::XML_NS_PURL, "date", /* year_ + */ "2014-01-01" ); //TODO

//        for( auto & track : item.trackList() ) {
//            write( track, item_element );
//        }
	}
	void write ( const std::string & id_prefix, const std::string & parent_prefix, DidlMovie item ) {
		commons::xml::Node item_element = _xmlWriter->element ( _didl_element, "", "item", "" );
        _xmlWriter->attribute ( item_element, "id", didl::stringFormat( id_prefix, item.id() ) );
        _xmlWriter->attribute ( item_element, "parentID", didl::stringFormat( parent_prefix, item.parentId() ) );
		_xmlWriter->attribute ( item_element, "restricted", "1" );
		_xmlWriter->element ( item_element, upnp::XML_NS_UPNP, "class", className ( item.cls() ) );
		_xmlWriter->element ( item_element, upnp::XML_NS_PURL, "title",item.title() );


		_xmlWriter->element ( item_element, upnp::XML_NS_PURL, "date", /* year_ + */ "2014-01-01" ); //TODO

//        for( auto & track : item.trackList() ) {
//            write( track, item_element );
//        }
	}

private:
	commons::xml::XMLWriter * _xmlWriter;
	commons::xml::Node _didl_element;

    void write ( const DidlResource & item, commons::xml::Node & item_element, const std::string & res_uri ) {

        std::string mime_type_ = item.attributes()[DidlResource::mimeType];
        commons::xml::Node dlna_res_node = _xmlWriter->element ( item_element, "", "res",
            didl::stringFormat( res_uri, item.id(), http::mime::extension( mime_type_ ).c_str() ) );
        _xmlWriter->attribute ( dlna_res_node, "", "protocolInfo",
                                "http-get:*:" + mime_type_ + ":DLNA.ORG_OP=11;DLNA.ORG_FLAGS=01700000000000000000000000000000" );

		for ( auto & attr : item.attributes() ) {
            switch ( attr.first ) {
            case DidlResource::duration:
                _xmlWriter->attribute ( dlna_res_node, "", "duration", attr.second ); break;
            case DidlResource::size:
                _xmlWriter->attribute ( dlna_res_node, "", "size", attr.second ); break;
            case DidlResource::bitrate:
                _xmlWriter->attribute ( dlna_res_node, "", "bitrate", attr.second ); break;
            case DidlResource::bitsPerSample:
                _xmlWriter->attribute ( dlna_res_node, "", "bitsPerSample", attr.second ); break;
            case DidlResource::sampleFrequency:
                _xmlWriter->attribute ( dlna_res_node, "", "sampleFrequency", attr.second ); break;
            case DidlResource::nrAudioChannels:
                _xmlWriter->attribute ( dlna_res_node, "", "nrAudioChannels", attr.second ); break;
//TODO        case DidlResource::resolution:
//                _xmlWriter->attribute ( dlna_res_node, "", "resolution", attr.second ); break;
//            case DidlResource::dlnaProfile:
//                _xmlWriter->attribute ( dlna_res_node, "", "dlnaProfile", attr.second ); break;
//            case DidlResource::colorDepth:
//                _xmlWriter->attribute ( dlna_res_node, "", "colorDepth", attr.second ); break;
//            case DidlResource::framerate:
//                _xmlWriter->attribute ( dlna_res_node, "", "framerate", attr.second ); break;
//            case DidlResource::mimeType:
//                _xmlWriter->attribute ( dlna_res_node, "", "mime-type", attr.second ); break;
            }
		}
	}
};
} //didl
using namespace boost::fusion;

BOOST_FUSION_ADAPT_STRUCT(
    didl::DidlObject,
    (didl::DIDL_CLASS, _cls)
    (size_t, _id)
    (size_t, _parent_id)
    (std::string, _title)
    (std::string, _path)
    (unsigned long,  _mtime)
    (size_t, _object_update_id)
    (bool, _import) )

BOOST_FUSION_ADAPT_STRUCT(
    didl::DidlContainer,
        (didl::DIDL_CLASS, _cls)
        (size_t, _id)
        (size_t, _parent_id)
        (std::string, _title)
        (std::string, _path)
        (unsigned long,  _mtime)
        (size_t, _object_update_id)
        (size_t, _child_count)
        (bool, _import) )

template < typename S, typename N >
struct Comma {
    template < typename Ostream >
    static inline void comma(Ostream& os) {
        os << ", ";
    }
};

template < typename S >
struct Comma< S, typename boost::mpl::prior< typename boost::fusion::result_of::size< S >::type >::type > {
    template < typename Ostream >
    static inline void comma(Ostream&) {}
};

template < typename S, typename N >
struct StructImpl {

    typedef typename boost::fusion::result_of::value_at< S, N >::type current_t;
    typedef typename boost::mpl::next< N >::type next_t;
    typedef boost::fusion::extension::struct_member_name< S, N::value > name_t;

    template < typename Ostream >
    static inline void serialize(Ostream& os, const S & s) {
        std::string name_ = name_t::call();
        if( name_ == "_id" ) name_ = "_ROWID";
        os << name_.substr(1);
        Comma< S, N >::comma(os);

        StructImpl< S, next_t >::serialize(os, s);
    }

    template < typename Ostream >
    static inline void bind(Ostream& os, const S & s) {
        std::string name_ = name_t::call();

        os << name_ << "::" << typeid(s).name() << "::" <<  ""; //s;
        Comma< S, N >::comma(os);

        StructImpl< S, next_t >::bind(os, s);
    }

};

template < typename S >
struct StructImpl< S, typename boost::fusion::result_of::size< S >::type > {
    template < typename Ostream >
    static inline void serialize(Ostream& os, const S& s) {}

    template < typename Ostream >
    static inline void bind(Ostream& os, const S& s) {}
};





// Iterador sobre una estructura. Template fachada.
template < typename S >
struct Struct : StructImpl< S, boost::mpl::int_< 0 > > {};

#endif // DIDL_H
