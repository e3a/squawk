/*
    album entity definition.
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

#ifndef ALBUM_H
#define ALBUM_H

#include <string>

#include "artist.h"
#include "squawk.h"

#include <boost/algorithm/string/trim.hpp>

namespace squawk {
namespace media {

/**
 * \brief The album entity.
 */
class Album {
public:
	explicit Album() : id_ ( 0 ) {}
	Album ( const std::string & name, const std::string & genre, const std::string & year, const std::string & path ) :
		id_ ( 0 ), name_ ( boost::algorithm::trim_copy ( name ) ), genre_ ( boost::algorithm::trim_copy ( genre ) ), year_ ( boost::algorithm::trim_copy ( year ) ),
		clean_name_ ( squawk::media::clean_name ( name_ ) ),
		letter_ ( squawk::media::get_letter ( clean_name_ ) ),
		clean_path_ ( squawk::media::get_album_clean_path ( path ) ) {}

	Album ( const Album& ) = default;
	Album ( Album&& ) = default;
	Album& operator= ( const Album& ) = default;
	Album& operator= ( Album&& ) = default;
	~Album() {}

	unsigned long id() const;
	void id ( const unsigned long & id_ );
	std::string name() const;
	void name ( const std::string & name );
	std::string genre() const;
	void genre ( const std::string & genre );
	std::string year() const;
	void year ( const std::string & year );
	std::string cleanPath() const;
	std::string cleanName() const;
	std::string letter() const;

	std::list< Artist > & artists() { return artists_; }
	void add ( Artist artist );
	bool contains ( const Artist & artist ) const;

	bool equals ( const std::string & name ) const;

private:
	unsigned long id_;
	std::string name_, genre_, year_, clean_name_, letter_, clean_path_;
	std::list< Artist > artists_;
};
} // media
} // squawk
#endif // ALBUM_H
