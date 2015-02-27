#ifndef XML_H
#define XML_H

#include <map>
#include <string>
#include <vector>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

/**
 *\brief common utilities.
 */
namespace commons {
/**
 * \brief namespace for the xml parser wraper.
 */
namespace xml {

// TODO xmlCleanupParser();

/**
 * @brief The XmlException class
 *
 * Error Codes:
<ol>
<li>Could not parse file</li>
<li>unable to allocate XML buffer memory</li>
<li>error creating XML writer</li>
<li>error at start XML document</li>
<li>XML namespace uri or prefix is empty.</li>
<li>Unable to add child to parent node.</li>
</ol>
 */
class XmlException : public std::exception {
public:
  explicit XmlException (int _code, std::string _what) throw() : _code(_code), _what(_what) {}
  virtual ~XmlException() throw() {}
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
 * @brief The Attribute class
 */
class Attribute {
public:
    Attribute( xmlNode * node, xmlAttr * attr ) : node(node), attr(attr) {}
    std::string name() {
        return (char *)attr->name;
    }
    std::string content() {
        xmlChar* value = xmlGetProp(node, attr->name);
        std::string re = std::string((char *) value );
        xmlFree(value);
        return re;
    }
private:
    xmlNode * node;
    xmlAttr * attr;
};
/**
 * @brief The Node class
 */
class Node {
friend class XMLWriter;
public:
    Node(xmlNode * node) : node(node) {}
    std::string name() {
        return std::string( (char *) node->name );
    }
    std::string ns() {
        return (char *)node->ns->href;
    }
    std::string content() {
        xmlNode * content_node = node->children;
        if( content_node != NULL && content_node->type == XML_TEXT_NODE ) {
            return (char *) content_node->content;
        } else return "";
    }
    std::vector< Node > children() {
        std::vector< Node > child_nodes;
        for( xmlNode * cnode = node->children; cnode; cnode = cnode->next ) {
            if(cnode->type == XML_ELEMENT_NODE )
                child_nodes.push_back( Node( cnode ) );
        }
        return child_nodes;
    }
    std::vector< Attribute > attributes() {
        std::vector< Attribute > atts;
        for( xmlAttr * cnode = node->properties; cnode; cnode = cnode->next ) {
            if(cnode->type == XML_ATTRIBUTE_NODE )
                atts.push_back( Attribute( node, cnode ) );
        }
        return atts;
    };
private:
    xmlNode * node;
};

/**
 * @brief The XMLReader class
 */
class XMLReader {
public:
    XMLReader(std::string xml) { //TODO open file directly
        doc = xmlReadMemory( xml.c_str(), xml.length(), NULL, NULL, XML_PARSE_RECOVER | XML_PARSE_NOENT );
        if (doc == NULL) {
            throw XmlException( 1, "error: could not parse file." );
        }
        root_element = xmlDocGetRootElement(doc);
    }
    ~XMLReader() {
        if( doc )
            xmlFreeDoc(doc);
    };
    bool next() {
        if( cur_node == NULL )
            cur_node = root_element;
        else
            cur_node = cur_node->next;
        return cur_node;
    }
    Node node() {
        return Node(cur_node);
    };
    std::vector< Node > getElementsByName( std::string name ) {
        std::vector< Node > child_nodes;
        search( name, root_element, child_nodes );
        return child_nodes;
    };
    std::vector< Node > getElementsByName( std::string ns, std::string name ) {
        std::vector< Node > child_nodes;
        search( ns, name, root_element, child_nodes );
        return child_nodes;
    };
private:
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    xmlNode *cur_node = NULL;
    void search( std::string name, xmlNode * _node, std::vector< Node > & list ) {
        xmlNode *cur_node = NULL;
            for (cur_node = _node; cur_node; cur_node = cur_node->next) {
            if (cur_node->type == XML_ELEMENT_NODE && (( char *)cur_node->name) == name) {
                    list.push_back( Node( cur_node ) );
                }
                search( name, cur_node->children, list );
            }
    };
    void search( std::string ns, std::string name, xmlNode * _node, std::vector< Node > & list ) {
        xmlNode *cur_node = NULL;
            for (cur_node = _node; cur_node; cur_node = cur_node->next) {
            if (cur_node->type == XML_ELEMENT_NODE && cur_node->ns != NULL && (( char *)cur_node->ns->href) == ns && (( char *)cur_node->name) == name) {
                    list.push_back( Node( cur_node ) );
                }
                search( ns, name, cur_node->children, list );
            }
    };
};
/**
The XML writer.
Create a XML tree and write it to a string.

create an empty roo element without content:
<pre>
commons::xml::XMLWriter writer;
commons::xml::Node root_node = writer.element( "", "root", "" );
</pre>

add an empty element with an attribute:
<pre>
commons::xml::Node child_node = writer.element( root_node, "", "child" );
writer.attribute( child_node, "att" "content" );
</pre>

get the xml as string:
<pre>
std::string result = writer.str();
</pre>

 * @brief The XMLWriter class
 */
class XMLWriter {
public:
    /**
     * Create a new XMLWriter.
     * @brief XMLWriter
     */
    XMLWriter() {
        doc = xmlNewDoc(BAD_CAST "1.0");
    };
    ~XMLWriter() {
        if( doc )
            xmlFreeDoc(doc);
    };
    /**
     * @brief element create the root node.
     * @param ns the namespace
     * @param name the element name
     * @return new element
     */
    Node element( const std::string & name ) {
        xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST name.c_str() );
        xmlDocSetRootElement(doc, root_node);
        return Node( root_node );
    };
    /**
     * @brief element create an element
     * @param parent the parent element
     * @param ns the element namspace
     * @param name the element name
     * @return new element
     */
    Node element( const Node & parent, const std::string & ns, const std::string & name ) {
        xmlNodePtr node;
        if( namespaces.find( ns ) != namespaces.end() ) {
            node = xmlNewNode( namespaces[ ns ], BAD_CAST name.c_str() );
        } else {
            node = xmlNewNode( NULL, BAD_CAST name.c_str() );
        }
        node = xmlAddChild( parent.node, node );
        if( node == NULL )
            throw XmlException(6, "Unable to add child to parent node.");
        return Node( node );
    };
    /**
     * @brief element create an element
     * @param parent the parent element
     * @param ns the element namspace
     * @param name the element name
     * @param content the element content
     * @return the new element
     */
    Node element( const Node & parent, const std::string & ns, const std::string & name, const std::string & content ) {
        xmlNodePtr node;
        if( namespaces.find( ns ) != namespaces.end() ) {
            node = xmlNewNode( namespaces[ ns ], BAD_CAST name.c_str() );
        } else {
            node = xmlNewNode( NULL, BAD_CAST name.c_str() );
        }
        xmlNodeAddContent( node, BAD_CAST content.c_str() );
        node = xmlAddChild( parent.node, node );
        if( node == NULL )
            throw XmlException(6, "Unable to add child to parent node.");
        return Node( node );
    };
    /**
     * @brief attribute add attribute to an element.
     * @param parent the parent element
     * @param name the attribute name
     * @param content the attribute content
     */
    void attribute( const Node & parent, const std::string & name, const std::string & content ) {
        xmlNewProp( parent.node, BAD_CAST name.c_str(), BAD_CAST content.c_str() );
    };
    /**
     * @brief attribute add attribute to an element.
     * @param parent the parent element
     * @param ns the element namspace
     * @param name the attribute name
     * @param content the attribute content
     */
    void attribute( const Node & parent, const std::string & ns, const std::string & name, const std::string & content ) {
        if( namespaces.find( ns ) != namespaces.end() ) {
            xmlNewNsProp( parent.node, namespaces[ ns ], BAD_CAST name.c_str(), BAD_CAST content.c_str() );
        } else {
            xmlNewProp( parent.node, BAD_CAST name.c_str(), BAD_CAST content.c_str() );
        }
    };
    /**
     * Create a default namespace.
     * @brief ns new default namepace.
     * @param parent the parent node
     * @param href the namespace uri
     */
    void ns( const Node & parent, const std::string & href ) {
        xmlNsPtr ns = xmlNewNs( parent.node, BAD_CAST href.c_str(), NULL );
        xmlSetNs( parent.node, ns );
    }
    /**
     * Create a new namespace.
     * @brief ns new namepace.
     * @param parent the parent node
     * @param href the namespace uri
     * @param prefix the namespace prefix
     * @param assign assign namespace to parent node (default false)
     */
    void ns( const Node & parent, const std::string & href, const std::string & prefix, bool assign = false ) {
        if( href.size() == 0 || prefix.size() == 0 ) {
            throw XmlException( 5, "XML namespace uri or prefix is empty." );
        }
        //TODO remove default namespace
        xmlNsPtr ns = xmlNewNs( parent.node, BAD_CAST href.c_str(), ( prefix.size() == 0 ? NULL : BAD_CAST prefix.c_str() ) );
        namespaces[ href ] = ns;
        if( assign ) {
            xmlSetNs( parent.node, ns );
        }
    }
    /**
     * Output the XML as string with encoding.
     * @brief to string
     * @param encoding the text encoding
     * @return
     */
    std::string str( std::string encoding ) {
        std::string out;
        xmlChar *s;
        int size;
        xmlDocDumpMemoryEnc( doc, &s, &size, encoding.c_str() );
        if ( s == NULL )
            throw std::bad_alloc();
        try {
            out =  (char *)s;
        } catch ( ... ) {
            xmlFree( s );
            throw;
        }
        xmlFree( s );
        return out;
    };
    /**
     * Output the XML as string.
     * @brief to string
     * @return
     */
    std::string str( bool prettyPrint = false ) {
        std::string out;
        xmlChar *s;
        int size;
        xmlDocDumpFormatMemory( doc, &s, &size, prettyPrint ); //TODO use DEBUG variable
        if ( s == NULL )
            throw std::bad_alloc();
        try {
            out =  (char *)s;
        } catch ( ... ) {
            xmlFree( s );
            throw;
        }
        xmlFree( s );
        return out;
    }
    /**
     * @brief Save the XML to String.
     * @return
     */
    void write( const std::string & filename, bool prettyPrint = false ) {
        FILE *file = fopen( filename.c_str(), "w" );
        xmlDocFormatDump( file, doc, prettyPrint );
        fclose( file );
    }
private:
    xmlDocPtr doc = NULL;
    std::map< std::string, xmlNsPtr> namespaces;
};
}}

#endif // XML_H
