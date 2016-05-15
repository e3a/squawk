/*
    http request header
    Copyright (C) 2014  <e.knecht@netwings.ch>

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

#ifndef HTTPREQUEST
#define HTTPREQUEST

#include <vector>

namespace http {

/**
 * @brief Http Request class.
 */
class HttpRequest {
public:
	HttpRequest();
	HttpRequest ( const std::string & path );

	/**
	 * @brief Set the request method.
	 * @param method
	 */
	void method ( const std::string & method );
	/**
	 * @brief Get the request method.
	 * @return
	 */
	std::string method() const;
	/**
	 * @brief Set the request protocol.
	 * @param protocol
	 */
	void protocol ( const std::string & protocol );
	/**
	 * @brief Get the request protocol.
	 * @return
	 */
	std::string protocol() const;
	/**
	 * @brief Get the request path.
	 * @return
	 */
	std::string uri() const;
	/**
	 * @brief Set the request path.
	 * @param uri
	 */
	void uri ( const std::string & uri );
	/**
	 * @brief Set the http major version.
	 * @param http_version_major
	 */
	void httpVersionMajor ( const int & http_version_major );
	/**
	 * @brief Get the http major version.
	 * @return
	 */
	int httpVersionMajor() const;
	/**
	 * @brief Set the http minor version.
	 * @param http_version_minor
	 */
	void httpVersionMinor ( const int & http_version_minor );
	/**
	 * @brief Get the http minor version.
	 * @return
	 */
	int httpVersionMinor() const;
	/**
	 * @brief Get the remote IP.
	 * @return
	 */
	std::string remoteIp() const;
	/**
	 * @brief Set the remote IP.
	 * @param remote_ip
	 */
	void remoteIp ( const std::string & remote_ip );
	/**
	 * @brief Set the request body.
	 * @param in
	 */
	void requestBody ( const size_t & size, std::shared_ptr< std::istream > request_body );
	/**
	 * @brief Get the request body.
	 * @return
	 */
	const std::string requestBody() const;
	/**
	 * @brief Persistent connection.
	 * @return
	 */
	bool isPersistent();
	/**
	 * @brief Set persistend connection.
	 * @param persistent
	 * @return
	 */
//TODO	void setPersistend ( bool persistent );
	/**
	 * @brief Set a request parameter.
	 * @param name
	 * @param value
	 */
	void parameter ( const std::string & name, const std::string & value );
	/**
	 * @brief Get a request paremeter
	 * @param name
	 * @return
	 */
	std::string parameter ( const std::string & name );
	/**
	 * @brief Contains request parameter by key.
	 * @param name
	 * @return
	 */
	bool containsParameter ( const std::string & name );
	/**
	 * @brief Get the request parameter names.
	 * @return
	 */
	std::vector<std::string> parameterNames();
	/**
	 * @brief Get a copy of the request parameter map.
	 * @return
	 */
	std::map< std::string, std::string > parameterMap();
	/**
	 * @brief Set a request attribute.
	 * @param name
	 * @param value
	 */
	void attribute ( const std::string & name, const std::string & value );
	/**
	 * @brief Get a request attribute
	 * @param name
	 * @return
	 */
	std::string attribute ( const std::string & name );
	/**
	 * @brief Contains request attribute by key.
	 * @param name
	 * @return
	 */
	bool containsAttribute ( const std::string & name );
	/**
	 * @brief Get the request attribute names.
	 * @return
	 */
        std::list<std::string> attributeNames();
	/**
	 * @brief Get a copy of the request parameter map.
	 * @return
	 */
	std::map< std::string, std::string > attributeMap();
	/**
	* @brief Get body size.
	 * @return
	 */
	size_t bodySize() {
		return body_size_;
	}

	std::istream * outBody() { //TODO remove
		return out_body_.get();
	}

	/**
	 * @brief operator <<
	 * @param in
	 */
	void operator<< ( const std::string & in );

	void content ( std::array< char, 8192 > & body, const size_t & index, const size_t & count );

	/**
	 * @brief output the request as string.
	 * @param out
	 * @param request
	 * @return
	 */
	friend std::ostream& operator<< ( std::ostream& out, const http::HttpRequest & request );

private:
	friend class HttpClient; //TODO remove
	friend class HttpClientConnection; //TODO remove

	std::string method_, uri_,  protocol_, remote_ip_;
	size_t body_size_;
	int http_version_major_, http_version_minor_;
	std::map< std::string, std::string > parameters_;
	std::map< std::string, std::string > attributes_;

	std::shared_ptr< std::istream > out_body_;
};

} //http
#endif // HTTPREQUEST

