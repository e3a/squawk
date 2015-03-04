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

  enum MIME_TYPE { GIF=0, HTM=1, HTML=2, JPEG=3, PNG=4, CSS=5, JS=6, MPEG=7, FLAC=8, VORBIS=9, MUSEPACK=10, JSON=11, TEXT=12, XML=13, AVI=14,
                   MP4=15, MKV=16 };

  static struct mapping {
    const char* extension;
    const char* mime_type;
  } mappings[] = {
    { "gif", "image/gif" },
    { "htm", "text/html" },
    { "html", "text/html" },
    { "jpg", "image/jpeg" },
    { "png", "image/png" },
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
    { 0, 0 }
  };

  /**
   * @brief mime-type by extension.
   * @param extension
   * @return
   */
  inline MIME_TYPE mime_type(const std::string & extension) {
    int position = 0;
    for (mapping* m = mappings; m->extension; ++m) {
      if (m->extension == extension) {
            return MIME_TYPE(position);
      }
      position++;
    }
    return TEXT;
  }
  /**
   * @brief mime type for enum type.
   */
  inline std::string mime_type(MIME_TYPE mime_type) {
    return mappings[mime_type].mime_type;
  }
  /**
   * @brief get extension by mime type.
   */
  inline std::string extension( MIME_TYPE mime_type ) {
    return mappings[mime_type].extension;
  }
  /**
   * @brief get extension by mime type string.
   */
  inline std::string extension( const std::string & mime_type ) {
      int position = 0;
      MIME_TYPE type = TEXT;
      for (mapping * m = mappings; m->extension; ++m) {
        if (m->mime_type == mime_type) {
              type = MIME_TYPE(position);
        }
        position++;
      }
    return mappings[type].extension;
  }
}}
#endif // MIME_TYPES_H
