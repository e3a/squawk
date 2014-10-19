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
 * 1 error: could not parse file.
 * 2 unable to allocate XML buffer memory
 * 3 error creating XML writer
 * 4 error at start XML document
 * 5
 *
 */
class XmlException : public std::exception {
public:
  explicit XmlException (int _code, std::string _what) throw() : _code(_code), _what(_what) {};
  virtual ~XmlException() throw() {};
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

/**
 * @brief The Attribute class
 */
class Attribute {
public:
    Attribute( xmlNode * node, xmlAttr * attr ) : node(node), attr(attr) {};
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
    Node(xmlNode * node) : node(node) {};
    std::string name() {
        return std::string( (char *) node->name );
    };
    std::string ns() {
        return (char *)node->ns->href;
    };
    std::string content() {
        xmlNode * content_node = node->children;
        if( content_node != NULL && content_node->type == XML_TEXT_NODE ) {
            return (char *) content_node->content;
        } else return "";
    };
    std::vector< Node > children() {
        std::vector< Node > child_nodes;
        for( xmlNode * cnode = node->children; cnode; cnode = cnode->next ) {
            if(cnode->type == XML_ELEMENT_NODE )
                child_nodes.push_back( Node( cnode ) );
        }
        return child_nodes;
    };
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
    XMLReader(std::string xml) {
        doc = xmlReadMemory( xml.c_str(), xml.length(), NULL, NULL, XML_PARSE_RECOVER | XML_PARSE_NOENT );
        if (doc == NULL) {
            throw XmlException( 1, "error: could not parse file." );
        }
        root_element = xmlDocGetRootElement(doc);
    };
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
     * @param content the element content
     * @return new element
     */
    Node element( std::string ns, std::string name, std::string content ) {
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
    Node element( Node parent, std::string ns, std::string name ) {
        return Node( xmlNewChild( parent.node, NULL, BAD_CAST name.c_str(), NULL ) );
    };
    /**
     * @brief element create an element
     * @param parent the parent element
     * @param ns the element namspace
     * @param name the element name
     * @param content the element content
     * @return the new element
     */
    Node element( Node parent, std::string ns, std::string name, std::string content ) {
        if( namespaces.find( ns ) != namespaces.end() ) {
            return Node( xmlNewChild( parent.node, namespaces[ ns ], BAD_CAST name.c_str(), BAD_CAST content.c_str() ) );
        } else {
            return Node( xmlNewChild( parent.node, NULL, BAD_CAST name.c_str(), BAD_CAST content.c_str() ) );
        }
    };
    /**
     * @brief attribute add attribute to an element.
     * @param parent the parent element
     * @param name the attribute name
     * @param content the attribute content
     */
    void attribute( Node parent, std::string name, std::string content ) {
        xmlNewProp( parent.node, BAD_CAST name.c_str(), BAD_CAST content.c_str() );
    };
    /**
     * Create a new namespace. For default namespace create the namespace with an empty prefix ("").
     * @brief ns new namepace.
     * @param parent the parent node
     * @param href the namespace uri
     * @param prefix the namespace prefix
     * @param assign assign namespace to parent node (default false)
     */
    void ns( Node parent, std::string href, std::string prefix, bool assign = false ) {
        xmlNsPtr ns = xmlNewNs( parent.node, BAD_CAST href.c_str(), ( prefix.size() == 0 ? NULL : BAD_CAST prefix.c_str() ) );
        namespaces[ href ] = ns;
        if( assign ) {
            xmlSetNs( parent.node, ns );
        }
    }
    /**
     * Output the XML as string.
     * @brief to string
     * @return
     */
    std::string str() {
        std::string out;
        xmlChar *s;
        int size;
        xmlDocDumpMemory(doc, &s, &size);
        if (s == NULL)
            throw std::bad_alloc();
        try {
            out = (char *)s;
        } catch (...) {
            xmlFree(s);
            throw;
        }
        xmlFree( s );
        return out;
    };
private:
    xmlDocPtr doc = NULL;
    std::map< std::string, xmlNsPtr> namespaces;
};
}}

#endif // XML_H
