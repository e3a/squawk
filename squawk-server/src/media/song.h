/*
    Song entity.
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

#ifndef SONG_H
#define SONG_H

namespace squawk {
namespace media {

/**
 * \brief The song entity.
 */
struct Song {
public:
	Song ( std::string title, std::string mime_type, std::string filename, int bitrate,
		   int sampleFrequency, int playLength, int track, int disc, int channels, int bits_per_sample )  :
		id_ ( 0 ), title_ ( title ), mime_type_ ( mime_type ), filename_ ( filename ), bitrate_ ( bitrate ),
		sampleFrequency_ ( sampleFrequency ), playLength_ ( playLength ), track_ ( track ), disc_ ( disc ),
		channels_ ( channels ), bits_per_sample_ ( bits_per_sample ) {}

	Song ( const Song& ) = default;
	Song ( Song&& ) = default;
	Song& operator= ( const Song& ) = default;
	Song& operator= ( Song&& ) = default;
	~Song() {}

	unsigned long id() const { return id_; }
	void id ( unsigned long & id ) { id_ = id; }
	std::string title() const { return title_; }
	void title ( const std::string & title ) { title_ = title; }
	std::string mime_type() const { return mime_type_; }
	void mime_type ( const std::string & mime_type ) { mime_type_ = mime_type; }
	int bitrate() const { return bitrate_; }
	void bitrate ( int bitrate ) { bitrate_ = bitrate; }
	int bitsPerSample() const { return bits_per_sample_; }
	void bitsPerSample ( const int & bits_per_sample ) { bits_per_sample_ = bits_per_sample; }
	int disc() const { return disc_; }
	void disc ( const int & disc ) { disc_ = disc; }
	int sampleFrequency() const { return sampleFrequency_; }
	void sampleFrequency ( const int & sampleFrequency ) { sampleFrequency_ = sampleFrequency; }
	int playLength() const { return playLength_; }
	void playLength ( const int & playLength ) { playLength_ = playLength; }
	int channels() const { return channels_; }
	void channels ( const int & channels ) { channels_ = channels; }
	int track() const { return track_; }
	void track ( const int & track ) { track_ = track; }

private:
	unsigned long id_;
	std::string title_, mime_type_, filename_, album_, genre, comment, year;
	int mtime, bitrate_, size, sampleFrequency_, playLength_, track_, disc_, channels_, bits_per_sample_;
};
} // media
} // squawk

#endif // SONG_H
