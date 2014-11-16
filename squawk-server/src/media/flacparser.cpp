/*
    Parse FLAC files

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

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include "FLAC++/all.h"

#include "mimetypes.h"
#include "flacparser.h"

namespace squawk {
namespace media {
  
log4cxx::LoggerPtr FlacParser::logger(log4cxx::Logger::getLogger("squawk.media.FlacParser"));

bool FlacParser::parse(struct squawk::media::Audiofile & audiofile, const std::string mime_type, const std::string file) {

    if( mime_type != "audio/x-flac" ) {
        return false;
    }

    audiofile.mime_type = std::string("audio/flac"); //TODO type from header

    FLAC::Metadata::Chain chain;
    if (!chain.read(file.c_str())) {
        LOG4CXX_FATAL(logger, "Unable to open file:" << file);
    return false;
    }

    FLAC::Metadata::Iterator iterator;
    iterator.init(chain);

    FLAC::Metadata::VorbisComment *vc = 0;
    FLAC::Metadata::StreamInfo *si = 0;
    do {
        FLAC::Metadata::Prototype* block = iterator.get_block();
        if (block->get_type() == FLAC__METADATA_TYPE_VORBIS_COMMENT) {
            vc = dynamic_cast<FLAC::Metadata::VorbisComment*>(iterator.get_block());
        } else if(block->get_type() == FLAC__METADATA_TYPE_STREAMINFO) {
            si = dynamic_cast<FLAC::Metadata::StreamInfo*>(iterator.get_block());
	}
    } while (iterator.next());

    if(vc != 0) {
	for(int i=0; i<vc->get_num_comments(); i++) {
            FLAC::Metadata::VorbisComment::Entry entry = vc->get_comment(i);
            const char * field_name = commons::string::to_upper( entry.get_field_name() ).c_str();
            if(strcmp(field_name, "DATE") == 0) {
                audiofile.year = std::string(entry.get_field_value());
            } else if(strcmp(field_name, "ARTIST") == 0 ||
                  strcmp(field_name, "BAND") == 0 ||
                  strcmp(field_name, "COMPOSER") == 0 ||
                  strcmp(field_name, "PERFORMER") == 0 ||
                  strcmp(field_name, "ALBUMARTIST") == 0 ||
                  strcmp(field_name, "ENSEMBLE") == 0 ||
                  strcmp(field_name, "ALBUM ARTIST") == 0) {
	      
          audiofile.artist.insert(audiofile.artist.end(), entry.get_field_value());
	      
        } else if(strcmp(field_name, "ALBUM") == 0) {
                audiofile.album = std::string(entry.get_field_value());
            } else if(strcmp(field_name, "GENRE") == 0) {
                audiofile.genre = std::string(entry.get_field_value());
            } else if(strcmp(field_name, "TRACKNUMBER") == 0) {
                audiofile.track = atoi(entry.get_field_value());
            } else if(strcmp(field_name, "TITLE") == 0) {
                audiofile.title = std::string(entry.get_field_value());
            } else if(strcmp(field_name, "DISCNUMBER") == 0) {
                audiofile.disc = atoi(entry.get_field_value());
            } else if(strcmp(field_name, "COMMENT") == 0) {
                audiofile.comment = std::string(entry.get_field_value());
		
            } else if(logger->isDebugEnabled() &&
              strcmp(field_name, "TRACKTOTAL") != 0 && strcmp(field_name, "TOTALTRACKS") != 0 &&
              strcmp(field_name, "HDTRACKS") != 0 && strcmp(field_name, "DESCRIPTION") != 0 &&
              strcmp(field_name, "ENCODER") != 0 && strcmp(entry.get_field_name(), "CHANNELS") != 0 &&
		      strcmp(entry.get_field_name(), "ALBUM DYNAMIC RANGE") != 0 && strcmp(entry.get_field_name(), "DYNAMIC RANGE") != 0 &&
		      strcmp(entry.get_field_name(), "REPLAYGAIN_ALBUM_PEAK") != 0 && strcmp(entry.get_field_name(), "REPLAYGAIN_ALBUM_GAIN") != 0  &&
		      strcmp(entry.get_field_name(), "REPLAYGAIN_TRACK_PEAK") != 0 && strcmp(entry.get_field_name(), "REPLAYGAIN_TRACK_GAIN") != 0  &&
		      strcmp(entry.get_field_name(), "dvda_track") != 0 && strcmp(entry.get_field_name(), "dvda_title") != 0  &&
		      strcmp(entry.get_field_name(), "dvda_titleset") != 0 && strcmp(entry.get_field_name(), "Encoder") != 0  &&
		      strcmp(entry.get_field_name(), "Ripping Tool") != 0 && strcmp(entry.get_field_name(), "Release Type") != 0  &&
              strcmp(field_name, "ORGANIZATION") != 0 && strcmp(entry.get_field_name(), "Language") != 0  &&
		      strcmp(entry.get_field_name(), "Rip Date") != 0 && strcmp(entry.get_field_name(), "Retail Date") != 0  &&
		      strcmp(entry.get_field_name(), "TOTALDISCS") != 0 && strcmp(entry.get_field_name(), "SERIAL_NO") != 0  &&
              strcmp(entry.get_field_name(), "compilation") != 0 && strcmp(entry.get_field_name(), "itunesmediatype") != 0  &&
              strcmp(entry.get_field_name(), "DISCTOTAL") != 0 && strcmp(entry.get_field_name(), "CODINGHISTORY") != 0  &&
              strcmp(entry.get_field_name(), "URL") != 0 && strcmp(entry.get_field_name(), "Media") != 0  &&
              strcmp(entry.get_field_name(), "CATALOG #") != 0 && strcmp(field_name, "SOURCE") != 0 ) {
                LOG4CXX_DEBUG(logger, ":Vorbis Comment: " << entry.get_field_name () << " = " << entry.get_field_value());
	    }
	}
    } else {
        LOG4CXX_INFO(logger, "no tag found in:" << file);
        return false;
    }
    if(si != 0) {
      audiofile.bits_per_sample = si->get_bits_per_sample();
      audiofile.sample_rate = si->get_sample_rate();
      audiofile.channels = si->get_channels();
      audiofile.length = ( si->get_total_samples() / si->get_sample_rate());
      
    } else {
        LOG4CXX_FATAL(logger, "no stream info in:" << file);
    }
    delete vc;
    delete si;

    return true;
}
}}
