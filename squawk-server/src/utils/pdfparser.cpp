/*
    get ISBN from pdf file.
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

#include <iostream>

#include "poppler/cpp/poppler-document.h"
#include "poppler/cpp/poppler-page.h"
#include "pcrecpp.h"

#include "pdfparser.h"
#include "squawk.h"

std::string PdfParser::parsePdf( const std::string & filename ) {

    static pcrecpp::RE re("ISBN(-10)? *:? *([\\d*-?]*)");
    poppler::document * doc = poppler::document::load_from_file  ( filename );
    if( doc == nullptr ) {
        std::cerr << "unable to open pdf document" << std::endl;
        return "";
    }
    size_t pages = doc->pages();
    for( size_t i=0; i<pages; i++ ) {
        poppler::page * p = doc->create_page( i );
        std::string dummy, isbn_;
        if( re.PartialMatch( p->text().to_latin1(), &dummy, &isbn_ ) ) {
            return isbn_;
        }
    }
    if( squawk::DEBUG )
        { std::cerr << "no ISBN found in: " << filename << std::endl; }
    return "";
}
