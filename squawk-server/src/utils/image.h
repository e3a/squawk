/*
    image entity defintion.
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

#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <string>
#include <httpcpp/mimetypes.h>

#include "Imlib2.h"

namespace image {
/**
 * @brief Image class to read and manipulate images.
 */
class Image {
public:
	explicit Image ( const std::string & filename ) : id_ ( 0 ), width_ ( 0 ), height_ ( 0 ), filename_ ( filename ), loaded_ ( false ) {}

	Image ( const Image& ) = delete;
	Image ( Image&& ) = delete;
	Image& operator= ( const Image& ) = delete;
	Image& operator= ( Image&& ) = delete;
	~Image();
	/**
	 * @brief the image id
	 * @return
	 */
	unsigned long id() const;
	/**
	 * @brief set the image id
	 * @param id
	 */
	void id ( unsigned long id );
	/**
	 * @brief the image width
	 * @return width
	 */
	int width();
	/**
	 * @brief the image height
	 * @return height
	 */
	int height();
	/**
	 * @brief the image mime-type
	 * @return mime-type
	 */
	std::string mimeType();
    /**
     * @brief the image color depth
     * @return color depth
     */
    std::string colorDepth();
    /**
	 * @brief write new scaled image
	 * @param width new image width
	 * @param height new image height
	 * @param outfile the target filename
	 */
	void scale ( const int & width, const int & height, const std::string & outfile );

private:
	unsigned long id_;
	int width_, height_;
	http::mime::MIME_TYPE mime_type_;
	std::string filename_;
	bool loaded_;
	Imlib_Image image_;
	void init_();
};
} // image
#endif // IMAGE_H
