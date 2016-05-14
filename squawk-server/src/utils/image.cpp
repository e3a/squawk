/*
    image entity implementation.
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

#include "image.h"

#include "Imlib2.h"

namespace image {

Image::~Image() {
	if ( loaded_ ) { imlib_free_image(); }
}
void Image::init_() {
	image_ = imlib_load_image ( filename_.c_str() );
	imlib_context_set_image ( image_ );
	loaded_ = true;

	if ( !image_ ) {
        std::cerr << "can not load image:" << filename_ << std::endl;
	}
}
unsigned long Image::id() const {
	return id_;
}
void Image::id ( unsigned long id ) {
	id_ = id;
}
int Image::width() {
	if ( !loaded_ ) { init_(); }

	return imlib_image_get_height();
}
int Image::height() {
	if ( !loaded_ ) { init_(); }

	return imlib_image_get_height();
}
std::string Image::mimeType() {
	if ( !loaded_ ) { init_(); }

	char * format = imlib_image_format();
	return format;
}
std::string Image::colorDepth() {
    if ( !loaded_ ) { init_(); }

//    Imlib_Color_Range format = imlib_context_get_color_range();
    return  "16"; //TODO return range format;
}
void Image::scale ( const int & width, const int & height, const std::string & outfile ) {
	if ( !loaded_ ) { init_(); }

	int x, y;

	if ( imlib_image_get_width() > imlib_image_get_height() ) {
		double ratio = ( double ) width / ( double ) imlib_image_get_width();
		x = ( double ) imlib_image_get_width() * ratio;
		y = ( double ) imlib_image_get_height() * ratio;

	} else {
		double ratio = ( double ) height / ( double ) imlib_image_get_height();
		x = ( double ) imlib_image_get_width() * ratio;
		y = ( double ) imlib_image_get_height() * ratio;
	}

	Imlib_Image imlib_thumb = imlib_create_cropped_scaled_image ( 0, 0, imlib_image_get_width(), imlib_image_get_height(), x, y );
	imlib_context_set_image ( imlib_thumb );
	imlib_save_image ( outfile.c_str() );
	imlib_free_image();
	imlib_context_set_image ( image_ );
}
} // media
