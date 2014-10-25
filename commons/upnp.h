#ifndef UPNP_H
#define UPNP_H

#include <map>
#include <ostream>
#include <string>

#include <xml.h>

/* #define SOAP_NS "http://schemas.xmlsoap.org/soap/envelope/"
#define UPNP_DS_NS "urn:schemas-upnp-org:service:ContentDirectory:1" */
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
 * @brief XML SOAP Namespace
 */
const static std::string XML_NS_SOAP = "http://schemas.xmlsoap.org/soap/envelope/";
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
 * @brief XML PURL NAMESPACE
 */
const static std::string XML_NS_PURL = "http://purl.org/dc/elements/1.1/";
/**
 * @brief XML UPNP NAMESPACE
 */
const static std::string XML_NS_UPNP = "urn:schemas-upnp-org:metadata-1-0/upnp/";
/**
 * @brief XML DIDL NAMESPACE
 */
const static std::string XML_NS_DIDL = "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/";
/**
 * @brief XML DLNA NAMESPACE
 */
const static std::string XML_NS_DLNA = "urn:schemas-dlna-org:metadata-1-0/";
/**
 * @brief XML UPNP Content Directory NAMESPACE
 */
const static std::string XML_NS_UPNP_CDS = "urn:schemas-upnp-org:service:ContentDirectory:1";
/**
 * @brief XML PV NAMESPACE
 */
const static std::string XML_NS_PV = "http://www.pv.com/pvns/";
/**
 * @brief XML DLNA Metadata NAMESPACE
 */
const static std::string XML_NS_DLNA_METADATA = "urn:schemas-dlna-org:metadata-1-0/";

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

class CdsResult {
public:
    CdsResult() : number_returned_(0), total_matches_(0), update_id_(0) {}
    int number_returned() {
        return number_returned_;
    }
    void number_returned(int number_returned) {
        number_returned_ = number_returned;
    }
    int total_matches() {
        return total_matches_;
    }
    void total_matches(int total_matches) {
        total_matches_ = total_matches;
    }
    int update_id() {
        return update_id_;
    }
    void update_id(int update_id) {
        update_id_ = update_id;
    }
private:
    int number_returned_;
    int total_matches_;
    int update_id_;
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
        std::string getValue( const std::string & key ) {
            return props[ key ];
        };
        bool contains( const std::string & key ) {
            return( props.find( key ) != props.end() );
        };
        bool contains( const std::string & key, const std::string & value ) {
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
    std::vector< commons::xml::Node > root_node = reader.getElementsByName( XML_NS_SOAP, "Body" );
    if( root_node.size() == 1 ) {
        std::vector< commons::xml::Node > upnp_command = root_node[0].children();
        if( upnp_command.size() == 1 ) {
            if( upnp_command[0].ns() == XML_NS_UPNP_CDS && upnp_command[0].name() == UPNP_COMMAND_BROWSE ) {
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
    virtual void getRootNode( commons::xml::XMLWriter * xmlWriter, CdsResult * cds_result ) = 0;
    virtual bool match( UpnpContentDirectoryRequest * parseRequest ) = 0;
    virtual void parseNode( commons::xml::XMLWriter * xmlWriter, CdsResult * cds_result, UpnpContentDirectoryRequest * parseRequest ) = 0;
};

}}

#endif // UPNP_H
