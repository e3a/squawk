#ifndef UPNP_H
#define UPNP_H

#include <map>
#include <ostream>
#include <string>

#include <xml.h>

#define SOAP_NS "http://schemas.xmlsoap.org/soap/envelope/"
#define UPNP_DS_NS "urn:schemas-upnp-org:service:ContentDirectory:1"
#define UPNP_COMMAND_BROWSE "Browse"
#define UPNP_CDS_BROWSE_FLAG "BrowseFlag"
#define UPNP_CDS_BROWSE_FLAG_METADATA "BrowseMetadata"
#define UPNP_CDS_BROWSE_FLAG_DIRECT_CHILDREN "BrowseDirectChildren"

namespace commons {
/**
 * \brief namespace for the upnp utilities.
 */
namespace upnp {

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
  explicit UpnpException (int _code, std::string _what) throw() : _code(_code), _what(_what) {};
  virtual ~UpnpException() throw() {};
  virtual const char* what() const throw() {
return _what.c_str();
  };
  int code() throw() {
return _code;
  }
private:
  int _code;
  std::string _what;
};

class UpnpContentDirectoryRequest {
public:
        enum TYPE { BROWSE };
        UpnpContentDirectoryRequest( TYPE type ) : type( type ) {};
        const TYPE type;
        void addValue( std::string key, std::string value ) {
            props[ key ] = value;
        };
        std::vector< std::string > getNames() {
            std::vector< std::string > names;
            for( const auto &iter : props ) {
                names.push_back( iter.first );
            }
            return names;
        };
        std::string getValue( std::string key ) {
            return props[ key ];
        };
        bool contains( std::string key ) {
            return( props.find( key ) != props.end() );
        };
        bool contains( std::string key, std::string value ) {
            if( contains( key ) ) {
                return props[ key ] == value;
            }
            return false;
        };
    friend std::ostream& operator <<(std::ostream &os,const UpnpContentDirectoryRequest &obj) {
        os <<  "UpnpContentDirectoryRequest::Browse\n";
        for( const auto &iter : obj.props ) {
            os << "\t" << iter.first << " = " << iter.second << "\n";
        }
        return os;
    };

private:
        std::map< std::string, std::string > props;
};

inline UpnpContentDirectoryRequest parseRequest( std::string request ) {
    commons::xml::XMLReader reader( request );
    std::vector< commons::xml::Node > root_node = reader.getElementsByName( SOAP_NS, "Body" );
    if( root_node.size() == 1 ) {
        std::vector< commons::xml::Node > upnp_command = root_node[0].children();
        if( upnp_command.size() == 1 ) {
            if( upnp_command[0].ns() == UPNP_DS_NS && upnp_command[0].name() == UPNP_COMMAND_BROWSE ) {
                UpnpContentDirectoryRequest request( UpnpContentDirectoryRequest::BROWSE );
                std::vector< commons::xml::Node > upnp_props = upnp_command[0].children();
                for( auto prop : upnp_props ) {
                    request.addValue( prop.name(), prop.content() );
                }
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

/**
 * Content Directory Module
 */
class ContentDirectoryModule {
  public:
    virtual std::string getRootNode() = 0;
    virtual bool match( UpnpContentDirectoryRequest parseRequest ) = 0;
    virtual std::string parseNode( UpnpContentDirectoryRequest parseRequest ) = 0;
};

}}

#endif // UPNP_H
