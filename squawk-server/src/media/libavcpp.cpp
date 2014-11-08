/*
    Parse Files and insert to Database

    Copyright (C) 2013  <copyright holder> <email>

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

#include "libavcpp.h"

#include <cstring>

#include "commons.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}

namespace squawk {
namespace media {

log4cxx::LoggerPtr LibAVcpp::logger(log4cxx::Logger::getLogger("squawk.media.LibAVcpp"));

LibAVcpp::LibAVcpp() {
}

char *const get_error_text(const int error) {
    static char error_buffer[255];
    av_strerror(error, error_buffer, sizeof(error_buffer));
    return error_buffer;
}

/** Open an input file and the required decoder. */
static int open_input_file(const char *filename,
                           AVFormatContext **input_format_context,
                           AVCodecContext **input_codec_context) {
    AVCodec *input_codec;
    int error;

    /** Open the input file to read from it. */
    if ((error = avformat_open_input(input_format_context, filename, NULL,
                                     NULL)) < 0) {
        fprintf(stderr, "Could not open input file '%s' (error '%s')\n",
                filename, get_error_text(error));
        *input_format_context = NULL;
        return error;
    }

    /** Get information on the input file (number of streams etc.). */
    if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
        fprintf(stderr, "Could not open find stream info (error '%s')\n",
                get_error_text(error));
        avformat_close_input(input_format_context);
        return error;
    }

    /** Make sure that there is only one stream in the input file. */
    int stream_index = -1;
    if ( (*input_format_context)->nb_streams > 0 ) {
        for( int i=0; i<(*input_format_context)->nb_streams; i++ ) { //TODO remove output
            std::cout << "input stream " << i << ", type: " << (*input_format_context)->streams[i]->codec->codec_type << ": ";
            switch( (*input_format_context)->streams[i]->codec->codec_type ) {
                case AVMEDIA_TYPE_UNKNOWN: std::cout << "AVMEDIA_TYPE_UNKNOWN" << std::endl; break;
                case AVMEDIA_TYPE_VIDEO: std::cout << "AVMEDIA_TYPE_VIDEO" << std::endl; break;
                case AVMEDIA_TYPE_AUDIO: std::cout << "AVMEDIA_TYPE_AUDIO" << std::endl; stream_index = i; break;
                case AVMEDIA_TYPE_DATA: std::cout << "AVMEDIA_TYPE_DATA" << std::endl; break;
                case AVMEDIA_TYPE_SUBTITLE: std::cout << "AVMEDIA_TYPE_SUBTITLE" << std::endl; break;
                case AVMEDIA_TYPE_ATTACHMENT: std::cout << "AVMEDIA_TYPE_ATTACHMENT" << std::endl; break;
                case AVMEDIA_TYPE_NB: std::cout << "AVMEDIA_TYPE_NB" << std::endl; break;
                default: std::cout << "UNKNOWN" << std::endl;
            }
         }
        if( stream_index == -1 ) {
            avformat_close_input(input_format_context);
            return AVERROR_EXIT;
        }
    } else {
        std::cout << "no audio stram found:'" << filename << "'" << std::endl;
        avformat_close_input(input_format_context);
        return AVERROR_EXIT;
    }

    /** Find a decoder for the audio stream. */
    if (!(input_codec = avcodec_find_decoder((*input_format_context)->streams[stream_index]->codec->codec_id))) {
        fprintf(stderr, "Could not find input codec\n");
        avformat_close_input(input_format_context);
        return AVERROR_EXIT;
    }

    /** Open the decoder for the audio stream to use it later. */
    if ((error = avcodec_open2((*input_format_context)->streams[stream_index]->codec,
                               input_codec, NULL)) < 0) {
        fprintf(stderr, "Could not open input codec (error '%s')\n",
                get_error_text(error));
        avformat_close_input(input_format_context);
        return error;
    }

    /** Save the decoder context for easier access later. */
    *input_codec_context = (*input_format_context)->streams[stream_index]->codec;

    return 0;
}

bool LibAVcpp::parse(struct squawk::media::Audiofile & audiofile, const std::string mime_type, const std::string file) {
    LOG4CXX_TRACE(logger, "parse audio file:'" << file.c_str() << "'")

    AVFormatContext *fmt_ctx = NULL;
    AVCodecContext *input_codec_context = NULL;
    AVDictionaryEntry *tag = NULL;

    av_register_all();

    if( open_input_file(file.c_str(), &fmt_ctx, &input_codec_context) < 0 ) {
        return false;
    }

    audiofile.length = fmt_ctx->duration / AV_TIME_BASE;
    audiofile.bitrate = input_codec_context->bit_rate;
    audiofile.sample_rate = input_codec_context->sample_rate;
    audiofile.bits_per_sample = input_codec_context->bits_per_raw_sample;
    audiofile.channels = input_codec_context->channels;
    audiofile.sample_frequency = input_codec_context->sample_fmt;

    while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        if(std::strcmp(tag->key, "TITLE")==0 ||
                std::strcmp(tag->key, "title")==0) {
            audiofile.title = tag->value;
        } else if(std::strcmp(tag->key, "ARTIST")==0 ||
                  std::strcmp(tag->key, "artist")==0 ||
                  std::strcmp(tag->key, "album_artist")==0 ||
                  std::strcmp(tag->key, "COMPOSER")==0 ||
                  std::strcmp(tag->key, "ALBUM ARTIST")==0 ||
                  std::strcmp(tag->key, "ENSEMBLE")==0 ||
                  std::strcmp(tag->key, "PERFORMER")==0) {
            audiofile.artist.insert(audiofile.artist.end(), tag->value);
        } else if(std::strcmp(tag->key, "ALBUM")==0 ||
                  std::strcmp(tag->key, "album")==0) {
            audiofile.album = tag->value;
        } else if(std::strcmp(tag->key, "DATE")==0 ||
                  std::strcmp(tag->key, "date")==0 ||
                  std::strcmp(tag->key, "RELEASEYEAR")==0) {
            audiofile.year = tag->value;
        } else if(std::strcmp(tag->key, "track")==0) {
            audiofile.track = commons::string::parse_string<int>(std::string(tag->value));
        } else if(std::strcmp(tag->key, "GENRE")==0 ||
                  std::strcmp(tag->key, "genre")==0) {
            audiofile.genre= tag->value;
        } else if(std::strcmp(tag->key, "disc")==0) {
            audiofile.disc = commons::string::parse_string<int>(tag->value);
        } else if(std::strcmp(tag->key, "COMMENT")==0 ||
                  std::strcmp(tag->key, "DESCRIPTION")==0) {
            audiofile.comment = tag->value;
        } else if(std::strcmp(tag->key, "TRACKTOTAL")!=0 &&
                  std::strcmp(tag->key, "TLEN")!=0 &&
                  std::strcmp(tag->key, "encoded_by")!=0 &&
                  std::strcmp(tag->key, "encoder")!=0 &&
                  std::strcmp(tag->key, "ENCODER")!=0 &&
                  std::strcmp(tag->key, "HDTRACKS")!=0 &&
                  std::strcmp(tag->key, "SOURCE")!=0 &&
                  std::strcmp(tag->key, "CATALOG #")!=0 &&
                  std::strcmp(tag->key, "ALBUM DYNAMIC RANGE")!=0 &&
                  std::strcmp(tag->key, "DYNAMIC RANGE")!=0 &&
                  std::strcmp(tag->key, "CHANNELS")!=0 &&
                  std::strcmp(tag->key, "URL")!=0 &&
                  std::strcmp(tag->key, "TOTALTRACKS")!=0 &&
                  std::strcmp(tag->key, "language")!=0 &&
                  std::strcmp(tag->key, "ITUNES_CDDB_1")!=0 &&
                  std::strcmp(tag->key, "ENGINEER")!=0 &&
                  std::strcmp(tag->key, "ORIGINATOR")!=0 &&
                  std::strcmp(tag->key, "DISCTOTAL")!=0 &&
                  commons::string::starts_with(std::string(tag->key), "DISCOGS_") &&
                  commons::string::starts_with(std::string(tag->key), "REPLAYGAIN_") &&
                  ! commons::string::starts_with(std::string(tag->key), "Unknown Frame:")) {
            LOG4CXX_TRACE(logger, " * " << tag->key << " = " << tag->value)
        }
    }

    avcodec_close(input_codec_context);
    avformat_close_input(&fmt_ctx);
    return true;
}
}}
