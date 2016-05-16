#ifndef UPNP2
#define UPNP2

#include <sstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "didl.h"
#include "ssdp.h"
#include "xml.h"

namespace upnp {
    /**
     * @brief XML SOAP Namespace
     */
    const static std::string XML_NS_SOAP = "http://schemas.xmlsoap.org/soap/envelope/";
    /**
     * @brief XML UPNP Content Directory NAMESPACE
     */
    const static std::string XML_NS_UPNP_CDS = "urn:schemas-upnp-org:service:ContentDirectory:1";
    /**
     * @brief XML UPNP Connection Manager NAMESPACE
     */
    const static std::string XML_NS_UPNP_CMS = "urn:schemas-upnp-org:service:ConnectionManager:1";

    /** @brief upnp:class:object.container */
    const static std::string UPNP_CLASS_CONTAINER = "object.container";
    /** @brief upnp:class:object.container.album.musicAlbum */
    const static std::string UPNP_CLASS_MUSIC_ALBUM = "object.container.album.musicAlbum";
    /** @brief upnp:class:object.container.person.musicArtist */
    const static std::string UPNP_CLASS_MUSIC_ARTIST = "object.container.person.musicArtist";
    /** @brief upnp:class:object.container.person.musicArtist */
    const static std::string UPNP_CLASS_MUSIC_TRACK = "object.item.audioItem.musicTrack";

    /** @brief Object ID */
    const static std::string OBJECT_ID = "ObjectID";
    /** @brief Start Index Parameter */
    const static  std::string START_INDEX = "StartingIndex";
    /** @brief Requested Count Parameter */
    const static std::string REQUESTED_COUNT = "RequestedCount";

    /**
     * @brief XML SOAP Encoding Namespace
     */
    const static std::string XML_NS_SOAPENC = "http://schemas.xmlsoap.org/soap/encoding/";
    /**
     * @brief XML Schema Namespace
     */
    const static std::string XML_NS_SCHEMA = "http://www.w3.org/2001/XMLSchema";
    /**
     * @brief XML Schema Instance Namespace
     */
    const static std::string XML_NS_SCHEMA_INSTANCE = "http://www.w3.org/2001/XMLSchema-instance";


/**
 * @brief The UpnpException class
 *
 * Error Codes:
 * 1 Can not parse Upnp SOAP Envelope
 * 2 Unknown UPNP command
 * 3
 * 4
 * 5
 *
 */
class UpnpException : public std::exception {
public:
  explicit UpnpException (int _code, std::string _what) throw() : _code(_code), _what(_what) {}
  virtual ~UpnpException() throw() {}
  virtual const char* what() const throw() {
    return _what.c_str();
  }
  int code() throw() {
return _code;
  }
private:
  int _code;
  std::string _what;
};

/**
 * @brief The UpnpContentDirectoryRequest class
 */
class UpnpContentDirectoryRequest {
public:

    /** The Reqquest Types */
    enum TYPE { BROWSE, PROTOCOL_INFO, X_FEATURE_LIST };
    /**
     * @brief UpnpContentDirectoryRequest
     * @param type
     */
    UpnpContentDirectoryRequest( TYPE type ) : type( type ) {}
    const TYPE type;
    void addValue( std::string key, std::string value ) {
        props[ key ] = value;
    }
    /**
     * @brief get the property names.
     * @return
     */
    std::vector< std::string > getNames() const {
        std::vector< std::string > names;
        for( const auto &iter : props ) {
            names.push_back( iter.first );
        }
        return names;
    }
    /**
     * @brief get property value for a key.
     * @param key
     * @return
     */
    std::string getValue( const std::string & key ) {
        return props[ key ];
    }
    /**
     * @brief test if properties contains a key.
     * @param key
     * @return
     */
    bool contains( const std::string & key ) {
        return( props.find( key ) != props.end() );
    }
    /**
     * @brief test if properties with key and value are equal.
     * @param key
     * @param value
     * @return
     */
    bool contains( const std::string & key, const std::string & value ) {
        if( contains( key ) ) {
            return props[ key ] == value;
        }
        return false;
    }
    /**
     * @brief operator write the properties to the ostream.
     */
    friend std::ostream& operator <<(std::ostream &os,const UpnpContentDirectoryRequest &obj) {
        os <<  "UpnpContentDirectoryRequest::";
        for( const auto &iter : obj.props ) {
            os << "\t" << iter.first << " = " << iter.second << "\n";
        }
        return os;
    }
private:
    std::map< std::string, std::string > props;
};

inline UpnpContentDirectoryRequest parseRequest( std::string request ) {

    commons::xml::XMLReader reader( request );
    std::vector< commons::xml::Node > root_node = reader.getElementsByName( XML_NS_SOAP, "Body" );
    if( root_node.size() == 1 ) {
        std::vector< commons::xml::Node > upnp_command = root_node[0].children();
        if( upnp_command.size() == 1 ) {
            if( upnp_command[0].ns() == XML_NS_UPNP_CDS && upnp_command[0].name() == "Browse" ) {
                UpnpContentDirectoryRequest request( UpnpContentDirectoryRequest::BROWSE );
                std::vector< commons::xml::Node > upnp_props = upnp_command[0].children();
                for( auto prop : upnp_props ) {
                    request.addValue( prop.name(), prop.content() );
                }
                return request;
            } else if( upnp_command[0].ns() == XML_NS_UPNP_CMS && upnp_command[0].name() == "GetProtocolInfo" ) {
                UpnpContentDirectoryRequest request( UpnpContentDirectoryRequest::PROTOCOL_INFO );
                return request;

            } else if( upnp_command[0].ns() == XML_NS_UPNP_CDS && upnp_command[0].name() == "UPNP_COMMAND_FEATURE_LIST" ) {
                UpnpContentDirectoryRequest request( UpnpContentDirectoryRequest::X_FEATURE_LIST );
                return request;

            } else {
                throw UpnpException(1, "unkown upnp command: " +  upnp_command[0].ns() + ":" + upnp_command[0].name() );
            }

        } else {
            throw UpnpException(1, "unkown upnp command." );
        }
    } else {
        throw UpnpException(1, "can not parse upnp SOAP envelope." );
    }
};

inline void parseDescription( const std::string & description, didl::UpnpDevice & device ) {
    commons::xml::XMLReader reader( description );
    commons::xml::Node rootNode = reader.getElementsByName("root")[0];
    for( auto node : rootNode.children() ) {
        if( node.name() == "specVersion" ) {
            for( auto specVersionNode : node.children() ) {
                if( specVersionNode.name() == "major" ) {
                    device.versionMajor( std::stoi( specVersionNode.content() ) );
                } else if( specVersionNode.name() == "minor") {
                    device.versionMinor( std::stoi( specVersionNode.content() ) );
                }
            }
        } else if( node.name() == "device" ) {
            for( auto deviceNode : node.children() ) {
                if( deviceNode.name() == "UDN" ) {
                    device.udn( deviceNode.content() );
                } else if( deviceNode.name() == "UPC" ) {
                    device.upc( deviceNode.content() );
                } else if( deviceNode.name() == "serialNumber" ) {
                    device.serialNumber( deviceNode.content() );
                } else if( deviceNode.name() == "modelNumber" ) {
                    device.modelNumber( deviceNode.content() );
                } else if( deviceNode.name() == "modelName" ) {
                    device.modelName( deviceNode.content() );
                } else if( deviceNode.name() == "modelDescription" ) {
                    device.modelDescription( deviceNode.content() );
                } else if( deviceNode.name() == "modelURL" ) {
                    device.modelUrl( deviceNode.content() );
                } else if( deviceNode.name() == "manufacturerURL" ) {
                    device.manufacturerUrl( deviceNode.content() );
                } else if( deviceNode.name() == "manufacturer" ) {
                    device.manufacturer( deviceNode.content() );
                } else if( deviceNode.name() == "friendlyName" ) {
                    device.friendlyName( deviceNode.content() );
                } else if( deviceNode.name() == "presentationURL" ) {
                    device.presentationUrl( deviceNode.content() );
                } else if( deviceNode.name() == "deviceType" ) {
                    device.deviceType( deviceNode.content() );
                }  else if( deviceNode.name() == "iconList" ) {
                    for( auto iconListNode : deviceNode.children() ) {
                        if( iconListNode.name() == "icon" ) {
                            didl::Icon icon;
                            for( auto iconNode : iconListNode.children() ) {
                                if( iconNode.name() == "mimetype" ) {
                                    icon.mimeType( iconNode.content() );
                                } else if( iconNode.name() == "width" ) {
                                    icon.width( std::stoi( iconNode.content() ) );
                                } else if( iconNode.name() == "height" ) {
                                    icon.height( std::stoi ( iconNode.content() ) );
                                } else if( iconNode.name() == "depth" ) {
                                    icon.depth( std::stoi ( iconNode.content() ) );
                                } else if( iconNode.name() == "url" ) {
                                    icon.url( iconNode.content() );
                                }
                            }
                            device.addIcon( std::move( icon ) );
                        }
                    }
                }  else if( deviceNode.name() == "serviceList" ) {
                    for( auto serviceListNode : deviceNode.children() ) {
                        if( serviceListNode.name() == "service" ) {
                            didl::Service service;
                            for( auto serviceNode : serviceListNode.children() ) {
                                if( serviceNode.name() == "serviceType" ) {
                                    service.serviceType( serviceNode.content() );
                                } else if( serviceNode.name() == "serviceId" ) {
                                    service.serviceId( serviceNode.content() );
                                } else if( serviceNode.name() == "SCPDURL" ) {
                                    service.scpdUrl( serviceNode.content() );
                                } else if( serviceNode.name() == "controlURL" ) {
                                    service.controlUrl( serviceNode.content() );
                                } else if( serviceNode.name() == "eventSubURL" ) {
                                    service.eventSubUrl( serviceNode.content() );
                                }
                            }
                            device.addService( std::move( service ) );
                        }
                    }
                }
            }
        } else if( node.name() == "URLBase" ) {
            //TODO
        } else { std::cout << "? " << node.name() << std::endl; }
    }
};
inline didl::UpnpDevice deviceDescription( const didl::SsdpEvent & event ) {
    didl::UpnpDevice device;
    //make the request
    try {
        curlpp::Cleanup myCleanup;
        curlpp::Easy myRequest;
        std::cout << "get device description:" << event.location() << std::endl;
        myRequest.setOpt<curlpp::options::Url>( event.location() );
        myRequest.setOpt(curlpp::options::ConnectTimeout(2L));

        std::stringstream sstream;
        curlpp::options::WriteStream ws(&sstream);
        myRequest.setOpt(ws);

        myRequest.perform();

        parseDescription( sstream.str(), device );
        return device;

    } catch(curlpp::RuntimeError & e) {
        std::cerr << e.what() << std::endl;
        throw;
    } catch(curlpp::LogicError & e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
};

}//namespace upnp
#endif // UPNP2

