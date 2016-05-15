#ifndef DIDL_H
#define DIDL_H

#include <cstdio>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <http.h>

#include <boost/type_traits.hpp>

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/next_prior.hpp>

#include <boost/fusion/mpl.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>
#include <boost/fusion/include/value_at.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

//forward declaration of class - make it a friend in DidlObject.
namespace squawk {
class UpnpContentDirectoryDao;
}

/**
 * \brief The DIDL Model
 */
namespace didl {

enum DIDL_CLASS {
    object = 0,
	objectContainer = 1,
    objectContainerAlbumMusicAlbum = 2,
	objectContainerPersonMusicArtist = 3,
	objectItem = 4,
    objectItemAudioItemMusicTrack = 5,
    objectItemImageItemPhoto = 6,
    objectItemVideoItemMovie = 7,
    objectItemEBook = 8,
    objectContainerAlbumPhotoAlbum = 9
};

const std::array<std::string, 10> DIDL_CLASS_NAMES {{
    "object",       "object.container", "object.container.album.musicAlbum",    "object.container.person.musicArtist",  "object.item",
    "object.item.audioItem.musicTrack", "object.item.imageItem.photo",          "object.item.videoItem.movie",          "object.item.book.ebook",
    "object.container.album.photoAlbum"
}};

/* \brief Get the class name as string */
inline std::string className ( DIDL_CLASS cls ) {
    return DIDL_CLASS_NAMES.at( cls );
}

/** \brief  File Statistics  */
struct DidlStatistics {
public:
    DidlStatistics( const int & albums_count, const int & artist_count, const std::map< std::string, int> & audiofile_types, const std::map< std::string, int > & file_types ) :
        _albums_count(albums_count), _artist_count(artist_count), _audiofile_types( audiofile_types ), _file_types( file_types ) {}

//    /** \brief the albums count */
//    int albumsCount() const { return _albums_count; }
//    /** \brief the artist count */
//    int artistCount() const { return _artist_count; }
//    /** \brief the number of total files. */
//    std::map< std::string, int> audiofileTypes() const { return _audiofile_types; }
//    /** \brief number of files for type */
//    std::map< std::string, int > fileTypes() const { return _file_types; }

    friend class squawk::UpnpContentDirectoryDao;
    friend std::ostream& operator<< ( std::ostream & os, const DidlStatistics & o );
    friend class boost::fusion::extension::access;

private:
    int _albums_count, _artist_count;
    std::map< std::string, int> _audiofile_types;
    std::map< std::string, int> _file_types;
};

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

        friend std::ostream& operator<< ( std::ostream & os, const DidlAlbumArtUri & o );
        friend std::ostream& operator<< ( std::ostream & os, const std::list< didl::DidlAlbumArtUri > & o );

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
        /** \brief playback time of the media in seconds */ duration = 0,
        /** \brief the bitrate of the media */ bitrate = 1,
        /** \brief bit per sample */ bitsPerSample = 2,
        /** \brief sample frequency of the media */ sampleFrequency = 3,
        /** \brief the number of channels of the media */ nrAudioChannels = 4,
        /** \brief the resolution of the media XxY (320x200) */ resolution = 5,
        /** \brief color depth of the media */ colorDepth = 6,
        /** \brief frame rate of the video */ framerate = 7 };

    const std::array<std::string, 8> UPNP_RES_ATTRIBUES_NAMES {{
        "duration",     "bitrate",      "bitsPerSample",    "sampleFrequency",
        "nrAudioChannels",  "resolution",   "colorDepth",   "framerate"
    }};

    DidlResource ( const size_t & id, const size_t & ref_id, const size_t & size,
                   const std::string & uri, const std::string & path, const std::string & protocol_info,
                   const std::string & dlna_profile, const std::string & mime_type,
                   const std::map< UPNP_RES_ATTRIBUTES, std::string > & attributes ) :
        _id(id), _ref_obj(ref_id), _size(size), _uri ( uri ), _path(path), _protocol_info ( protocol_info ),
        _dlna_profile(dlna_profile), _mime_type(mime_type), _attributes ( attributes ) {}


    /** \brief the id this resource */
    size_t id() const { return _id; }
    /** \brief the reference to the ObjectItem */
    size_t refObj() const { return _ref_obj; }
    /** \brief the size of the file */
    size_t size() const { return _size; }
    /** \brief the uri where this resource is located */
    std::string uri() const { return _uri; }
    /** \brief the path where this resource is located */
    std::string path() const { return _path; }
    /** \brief the dlna protocol info of this resource */
	std::string protocolInfo() const { return _protocol_info; }
    /** \brief the dlna profile info of this resource */
    std::string dlnaProfile() const { return _dlna_profile; }
    /** \brief the mimt-type info of this resource */
    std::string mimeType() const { return _mime_type; }

    /** \brief get the attributes of the resource */
    std::map< UPNP_RES_ATTRIBUTES, std::string > attributes () const { return _attributes; }

    friend class squawk::UpnpContentDirectoryDao;
    friend std::ostream& operator<< ( std::ostream & os, const DidlResource & o );
    friend std::ostream& operator<< ( std::ostream & os, const std::list< didl::DidlResource > & o );

private:
    size_t _id, _ref_obj, _size;
    std::string _uri, _path;
    std::string _protocol_info, _dlna_profile, _mime_type;
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

#define DIDL_ITEM_ATTRIBUTES const size_t size, const std::string & mime_type, const std::list< DidlResource > & item_resource
#define DIDL_ITEM_ATTRiBUTES_NAMES size, mime_type, item_resource
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

struct DidlObject {
public:
    DidlObject() {}
    DidlObject ( DIDL_CLASS cls, DIDL_OBJECT_ATTRIBUTES, const bool import = true ) :
            _cls ( cls ), DIDL_OBJECT_ASSIGNMENTS, _import ( import ) {}

	DidlObject ( const DidlObject& ) = default;
	DidlObject ( DidlObject&& ) = default;
	DidlObject& operator= ( const DidlObject& ) = default;
	DidlObject& operator= ( DidlObject&& ) = default;
    virtual ~DidlObject() {}

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

    DIDL_CLASS _cls = object;
    size_t _id = 0, _parent_id = 0;
    std::string _title = "",  _path = "";
    unsigned long _mtime = 0;
    size_t _object_update_id = 0;
    bool _import = false;
};

struct DidlContainer : public DidlObject {
public:
    DidlContainer() {}
    DidlContainer ( DIDL_OBJECT_ATTRIBUTES, DIDL_CONTAINER_ATTRIBUTES, const bool import = true ) :
                DidlObject ( objectContainer, DIDL_OBJECT_ATTRiBUTES_NAMES, import ), _child_count( child_count ) {}

    DidlContainer ( DIDL_CLASS cls, DIDL_OBJECT_ATTRIBUTES, DIDL_CONTAINER_ATTRIBUTES, const bool import = true ) :
                DidlObject ( cls, DIDL_OBJECT_ATTRiBUTES_NAMES, import ), _child_count( child_count ) {}

    DidlContainer ( const DidlContainer& ) = default;
	DidlContainer ( DidlContainer&& ) = default;
	DidlContainer& operator= ( const DidlContainer& ) = default;
	DidlContainer& operator= ( DidlContainer&& ) = default;
    virtual ~DidlContainer() {}

	/** \brief The number of childs for this container */
	size_t childCount() const { return _child_count; }

    friend std::ostream& operator<< ( std::ostream & os, const DidlContainer & o );
    friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;

protected:

    size_t _child_count = 0;
};

struct DidlItem : public DidlObject {
public:
    DidlItem() {}
    DidlItem ( DIDL_CLASS cls, DIDL_OBJECT_ATTRIBUTES, DIDL_ITEM_ATTRIBUTES, const bool import = true ) :
                DidlObject ( cls, DIDL_OBJECT_ATTRiBUTES_NAMES, import ), _size( size ), _mime_type ( mime_type ), _item_resource(item_resource) {}

	DidlItem ( const DidlItem& ) = default;
	DidlItem ( DidlItem&& ) = default;
	DidlItem& operator= ( const DidlItem& ) = default;
	DidlItem& operator= ( DidlItem&& ) = default;
    virtual ~DidlItem() {}

	/** \brief Size of the file represented by this item */
	size_t size() const { return _size; }
	/** \brief  mime-type of this item */
	std::string mimeType() const { return _mime_type; }
    /** \brief The track ressource */
    std::list< DidlResource > audioItemRes() const { return _item_resource; }

    friend std::ostream& operator<< ( std::ostream & os, const DidlItem & o );
    friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;

protected:

    size_t _size = 0;
    std::string _mime_type = "";
    std::list< DidlResource > _item_resource;
};

/* \brief The didl music album */
struct DidlContainerAlbum : public DidlContainer {
public:
    DidlContainerAlbum() {}
    DidlContainerAlbum ( DIDL_OBJECT_ATTRIBUTES, DIDL_CONTAINER_ATTRIBUTES,
						 const size_t & rating, const size_t & year, const size_t playback_count,
                         const std::string & contributor, const std::string & artist, const std::string & genre,
						 std::list< DidlAlbumArtUri > album_art_uri = std::list< DidlAlbumArtUri >(),
						 const bool import = true ) :
		DidlContainer ( objectContainerAlbumMusicAlbum, id, parent_id, title, path, mtime, object_update_id,
                        child_count, import ), _rating (rating ), _year( year ), _playback_count ( playback_count ),
                        _contributor (contributor ), _artist( artist ), _genre( genre ), _album_art_uri( album_art_uri ) {}

	DidlContainerAlbum ( const DidlContainerAlbum& ) = default;
	DidlContainerAlbum ( DidlContainerAlbum&& ) = default;
	DidlContainerAlbum& operator= ( const DidlContainerAlbum& ) = default;
	DidlContainerAlbum& operator= ( DidlContainerAlbum&& ) = default;
    virtual ~DidlContainerAlbum() {}

	/* TODO actor?, creator? */
    /** \brief Album contributor */
    std::string contributor() const { return _contributor; }
	/** \brief Album artist */
	std::string artist() const { return _artist; }
	/* \brief The music genre */
	std::string genre() const { return _genre; }
	/** \brief Release date */
	size_t year () const { return _year; }
	/** \brief number of times this song was played */
	size_t playbackCount() const { return _playback_count; }
	/* TODO LAST PLAYBACK TIME */
	/** \brief User rating of this album */
    size_t rating() const { return _rating; }
	/** \brief The album art object */
	std::list< DidlAlbumArtUri > albumArtUri() const { return _album_art_uri; }

    friend std::ostream& operator<< ( std::ostream & os, const DidlContainerAlbum & o );
    friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;

protected:
    size_t _rating = 0;
    size_t _year = 0;
    size_t _playback_count = 0;
    std::string _contributor = "";
    std::string _artist = "";
    std::string _genre = "";
    std::list< DidlAlbumArtUri > _album_art_uri;
};

struct DidlContainerPhotoAlbum : public DidlContainer {
public:
    DidlContainerPhotoAlbum() {}
    DidlContainerPhotoAlbum( DIDL_OBJECT_ATTRIBUTES, DIDL_CONTAINER_ATTRIBUTES,
                         const size_t & rating, const size_t & year, const size_t playback_count,
                         const std::string & contributor, const std::string & artist, const std::string & genre,
                         std::list< DidlAlbumArtUri > album_art_uri = std::list< DidlAlbumArtUri >(),
                         const bool import = true ) :
        DidlContainer( objectContainerAlbumPhotoAlbum, id, parent_id, title, path, mtime, object_update_id,
                       child_count, import ), _rating (rating ), _year( year ), _playback_count ( playback_count ),
                       _contributor (contributor ), _artist( artist ), _genre( genre ), _album_art_uri( album_art_uri ) {}

    DidlContainerPhotoAlbum( const DidlContainerPhotoAlbum& ) = default;
    DidlContainerPhotoAlbum( DidlContainerPhotoAlbum&& ) = default;
    DidlContainerPhotoAlbum& operator= ( const DidlContainerPhotoAlbum& ) = default;
    DidlContainerPhotoAlbum& operator= ( DidlContainerPhotoAlbum&& ) = default;
    virtual ~DidlContainerPhotoAlbum() {}

    /* TODO actor?, creator? */
    /** \brief Album contributor */
    std::string contributor() const { return _contributor; }
    /** \brief Album artist */
    std::string artist() const { return _artist; }
    /* \brief The music genre */
    std::string genre() const { return _genre; }
    /** \brief Release date */
    size_t year () const { return _year; }
    /** \brief number of times this song was played */
    size_t playbackCount() const { return _playback_count; }
    /* TODO LAST PLAYBACK TIME */
    /** \brief User rating of this album */
    size_t rating() const { return _rating; }
    /** \brief The album art object */
    std::list< DidlAlbumArtUri > albumArtUri() const { return _album_art_uri; }

    friend std::ostream& operator<< ( std::ostream & os, const DidlContainerPhotoAlbum & o );
    friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;

protected:
    size_t _rating = 0;
    size_t _year = 0;
    size_t _playback_count = 0;
    std::string _contributor = "";
    std::string _artist = "";
    std::string _genre = "";
    std::list< DidlAlbumArtUri > _album_art_uri;
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
                        import ), _clean_name(clean_name) {}

	DidlContainerArtist ( const DidlContainerArtist& ) = default;
	DidlContainerArtist ( DidlContainerArtist&& ) = default;
	DidlContainerArtist& operator= ( const DidlContainerArtist& ) = default;
	DidlContainerArtist& operator= ( DidlContainerArtist&& ) = default;
    virtual ~DidlContainerArtist() {}


    /** \brief the normalized artist name */
    std::string cleanName() const { return _clean_name; }

    friend std::ostream& operator<< ( std::ostream & os, const DidlContainerArtist & o );
    friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;

private:
    const std::string _clean_name;
};
struct DidlMusicTrack : public DidlItem {
public:
    DidlMusicTrack() {}
	DidlMusicTrack ( DIDL_OBJECT_ATTRIBUTES,
                     DIDL_ITEM_ATTRIBUTES,
                     const size_t rating, const size_t year, const size_t track, const size_t disc,
                     const size_t playback_count, const std::string & contributor,
                     const std::string & artist, const std::string & genre, const std::string & album,
                     const std::string comment, const unsigned long last_playback_time,
					 const bool import = true ) :        
                DidlItem ( objectItemAudioItemMusicTrack, DIDL_OBJECT_ATTRiBUTES_NAMES, DIDL_ITEM_ATTRiBUTES_NAMES, import ),
                           _rating(rating), _year(year), _track(track), _disc(disc), _playback_count(playback_count), _contributor(contributor),
                           _artist(artist), _genre(genre), _album(album), _comment(comment), _last_playback_time(last_playback_time) {}

	DidlMusicTrack ( const DidlMusicTrack& ) = default;
	DidlMusicTrack ( DidlMusicTrack&& ) = default;
	DidlMusicTrack& operator= ( const DidlMusicTrack& ) = default;
	DidlMusicTrack& operator= ( DidlMusicTrack&& ) = default;
    virtual ~DidlMusicTrack() {}

    /* \brief The music genre */
    std::string genre() const { return _genre; }

    /** \brief Album name */
    std::string album() const { return _album; }
    /** \brief comment */
    std::string comment() const { return _comment; }
    /** \brief Album contributor */
    std::string contributor() const { return _contributor; }
	/** \brief Album artist */
	std::string artist() const { return _artist; }
	/** \brief Release date */
	size_t year () const { return _year; }
	/** \brief Track on medium */
	size_t track() const { return _track; }
    /** \brief disc number */
    size_t disc() const { return _disc; }
    /** \brief number of times this song was played */
	size_t playbackCount() const { return _playback_count; }
	/** \brief number of times this song was played */
	unsigned long lastPlaybackTime() const { return _last_playback_time; }
	/** \brief User rating of this album */
	size_t rating() const { return _rating; }

    friend std::ostream& operator<< ( std::ostream & os, const DidlMusicTrack & o );
    friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;

protected:
    size_t _rating = 0, _year = 0, _track = 0, _disc, _playback_count = 0;
    std::string _contributor, _artist, _genre, _album, _comment;
    unsigned long _last_playback_time = 0;
};
struct DidlPhoto: public DidlItem {
public:
    DidlPhoto() {}
    DidlPhoto ( DIDL_OBJECT_ATTRIBUTES, DIDL_ITEM_ATTRIBUTES, const bool import = true ) :
                DidlItem ( objectItemImageItemPhoto, DIDL_OBJECT_ATTRiBUTES_NAMES, DIDL_ITEM_ATTRiBUTES_NAMES, import ) {}

	DidlPhoto ( const DidlPhoto& ) = default;
	DidlPhoto ( DidlPhoto&& ) = default;
	DidlPhoto& operator= ( const DidlPhoto& ) = default;
	DidlPhoto& operator= ( DidlPhoto&& ) = default;
    virtual ~DidlPhoto() {}

    friend std::ostream& operator<< ( std::ostream & os, const DidlPhoto & o );
    friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;
};
struct DidlMovie: public DidlItem {
public:
    DidlMovie() {}
    DidlMovie ( DIDL_OBJECT_ATTRIBUTES, DIDL_ITEM_ATTRIBUTES, const bool import = true ) :
                DidlItem ( objectItemVideoItemMovie, DIDL_OBJECT_ATTRiBUTES_NAMES, DIDL_ITEM_ATTRiBUTES_NAMES, import ) {}

	DidlMovie ( const DidlMovie& ) = default;
	DidlMovie ( DidlMovie&& ) = default;
	DidlMovie& operator= ( const DidlMovie& ) = default;
	DidlMovie& operator= ( DidlMovie&& ) = default;
    virtual ~DidlMovie() {}

    friend std::ostream& operator<< ( std::ostream & os, const DidlMovie & o );
    friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;
};
struct DidlEBook: public DidlItem {
public:
    DidlEBook() {}
    DidlEBook ( DIDL_OBJECT_ATTRIBUTES, DIDL_ITEM_ATTRIBUTES, const std::string & isbn, const bool import = true ) :
                DidlItem ( objectItemVideoItemMovie, DIDL_OBJECT_ATTRiBUTES_NAMES, DIDL_ITEM_ATTRiBUTES_NAMES, import ), _isbn(isbn) {}

    DidlEBook ( const DidlEBook& ) = default;
    DidlEBook ( DidlEBook&& ) = default;
    DidlEBook& operator= ( const DidlEBook& ) = default;
    DidlEBook& operator= ( DidlEBook&& ) = default;
    virtual ~DidlEBook() {}

    friend std::ostream& operator<< ( std::ostream & os, const DidlEBook & o );
    friend class squawk::UpnpContentDirectoryDao;
    friend class boost::fusion::extension::access;

    std::string _isbn = "";
};
} //didl

using namespace boost::fusion;

#define ADAPT_STRUCT_OBJECT     (didl::DIDL_CLASS, _cls) (size_t, _id) (size_t, _parent_id) (std::string, _title) (std::string, _path) (unsigned long,  _mtime) \
                                (size_t, _object_update_id)
#define ADAPT_STRUCT_CONTAINER  ADAPT_STRUCT_OBJECT (size_t, _child_count)
#define ADAPT_STRUCT_ALBUM      ADAPT_STRUCT_CONTAINER (size_t, _rating) (size_t, _year) (size_t, _playback_count) (std::string, _contributor) \
                                (std::string, _artist) (std::string, _genre) (std::list< didl::DidlAlbumArtUri >, _album_art_uri)
#define ADAPT_STRUCT_ARTIST     ADAPT_STRUCT_CONTAINER (std::string, _clean_name)

#define ADAPT_STRUCT_ITEM       ADAPT_STRUCT_OBJECT (size_t, _size) (std::string, _mime_type)
#define ADAPT_STRUCT_TRACK      ADAPT_STRUCT_ITEM (size_t, _rating) (size_t, _year) (size_t, _track) (size_t, _playback_count) (std::string, _contributor) \
                                (std::string, _artist) (std::string, _genre) (std::string, _album) (unsigned long, _last_playback_time) \
                                (std::list< didl::DidlResource >, _item_resource)
#define ADAPT_STRUCT_PHOTO      ADAPT_STRUCT_ITEM (std::list< didl::DidlResource >, _item_resource)
#define ADAPT_STRUCT_EBOOK      ADAPT_STRUCT_ITEM (std::list< didl::DidlResource >, _item_resource) (std::string, _isbn)

BOOST_FUSION_ADAPT_STRUCT( didl::DidlObject, ADAPT_STRUCT_OBJECT (bool, _import) )
BOOST_FUSION_ADAPT_STRUCT( didl::DidlContainer, ADAPT_STRUCT_CONTAINER (bool, _import) )
BOOST_FUSION_ADAPT_STRUCT( didl::DidlContainerAlbum, ADAPT_STRUCT_ALBUM (bool, _import) )
BOOST_FUSION_ADAPT_STRUCT( didl::DidlContainerPhotoAlbum, ADAPT_STRUCT_ALBUM (bool, _import) )
BOOST_FUSION_ADAPT_STRUCT( didl::DidlContainerArtist, ADAPT_STRUCT_ARTIST (bool, _import) )
BOOST_FUSION_ADAPT_STRUCT( didl::DidlItem, ADAPT_STRUCT_ITEM (bool, _import) )
BOOST_FUSION_ADAPT_STRUCT( didl::DidlMusicTrack, ADAPT_STRUCT_TRACK (bool, _import) )
BOOST_FUSION_ADAPT_STRUCT( didl::DidlPhoto, ADAPT_STRUCT_PHOTO (bool, _import) )
BOOST_FUSION_ADAPT_STRUCT( didl::DidlEBook, ADAPT_STRUCT_EBOOK (bool, _import) )

typedef std::map< std::string, int> _types_statistic;
BOOST_FUSION_ADAPT_STRUCT(
    didl::DidlStatistics,
    (int, _albums_count)
    (int, _artist_count)
    (_types_statistic, _audiofile_types)
    (_types_statistic, _file_types) )

BOOST_FUSION_ADAPT_STRUCT( //TODO
    didl::DidlMovie,
    (didl::DIDL_CLASS, _cls)
    (size_t, _id)
    (size_t, _parent_id)
    (std::string, _title)
    (std::string, _path)
    (unsigned long,  _mtime)
    (size_t, _object_update_id)
    (bool, _import) )

/* resolve the type of the didl object */
template <typename T>
struct DidlType {
    static didl::DIDL_CLASS cls() {
        return didl::object;
    }
};
template <>
struct DidlType<didl::DidlContainer> {
    static didl::DIDL_CLASS cls() {
        return didl::objectContainer;
    }
};
template <>
struct DidlType<didl::DidlItem> {
    static didl::DIDL_CLASS cls() {
        return didl::objectItem;
    }
};
template <>
struct DidlType<didl::DidlContainerAlbum> {
    static didl::DIDL_CLASS cls() {
        return didl::objectContainerAlbumMusicAlbum;
    }
};
template <>
struct DidlType<didl::DidlContainerPhotoAlbum> {
    static didl::DIDL_CLASS cls() {
        return didl::objectContainerAlbumPhotoAlbum;
    }
};
template <>
struct DidlType<didl::DidlContainerArtist> {
    static didl::DIDL_CLASS cls() {
        return didl::objectContainerPersonMusicArtist;
    }
};
template <>
struct DidlType<didl::DidlMusicTrack> {
    static didl::DIDL_CLASS cls() {
        return didl::objectItemAudioItemMusicTrack;
    }
};
template <>
struct DidlType<didl::DidlMovie> {
    static didl::DIDL_CLASS cls() {
        return didl::objectItemVideoItemMovie;
    }
};
template <>
struct DidlType<didl::DidlPhoto> {
    static didl::DIDL_CLASS cls() {
        return didl::objectItemImageItemPhoto;
    }
};
template <>
struct DidlType<didl::DidlEBook> {
    static didl::DIDL_CLASS cls() {
        return didl::objectItemEBook;
    }
};


/* add comma to the stream */
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
#endif // DIDL_H
