/*
    HTTP Server context definition.
    Copyright (C) 2013  <Etienne Knecht> <e.knecht@newings.ch>

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
#ifndef HTTPSERVLETCONTEXT_H
#define HTTPSERVLETCONTEXT_H

namespace http {
/**
 * @brief The HttpServerContext class
 *
 * The HTTPServerContext holds global variables for the server.
 */
class HttpServletContext {
public:
    HttpServletContext() {}
    HttpServletContext(const std::map< std::string, std::vector< std::string > > map ) {
        parameters_ = map;
    }
    ~HttpServletContext() {}

    /**
     * @brief Set a parameter.
     * @param name
     * @param value
     */
    void parameter( const std::string & name, const std::string & value ) {
        if( parameters_.find( name ) != parameters_.end() ) {

            parameters_[ name ].push_back( value );

        } else {
            std::vector< std::string > list;
            list.push_back( value );
            parameters_[ name ] = list;
         }
    }
    /**
     * @brief Get a paremeter
     * @param name
     * @return
     */
    std::string parameter( const std::string & name ) {
        return parameters_[ name ][0];
    }
    /**
     * @brief Get the paremeters
     * @param name
     * @return
     */
   std::vector< std::string > parameters( const std::string & name ) {
        return parameters_[ name ];
    }
    /**
     * @brief Contains parameter by key.
     * @param name
     * @return
     */
    bool containsParameter( const std::string & name ) const {
        return parameters_.find( name ) != parameters_.end();
    }
    /**
     * @brief Get the parameter names.
     * @return
     */
    std::vector<std::string> parameterNames() const {
        std::vector< std::string > result;

        for ( auto r : parameters_ )
        { result.push_back ( r.first ); }

        return result;
    }
    /**
     * @brief Get a copy of the parameter map.
     * @return
     */
    std::map< std::string, std::vector< std::string > > parametersMap() const {
        return parameters_;
    }

private:
    std::map< std::string, std::vector< std::string > > parameters_;
};
} // http
#endif // HTTPSERVLETCONTEXT_H
