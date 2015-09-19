/*
    album entity implementation.
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

#include "album.h"

namespace squawk {
namespace media {

unsigned long Album::id() const { return id_; }
void Album::id ( const unsigned long & id ) { id_ = id; }
std::string Album::name() const {return name_; }
void Album::name ( const std::string & name ) {name_ = boost::algorithm::trim_copy ( name ); }
std::string Album::genre() const {return genre_; }
void Album::genre ( const std::string & genre ) {genre_ = boost::algorithm::trim_copy ( genre ); }
std::string Album::year() const {return year_; }
void Album::year ( const std::string & year ) {year_ = boost::algorithm::trim_copy ( year ); }
std::string Album::cleanPath() const {return clean_path_; }

std::string Album::cleanName() const {
	return clean_name_;
}
std::string Album::letter() const {
	return letter_;
}
bool Album::contains ( const Artist & artist ) const {
	for ( auto & _artist : artists_ ) {
		if ( _artist.cleanName() == artist.cleanName() ) {
			return true;
		}
	}

	return false;
}
void Album::add ( squawk::media::Artist artist ) {
	artists_.push_back ( std::move ( artist ) );
}

bool Album::equals ( const std::string & name ) const {
	return name_ == boost::algorithm::trim_copy ( name );
}
} // media
} // squawk
