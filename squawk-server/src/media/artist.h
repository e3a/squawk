/*
    artist entity.
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

#ifndef ARTIST_H
#define ARTIST_H

#include "squawk.h"

namespace squawk {
namespace media {

/**
 * \brief The artist entity.
 */
struct Artist {
public:
	/**
	 * @brief Artist Class.
	 * @param name The artist Name.
	 */
	Artist ( const std::string & name ) :
		id_ ( 0 ), name_ ( name ),
		clean_name_ ( squawk::media::clean_name ( name_ ) ),
		letter_ ( squawk::media::get_letter ( clean_name_ ) ) {}

	Artist ( const Artist& ) = default;
	Artist ( Artist&& ) = default;
	Artist& operator= ( const Artist& ) = default;
	Artist& operator= ( Artist&& ) = default;
	~Artist() { }

	void id ( const unsigned long & id ) { this->id_ = id; }
	unsigned long id() const { return id_; }
	const std::string letter() const { return letter_; }
	const std::string name() const { return name_; }
	const std::string cleanName() const { return clean_name_; }

private:
	unsigned long id_;
	std::string name_, clean_name_, letter_;
};
} // squawk
} // media
#endif // ARTIST_H
