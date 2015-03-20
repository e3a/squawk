#ifndef IMAGE_IMLIB_H
#define IMAGE_IMLIB_H

#include <iostream>
#include <string>
#include <mimetypes.h>

#include "Imlib2.h"

/**
 *\brief common utilities.
 */
namespace commons {
/**
 * \brief namespace for the image utilities.
 */
namespace image {
/**
 * @brief Image class to read and manipulate images.
 */
class Image {
public:
    Image( const std::string & filename ) : _filename(filename), loaded(false) {}
    ~Image() {
        if( loaded ) imlib_free_image();
    }
    /**
     * @brief id the image id
     * @return
     */
    unsigned long id() {
        return _id;
    }
    void id( unsigned long id ) {
        _id = id;
    }
    /**
     * @brief the image width
     * @return width
     */
    int width() {
        if( !loaded ) init();
        return imlib_image_get_height();
    }
    /**
     * @brief the image height
     * @return height
     */
    int height() {
        if( !loaded ) init();
        return imlib_image_get_height();
    }
    /**
     * @brief the image mime-type
     * @return mime-type
     */
    std::string mimeType() {
        if( !loaded ) init();
        char * format = imlib_image_format();
        std::cout << "format:" << format;
        return format;
    }
    /**
     * @brief write new scaled image
     * @param width new image width
     * @param height new image height
     * @param outfile the target filename
     */
    void scale(const int & width, const int & height, const std::string & outfile) {
        if( !loaded ) init();
        int x, y;
        if(imlib_image_get_width() > imlib_image_get_height()) {
            double ratio = (double)width / (double)imlib_image_get_width();
            x = (double)imlib_image_get_width() * ratio;
            y = (double)imlib_image_get_height() * ratio;
        } else {
            double ratio = (double)height / (double)imlib_image_get_height();
            x = (double)imlib_image_get_width() * ratio;
            y = (double)imlib_image_get_height() * ratio;
        }
        Imlib_Image imlib_thumb = imlib_create_cropped_scaled_image(0, 0, imlib_image_get_width(), imlib_image_get_height(), x, y);
        imlib_context_set_image(imlib_thumb);
        imlib_save_image(outfile.c_str());
        imlib_free_image();
        imlib_context_set_image(image);
    }
private:
    unsigned long _id;
    int _width, _height;
    http::mime::MIME_TYPE _mime_type;
    std::string _filename;
    Imlib_Image image;
    bool loaded;
    void init() {
        image = imlib_load_image(_filename.c_str());
        imlib_context_set_image(image);
        loaded = true;
        if( !image ) {
            std::cerr << "can not load image:" << _filename;
        }

    }
};
}}
#endif // IMAGE_IMLIB_H
