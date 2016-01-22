/*
    asio connection implementation
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

#include "httpconnection.h"

namespace http {
inline namespace asio_impl {

HttpConnection::HttpConnection ( asio::io_service & io_service, http::HttpRequestHandler * httpRequestHandler ) :
    strand_ ( io_service ), socket_ ( io_service ), timer_ ( io_service ), httpRequestHandler_ ( httpRequestHandler ),
	httpResponse_ ( http::response_ptr ( new http::HttpResponse() ) ) {
}
HttpConnection::~HttpConnection() {
    if ( socket_.is_open() ) {
		socket_.close();
	}
}

asio::ip::tcp::socket & HttpConnection::socket() {
	return socket_;
}

void HttpConnection::start() {
    //TODO timer to close persistent connection */
	/*      timer_.expires_from_now( std::chrono::seconds( 30 ) );
	  timer_.async_wait( std::bind( &connection::timer_expired, shared_from_this(), std::placeholders::_1 ) ); */

    request_.reset( new HttpRequest() );
	socket_.async_read_some ( asio::buffer ( buffer_ ),
                              std::bind ( &HttpConnection::handle_read_header, shared_from_this(),
										  std::placeholders::_1,
										  std::placeholders::_2 ) );
}

void HttpConnection::handle_read_header ( const asio::error_code& e, std::size_t bytes_transferred ) {
	if ( !e ) {
        size_t result = http_parser_.parse_http_request ( request_.get(), buffer_, bytes_transferred );
        request_->remoteIp( socket_.remote_endpoint().address().to_string() );

		if ( result > 0 ) {

			if ( result < BUFFER_SIZE && result < bytes_transferred ) {

				//copy the body
                request_->content ( buffer_, result, ( bytes_transferred - result ) );
			}

            if ( request_->containsParameter ( header::CONTENT_LENGTH ) &&
                    request_->requestBody().size() < std::stoul ( request_->parameter ( header::CONTENT_LENGTH ) ) ) {

				socket_.async_read_some ( asio::buffer ( buffer_ ), strand_.wrap (
                                              std::bind ( &HttpConnection::handle_read_body, shared_from_this(),
													  std::placeholders::_1,
													  std::placeholders::_2 ) ) );

			} else {

                httpRequestHandler_->handle_request (
                    *request_.get(), *httpResponse_.get(),  std::function<void() > ( std::bind ( &HttpConnection::send_response, shared_from_this() ) ) );
			}

		} else {
			//read the rest of the headers
			socket_.async_read_some ( asio::buffer ( buffer_ ),
                                      std::bind ( &HttpConnection::handle_read_header, shared_from_this(),
												  std::placeholders::_1,
												  std::placeholders::_2 ) );
		}

    } else { http_parser_.reset(); } //on error
}
void HttpConnection::handle_read_body ( const asio::error_code & e, std::size_t bytes_transferred ) {

	if ( !e ) {
		body_read += bytes_transferred;
        (*request_) << std::string ( buffer_.data(), bytes_transferred );

        if ( std::stoul ( request_->parameter ( header::CONTENT_LENGTH ) ) > body_read ) {

			socket_.async_read_some ( asio::buffer ( buffer_ ), strand_.wrap (
                                          std::bind ( &HttpConnection::handle_read_body, shared_from_this(),
												  std::placeholders::_1,
												  std::placeholders::_2 ) ) );

		} else {

            httpRequestHandler_->handle_request (
                *request_.get(), *httpResponse_.get(),  std::function<void() > ( std::bind ( &HttpConnection::send_response, shared_from_this() ) ) );
		}

    } else { http_parser_.reset(); } //on error
}

void HttpConnection::send_response() {
    http_parser_.reset();
	asio::async_write ( socket_, asio::buffer ( httpResponse_->get_message_header() ), strand_.wrap (
                            std::bind ( &HttpConnection::handle_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2 ) ) );
}

void HttpConnection::handle_write ( const asio::error_code & e, int ) {
	if ( !e ) {
		int next_size = httpResponse_->fill_buffer ( buffer_.data(), BUFFER_SIZE );

		if ( next_size > 0 ) {

			//write next chunk
			asio::async_write ( socket_, asio::buffer ( buffer_, next_size ),
								strand_.wrap (
                                    std::bind ( &HttpConnection::handle_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2 ) ) );

        } else if ( request_->isPersistent() ) {

			httpResponse_->reset();
			start();

		} else {
            //TODO std::cout << "handle write: close" << std::endl;
            // Initiate graceful connection closure.
			asio::error_code ignored_ec;
			socket_.shutdown ( asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}

	} else {
		std::cerr << "error in handle_write: " << e.message() << std::endl;
	}
}
void HttpConnection::timer_expired ( const asio::error_code & error ) {
	if ( !error ) {
		std::cout << "timer expired." <<  std::this_thread::get_id() << std::endl;
		socket_.cancel();

	} else {
		std::cout << "timer expired." << error.message() << std::endl;
	}
}
} //asio_impl
} //http
