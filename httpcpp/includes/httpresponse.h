/*
    http response header
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

#ifndef HTTPRESPONSE
#define HTTPRESPONSE

namespace http {
/**
 * @brief The HttpResponse class
 */
class HttpResponse {
public:
	HttpResponse() : protocol_ ( "" ) {
		std::cout << "create response" << std::endl;
	}
	~HttpResponse() {
		std::cout << "delete response" << std::endl;

		if ( body_istream ) {
			delete body_istream; //TODO unique pointer
		}
	};

	/**
	 * @brief Set the protocol.
	 * @param protocol
	 */
	void protocol ( const std::string & protocol );
	/**
	 * @brief Get the protocol.
	 * @return
	 */
	std::string protocol();

	size_t size() {
		return size_;
	}
	http_status status() {
		return status_;
	}
	void status ( http_status status ) {
		status_ = status;
	}
	/**
	 * @brief Set a parameter.
	 * @param key
	 * @param value
	 */
	void parameter ( const std::string & key, const std::string & value );
	/**
	 * @brief Get a parameter.
	 * @param key
	 * @return
	 */
	std::string parameter ( const std::string & key );
	/**
	 * @brief Check if response contains parameter.
	 * @param key
	 * @return
	 */
	bool containsParameter ( const std::string & key );
	/**
	 * @brief reset the HttpResponse.
	 * Delete the Stream and Header Map.
	 */
	void reset();
	/**
	 * @brief set expires in seconds.
	 * @param seconds
	 * Set the expire time in seconds. if not set no header will be added.
	 */
	void set_expires ( int seconds );
	/**
	 * @brief set last modified date
	 * @param time
	 * Last modified date. if not set no header will be added.
	 */
	void set_last_modified ( time_t last_modified );
	/**
	 * @brief set_mime_type
	 * @param type
	 */
	void set_mime_type ( mime::MIME_TYPE type );

	short http_version_major;
	short http_version_minor;
	std::string remote_ip;

	void set_content ( std::istream & is );
	void produce ( std::array< char, 8192 > buffer, size_t size_ );

	size_t fill_buffer ( char * buffer, size_t buffer_size );
	void set_istream ( std::istream * is );

	/**
	 * @brief get the buffered body.
	 * @return
	 */
	std::string body();

	//DEPRECATED
	std::string get_message_header();

	HttpResponse & operator<< ( const std::string & str );

	friend std::ostream& operator<< ( std::ostream& out, const http::HttpResponse & response ) {
		out << response.protocol_ << "/" << response.http_version_major << "." << response.http_version_minor << " " << parse_status ( response.status_ ) << "\n";
		out << "ResponseLines:\n";

		for ( auto response_line : response.parameters_ ) {
			out << "\t" << response_line.first << ": " << response_line.second << "\n";
		}

		return out;
	}


private:
	std::string protocol_;
	http_status status_;
	std::map<std::string, std::string> parameters_;
	size_t size_ = 0;
	std::stringstream body_stream;
	int seconds = 0;
	time_t last_modified;
	mime::MIME_TYPE type = mime::MIME_TYPE::TEXT; //TODO octed bla bla
	static std::string to_string ( http_status status_ );
	std::istream * body_istream = nullptr;
};
typedef std::unique_ptr<http::HttpResponse> response_ptr;

} //http

#endif // HTTPRESPONSE
