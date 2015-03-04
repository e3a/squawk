#ifndef VIDEO_H
#define VIDEO_H

#include "commons.h"

#include <cstring>
#include <string>
#include <iostream>
#include <map>
#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>

#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#endif
}

namespace commons {
/**
 * @brief Media Utilities.
 */
namespace media {

enum Codec {

    MJPEGB = 1,
    H264 = 2
};

/**
 * @brief The MediaException class
 */
class MediaException : public std::exception {
public:

    static constexpr int FILE_EXCEPTION = 1; /** error with file */
    static constexpr int STREAM_EXCEPTION = 2; /** error with open media streams */
    static constexpr int CODEC_EXCEPTION = 3; /** error with open codec */

  explicit MediaException (int _code, std::string _what) throw() : _code(_code), _what(_what) {}
  virtual ~MediaException() throw() {}
  virtual const char* what() const throw() {
return _what.c_str();
  };
  int code() throw() {
return _code;
  }
private:
  int _code;
  std::string _what;
};

/**
 * @brief The AudioStream class
 */
class AudioStream {
public:
    /**
     * @brief AudioStream
     */
    AudioStream() : _bitrate(0), _bits_per_sample(0), _channels(0), _sample_frequency(0) {}
    /**
     * @brief bitrate
     * @return
     */
    int bitrate() { return _bitrate; }
    /**
     * @brief bitrate
     * @param bitrate
     */
    void bitrate( int bitrate ) { _bitrate = bitrate; }
    /**
     * @brief bitsPerSample
     * @return
     */
    int bitsPerSample() { return _bits_per_sample; }
    /**
     * @brief bitsPerSample
     * @param bits_per_sample
     */
    void bitsPerSample( int bits_per_sample ) { _bits_per_sample = bits_per_sample; }
    /**
     * @brief channels
     * @return
     */
    int channels() { return _channels; }
    /**
     * @brief channels
     * @param channels
     */
    void channels( int channels ) { _channels = channels; }
    /**
     * @brief codec id
     * @return
     */
    int codec() { return _codec; }
    /**
     * @brief codec id
     * @param codec
     */
    void codec( int codec ) { _bitrate = codec; }
    /**
     * @brief sampleFrequency
     * @return
     */
    int sampleFrequency() { return _sample_frequency; }
    /**
     * @brief sampleFrequency
     * @param sample_frequency
     */
    void sampleFrequency( int sample_frequency ) { _sample_frequency = sample_frequency; }

    /**
     * @brief operator write the object to the ostream.
     */
    friend std::ostream& operator <<(std::ostream &os,const AudioStream &obj) {
        os <<  "AudioStream::" << std::endl;
        os << " codec id: " << obj._codec << std::endl;
        os << " channels: " << obj._channels << ", bitrate: " << obj._bitrate << ", bits_per_sample: " <<
              obj._bits_per_sample << ", frequency: " << obj._sample_frequency << std::endl;
        return os;
    }
private:
    int _bitrate, _bits_per_sample, _channels, _sample_frequency, _codec;
};
/**
 * @brief The VideoStream class
 */
class VideoStream {
public:
    /**
     * @brief bitrate
     * @return
     */
    int bitrate() { return _bitrate; }
    /**
     * @brief bitrate
     * @param bitrate
     */
    void bitrate( int bitrate ) { _bitrate = bitrate; }
    /**
     * @brief VideoStream
     */
    VideoStream() : _width(0), _height(0), _codec(0) {}
    /**
     * @brief codec type
     * @return
     */
    int codec() { return _codec; }
    /**
     * @brief codec type
     * @param codec
     */
    void codec( int codec ) {  _codec = codec; }
    /**
     * @brief height
     * @return
     */
    int height() { return _height; }
    /**
     * @brief height
     * @param height
     */
    void height( int height ) {  _height = height; }
    /**
     * @brief width
     * @return
     */
    int width() { return _width; }
    /**
     * @brief width
     * @param width
     */
    void width( int width ) {  _width = width; }

    /**
     * @brief operator write the object to the ostream.
     */
    friend std::ostream& operator <<(std::ostream &os,const VideoStream &obj) {
        os <<  "VideoStream::" << std::endl;
        os << " codec id: " << obj._codec << std::endl;
        os << " bitrate: " << obj._bitrate << ", width: " << obj._width << ", height: " << obj._height << std::endl;
        return os;
    }
private:
    int _width, _height, _codec, _bitrate;
};
/**
 * @brief The MediaFile class
 */
class MediaFile {
public:
    /**
     * @brief The meta tag names.
     */
    enum TAG { ARTIST, ALBUM, COMPOSER, DISC, TRACK, TITLE, YEAR, GENRE, COMMENT };
    /**
     * @brief MediaFile
     * @param filename
     */
    MediaFile( std::string filename ) : filename(filename), _duration(0) {}
    /**
     * @brief duration
     * @return
     */
    int duration() { return _duration; }
    /**
     * @brief duration
     * @param duration
     */
    void duration( int duration ) { _duration = duration; }
    /**
     * @brief size
     * @return
     */
    int size() { return commons::filesystem::filesize( filename ); }
    /**
     * @brief name
     * @return
     */
    std::string name() {
        int start = 0;
        int end = filename.size();
        if( filename.find("/") != std::string::npos ) {
            start = filename.find_last_of("/") + 1;
        }
        if( filename.find(".", start ) != std::string::npos ) {
            end = filename.find_last_of(".");
        }
        return filename.substr(start, end - start);
    }
    /**
     * @brief getAudioStreams
     * @return
     */
    std::vector<AudioStream> getAudioStreams() {
        return audio_streams;
    }
    /**
     * @brief getVideoStreams
     * @return
     */
    std::vector<VideoStream> getVideoStreams() {
        return video_streams;
    }
    /**
     * @brief addTag
     * @param tag
     * @param value
     */
    void addTag( TAG tag, std::string value ) {
        tags[tag] = value;
    }
    /**
     * @brief getTag
     * @param tag
     * @return
     */
    std::string getTag( TAG tag ) {
        return tags[tag];
    }
    /**
     * @brief tagNames
     * @return
     */
    std::vector<TAG> tagNames() {
        std::vector<TAG> result;
        for(auto & itrTag : tags ) {
            result.push_back( itrTag.first );
        }
        return result;
    }
    /**
     * @brief check if tags contains key.
     * @param name tag key
     * @return
     */
    bool hasTag( const TAG & key ) {
        return tags.find( key ) != tags.end();
    }

    /**
     * @brief operator write the object to the ostream.
     */
    friend std::ostream& operator <<(std::ostream &os, const MediaFile &obj) {
        os <<  "MediaFile::" << std::endl;
        os << " duration: " <<  commons::string::time_to_string( obj._duration ) << std::endl;
        os << "Meta Information: " << std::endl;
        for( auto & iter : obj.audio_streams ) std::cout << iter;
        for( auto & iter : obj.video_streams ) std::cout << iter;
        for( auto & iter : obj.tags ) {
            os << "\t" << iter.first << " = " << iter.second << std::endl;
        }
        return os;
    }
private:
    std::string filename;
    int _duration;
    std::vector<VideoStream> video_streams;
    std::vector<AudioStream> audio_streams;
    std::map<TAG, std::string > tags;
    void addVideoStream( VideoStream && video ) {
        video_streams.push_back( video );
    }
    void addAudioStream( AudioStream && audio ) {
        audio_streams.push_back( audio );
    }
    friend class MediaParser;
};

class MediaParser {
public:
    static MediaFile parseFile( std::string filename ) {

        MediaFile media_file( filename );

        AVFormatContext *fmt_ctx = NULL;
        AVCodecContext *input_codec_context = NULL;
        AVDictionaryEntry *tag = NULL;

        av_register_all();

        open_input_file(filename.c_str(), &fmt_ctx, &input_codec_context);

        /** Make sure that there is only one stream in the input file. */
        if ( fmt_ctx->nb_streams > 0 ) {
            media_file.duration( fmt_ctx->duration / AV_TIME_BASE ); //set the playlength
            for( unsigned int i=0; i<fmt_ctx->nb_streams; i++ ) { //TODO remove output
                std::cout << "input stream " << i << ", type: " << fmt_ctx->streams[i]->codec->codec_type << ": ";
                switch( fmt_ctx->streams[i]->codec->codec_type ) {
                    case AVMEDIA_TYPE_UNKNOWN: std::cout << "AVMEDIA_TYPE_UNKNOWN" << std::endl; break;
                    case AVMEDIA_TYPE_VIDEO: {
                        std::cout << "AVMEDIA_TYPE_VIDEO" << std::endl;
                        open_input_decoder(i, &fmt_ctx, &input_codec_context);
                        VideoStream videofile;
                        videofile.width( input_codec_context->width );
                        videofile.height( input_codec_context->height );
                        videofile.codec( input_codec_context->codec_id );
                        media_file.addVideoStream( std::move( videofile ) );
                        avcodec_close(input_codec_context);
                        break;
                    }
                    case AVMEDIA_TYPE_AUDIO: {
                        open_input_decoder(i, &fmt_ctx, &input_codec_context);
                        std::cout << "AVMEDIA_TYPE_AUDIO, codec_id:" << input_codec_context->codec_id << std::endl;
                        if( input_codec_context->codec_id == AV_CODEC_ID_MP3 ) {
                            std::cout << "code is mp3:" << std::endl;
                        }
                        AudioStream audiofile;
                        audiofile.bitrate( input_codec_context->bit_rate );
                        audiofile.bitsPerSample( input_codec_context->bits_per_raw_sample );
                        audiofile.channels( input_codec_context->channels );
                        audiofile.sampleFrequency( input_codec_context->sample_rate );
                        audiofile.codec( input_codec_context->codec_id );
                        media_file.addAudioStream( std::move( audiofile ) );
                        avcodec_close(input_codec_context);
                        break;
                    }
                    case AVMEDIA_TYPE_DATA: std::cout << "AVMEDIA_TYPE_DATA" << std::endl; break;
                    case AVMEDIA_TYPE_SUBTITLE: std::cout << "AVMEDIA_TYPE_SUBTITLE" << std::endl; break;
                    case AVMEDIA_TYPE_ATTACHMENT: {

                        std::cout << "AVMEDIA_TYPE_ATTACHMENT" << std::endl;
                        open_input_decoder(i, &fmt_ctx, &input_codec_context);
                        std::cout << "AVMEDIA_TYPE_ATTACHMENT, codec_id:" << input_codec_context->codec_id << std::endl;
                        if( input_codec_context->codec_id == AV_CODEC_ID_MJPEG ) {
                            std::cout << "code is AV_CODEC_ID_MJPEG:" << std::endl;
                        } else if( input_codec_context->codec_id == AV_CODEC_ID_MJPEGB ) {
                            std::cout << "code is AV_CODEC_ID_MJPEGB:" << std::endl;
                        } else if( input_codec_context->codec_id == AV_CODEC_ID_LJPEG ) {
                            std::cout << "code is AV_CODEC_ID_LJPEG:" << std::endl;
                        }
                        break;
                    }
                    case AVMEDIA_TYPE_NB: std::cout << "AVMEDIA_TYPE_NB" << std::endl; break;
                    default: std::cout << "UNKNOWN" << std::endl;
                }
             }

            //add the meta information
            while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
                std::cout << tag->key << ":"<< tag->value << std::endl;
                if(std::strcmp(tag->key, "TITLE")==0 ||
                        std::strcmp(tag->key, "title")==0) {
                    media_file.addTag( MediaFile::TITLE, commons::string::trim( tag->value ) );
                } else if(std::strcmp(tag->key, "ARTIST")==0 ||
                          std::strcmp(tag->key, "artist")==0 ||
                          std::strcmp(tag->key, "album_artist")==0 ||
                          std::strcmp(tag->key, "COMPOSER")==0 ||
                          std::strcmp(tag->key, "ALBUM ARTIST")==0 ||
                          std::strcmp(tag->key, "ENSEMBLE")==0 ||
                          std::strcmp(tag->key, "PERFORMER")==0) {
                    media_file.addTag( MediaFile::ARTIST, commons::string::trim( tag->value ) );
                } else if(std::strcmp(tag->key, "COMPOSER")==0) {
                    media_file.addTag( MediaFile::COMPOSER, commons::string::trim( tag->value ) );
                } else if(std::strcmp(tag->key, "ALBUM")==0 ||
                          std::strcmp(tag->key, "album")==0) {
                    media_file.addTag( MediaFile::ALBUM, commons::string::trim( tag->value ) );
                } else if(std::strcmp(tag->key, "DATE")==0 ||
                          std::strcmp(tag->key, "date")==0 ||
                          std::strcmp(tag->key, "RELEASEYEAR")==0) {
                    media_file.addTag( MediaFile::YEAR, commons::string::trim( tag->value ) );
                } else if(std::strcmp(tag->key, "track")==0) {
                    media_file.addTag( MediaFile::TRACK, commons::string::trim( tag->value ) );
                } else if(std::strcmp(tag->key, "GENRE")==0 ||
                          std::strcmp(tag->key, "genre")==0) {
                    media_file.addTag( MediaFile::GENRE, commons::string::trim( tag->value ) );
                } else if(std::strcmp(tag->key, "disc")==0) {
                    media_file.addTag( MediaFile::DISC, commons::string::trim( tag->value ) );
                } else if(std::strcmp(tag->key, "COMMENT")==0 ||
                          std::strcmp(tag->key, "DESCRIPTION")==0) {
                    media_file.addTag( MediaFile::COMMENT, commons::string::trim( tag->value ) );
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
                    std::cout << " * " << tag->key << " = " << tag->value << std::endl;
                }
            }

        } else {
            std::cout << "no audio stram found:'" << filename << "'" << std::endl;
            avformat_close_input(&fmt_ctx);
            throw MediaException( MediaException::STREAM_EXCEPTION, "no stream in the input file found.");
        }

        avformat_close_input(&fmt_ctx);
        return media_file;
    }

private:
    static std::string get_error_text(const int error) {
        static char error_buffer[255];
        av_strerror(error, error_buffer, sizeof(error_buffer));
        return std::string( error_buffer );
    }

    /** get the media information **/
    static void getMediaInformation() {

    }

    /** Open an input file and the required decoder. */
    static void open_input_file(const char *filename,
                               AVFormatContext **input_format_context,
                               AVCodecContext **input_codec_context /* TODO unused */ ) {
        int error;

        /** Open the input file to read from it. */
        if ((error = avformat_open_input(input_format_context, filename, NULL, NULL)) < 0) {
            throw MediaException( MediaException::FILE_EXCEPTION, "unable to open media file: " + get_error_text(error));
        }

        /** Get information on the input file (number of streams etc.). */
        if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
            avformat_close_input(input_format_context);
            throw MediaException( MediaException::STREAM_EXCEPTION, "Could not find stream info: " + get_error_text(error));
        }

    }


    /** Open an input file and the required decoder. */
    static void open_input_decoder(int stream_index,
                               AVFormatContext **input_format_context,
                               AVCodecContext **input_codec_context ) {
        AVCodec *input_codec;

        /** Find a decoder for the audio stream. */
        if (!(input_codec = avcodec_find_decoder((*input_format_context)->streams[stream_index]->codec->codec_id))) {
            throw MediaException( MediaException::CODEC_EXCEPTION, "Could not find input codec: " );
        }

        /** Open the decoder for the audio stream to use it later. */
        int error;
        if ((error = avcodec_open2((*input_format_context)->streams[stream_index]->codec, input_codec, NULL)) < 0) {
            avformat_close_input(input_format_context);
            throw MediaException( MediaException::FILE_EXCEPTION, "Could not open input codec: " + get_error_text(error) );
        }

        /** Save the decoder context for easier access later. */
        *input_codec_context = (*input_format_context)->streams[stream_index]->codec;
    }
};
}}
#endif // VIDEO_H
