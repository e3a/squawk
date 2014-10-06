/*
    Parse MPEG files
    
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

#include <iostream>
#include <id3/tag.h>
#include "squawk.h"
#include "mpegparser.h"

namespace squawk {
namespace media {
  
log4cxx::LoggerPtr MpegParser::logger(log4cxx::Logger::getLogger("squawk.media.MpegParser"));

void MpegParser::parse(struct squawk::model::Song * audiofile, std::string file) {
  LOG4CXX_DEBUG(logger, "import mpeg file:" << file)
  
  audiofile->mime_type = std::string("audio/mpeg");

  ID3_Tag myTag(file.c_str());

      //get the streaminfo
      const Mp3_Headerinfo* mp3info;
      mp3info = myTag.GetMp3HeaderInfo();
      if (mp3info) {
	  switch(mp3info->channelmode) {
	  case    MP3CHANNELMODE_FALSE:
	      break;
	  case    MP3CHANNELMODE_STEREO:
	  case    MP3CHANNELMODE_JOINT_STEREO:
	  case    MP3CHANNELMODE_DUAL_CHANNEL:
	      audiofile->channels = 2;
	      break;
	  case    MP3CHANNELMODE_SINGLE_CHANNEL:
	      audiofile->channels = 1;
	      break;
	  }
	  audiofile->bitrate = mp3info->bitrate/1000;
	  audiofile->samplerate = mp3info->frequency;
      } else {
	  LOG4CXX_WARN(logger, "no mp3 stream info:" << file)
      }

      //the the tags
      ID3_Tag::Iterator* iter = myTag.CreateIterator();
      ID3_Frame* myFrame = NULL;
      while (NULL != (myFrame = iter->GetNext())) {
	if(myFrame->GetID() == ID3FID_TITLE) {
		char title[512];
		myFrame->GetField(ID3FN_TEXT)->Get(title, 512);
		audiofile->title = std::string(title);
	} else if(myFrame->GetID() == ID3FID_ALBUM) {
		char album[512];
		myFrame->GetField(ID3FN_TEXT)->Get(album, 512);
		audiofile->album = std::string(album);
	} else if(myFrame->GetID() == ID3FID_BAND) {
		char myBuffer[1024];
		size_t charsUsed = myFrame->GetField(ID3FN_TEXT)->Get(myBuffer, 1024);
		std::cout << "Band:" << myBuffer << "\n";
	} else if(myFrame->GetID() == ID3FID_LEADARTIST) {
		char c_artist[512];
		myFrame->GetField(ID3FN_TEXT)->Get(c_artist, 512);
		bool found_artist = false;
		for(std::list< squawk::model::Artist >::iterator artist_iter = audiofile->artist.begin(); artist_iter != audiofile->artist.end(); artist_iter++) {
		  if((*artist_iter).name == std::string(c_artist)) {
		    found_artist = true;
		    break;
		  }
		}
		if(! found_artist) {
		  squawk::model::Artist artist;
		  artist.name = std::string(c_artist);
		  audiofile->artist.insert(audiofile->artist.end(), artist);
		}
	} else if(myFrame->GetID() == ID3FID_COMPOSER) {
		char composer[512];
		myFrame->GetField(ID3FN_TEXT)->Get(composer, 512);
		//TODO add to artist list audiofile->composer = std::string(composer);
	} else if(myFrame->GetID() == ID3FID_YEAR) {
		char year[16];
		myFrame->GetField(ID3FN_TEXT)->Get(year, 16);
		audiofile->year = std::string(year);
	} else if(myFrame->GetID() == ID3FID_CONTENTTYPE) {
		char genre[32];
		myFrame->GetField(ID3FN_TEXT)->Get(genre, 32);
		audiofile->genre = std::string(genre);
	} else if(myFrame->GetID() == ID3FID_TRACKNUM) {
		char trckBuffer[4];
		if(myFrame->GetField(ID3FN_TEXT)->Get(trckBuffer, 8) > 0) {
			audiofile->track = atoi(trckBuffer);
		} else {
			audiofile->track = 0;
		}
	} else if(myFrame->GetID() == ID3FID_PUBLISHER) {
		char myBuffer[1024];
		size_t charsUsed = myFrame->GetField(ID3FN_TEXT)->Get(myBuffer, 1024);
		std::cout << "Publisher:" << myBuffer << "\n";
	} else if(myFrame->GetID() == ID3FID_CONTENTTYPE) {
		char myBuffer[1024];
		size_t charsUsed = myFrame->GetField(ID3FN_TEXT)->Get(myBuffer, 1024);
		std::cout << "Content Type:" << myBuffer << "\n";

	} else {
	  LOG4CXX_WARN(logger, "Unknown Frame:" << myFrame->GetID() << "=" << myFrame->GetDescription())
	}
    }
  delete iter;
  }
}
}