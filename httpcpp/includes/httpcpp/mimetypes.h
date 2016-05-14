/*
    Datastructures and utility classes for the Mime Types.
    Copyright (C) 2013  <etienne> <e.knecht@netwings.ch>

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

#ifndef MIMETYPES_H
#define MIMETYPES_H

namespace http {

/**
 * MimeType definitions
 */
namespace mime {

enum MIME_TYPE { GIF = 0, HTM = 1, HTML = 2, JPEG = 3, PNG = 4, CRTWO = 5, CSS = 6, JS = 7, MPEG = 8, FLAC = 9, VORBIS = 10, MUSEPACK = 11, JSON = 12, TEXT = 13, XML = 14, AVI = 15,
                                 MP4 = 16, MKV = 17, VIDEOMPEG = 18, MOV = 19, WMV = 20, VTT = 21, PDF=22, TXT=23
			   };

static struct mapping {
	const char* extension;
	const char* mime_type;
} mappings[] = {
	{ "gif", "image/gif" },
	{ "htm", "text/html" },
	{ "html", "text/html" },
	{ "jpg", "image/jpeg" },
        { "png", "image/png" },
        { "cr2", "image/x-canon-cr2" },
        { "css", "text/css" },
	{ "js", "text/javascript" },
	{ "mp3", "audio/mpeg" },
	{ "flac", "audio/x-flac" },
	{ "ogg", "audio/vorbis" },
	{ "mpc", "audio/musepack" },
	{ "json", "text/json" },
	{ "txt", "text/plain" },
	{ "xml", "text/xml" },
	{ "avi", "video/x-msvideo" },
	{ "mp4", "video/mp4" },
	{ "mkv", "video/x-matroska" },
        { "mpeg", "video/mpeg" },
        { "mov", "video/quicktime" },
        { "wmv", "video/x-ms-wmv" },
        { "vtt", "text/vtt" },
        { "pdf", "application/pdf" },
        { "txt", "text/plain" },
        { 0, 0 }
};

/**
 * @brief mime-type by extension.
 * @param extension
 * @return
 */
inline MIME_TYPE mime_type ( const std::string & extension ) {
        int position_ = 0;
        std::string clean_ext_ = extension;
        if( clean_ext_ == "jpeg" ) clean_ext_ = "jpg";
        for ( mapping * m = mappings; m->extension; ++m ) {
                if ( m->extension == clean_ext_ ) {
                        return MIME_TYPE ( position_ );
		}
                position_++;
	}
	return TEXT;
}
/**
 * @brief mime type for enum type.
 */
inline std::string mime_type ( MIME_TYPE mime_type ) {
	return mappings[mime_type].mime_type;
}
/**
 * @brief get extension by mime type.
 */
inline std::string extension ( MIME_TYPE mime_type ) {
	return mappings[mime_type].extension;
}
/**
 * @brief get extension by mime type string.
 */
inline std::string extension ( const std::string & mime_type ) {
	int position = 0;
	MIME_TYPE type = TEXT;

	for ( mapping * m = mappings; m->extension; ++m ) {
		if ( m->mime_type == mime_type ) {
			type = MIME_TYPE ( position );
		}

		position++;
	}

	return mappings[type].extension;
}
}
}
#endif // MIME_TYPES_H
