/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  <copyright holder> <email>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#include "upnpwriter.h"

UpnpWriter::UpnpWriter() {

}

UpnpWriter::~UpnpWriter() {

}

void UpnpWriter::addContainer(const CLASS & class_name, const std::string & id, const std::string & parentId, 
			      const std::string & restriced, const std::string & childCount, const std::string & title, 
			      const std::string & storageUsed) {
  
    commons::xml::Node container_element = xmlWriter->element( (*element), "", "container", "" );
    xmlWriter->attribute(container_element, "id", id);
    xmlWriter->attribute(container_element, "parentID", parentId);
    xmlWriter->attribute(container_element, "restricted", restriced);
    xmlWriter->attribute(container_element, "childCount", childCount);
    
    xmlWriter->element(container_element, commons::upnp::XML_NS_PURL, "title", title);
    xmlWriter->element(container_element, commons::upnp::XML_NS_UPNP, "class", class_names[(int)class_name]);
    xmlWriter->element(container_element, commons::upnp::XML_NS_UPNP, "storageUsed", storageUsed);
}
void UpnpWriter::addItem(const CLASS & class_name, const std::string & id, const std::string & parentId, 
			 const std::string & restriced, const std::string & title, std::string & resUri, 
			 std::map<std::string, std::string> & res_attributes ) {
  
  commons::xml::Node item_element = xmlWriter->element( didl_element, "", "item", "" );
  xmlWriter->attribute(item_element, "id", id );
  xmlWriter->attribute(item_element, "parentID", parentId );
  xmlWriter->attribute(item_element, "restricted", restricted );

  xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "title", title );
  xmlWriter->element(item_element, commons::upnp::XML_NS_UPNP, "class", class_names[(int)class_name] );
  xmlWriter->element(item_element, commons::upnp::XML_NS_PURL, "date", date );

  commons::xml::Node dlna_res_node = xmlWriter->element(item_element, "", "res", resUri );
  for( auto & iter : res_attributes ) {
    xmlWriter->attribute(dlna_res_node, "", iter.first, iter.second );
  }
}
