/*=========================================================================

  Program:  sampsim
  Module:   building.cxx
  Language: C++

=========================================================================*/

#include "building.h"

#include "household.h"
#include "tile.h"
#include "utilities.h"

#include <json/value.h>
#include <random>
#include <vector>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building::building( tile *parent )
  {
    this->parent = parent;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building::~building()
  {
    // we're holding a light reference to the parent, don't delete it
    this->parent = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::generate()
  {
    // determine the building's position
    std::pair< coordinate, coordinate > extent = this->parent->get_extent();
    this->position.x = utilities::random() * ( extent.second.x - extent.first.x );
    this->position.y = utilities::random() * ( extent.second.y - extent.first.y );

    // for now we're only allowing one household per building
    household *h = new household( this );
    h->generate();
    this->household_list.push_back( h );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::to_json( Json::Value &json )
  {
    json = Json::Value( Json::objectValue );
    this->position.to_json( json["position"] );
    json["household_list"] = Json::Value( Json::arrayValue );
    json["household_list"].resize( this->household_list.size() );

    int index = 0;
    std::vector< household* >::const_iterator it;
    for( it = this->household_list.begin(); it != this->household_list.end(); ++it, ++index )
      ( *it )->to_json( json["household_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  int building::get_population() const
  {
    int count = 0;
    std::vector< household* >::const_iterator it;
    for( it = this->household_list.begin(); it != this->household_list.end(); ++it )
      count += (*it)->get_population();

    return count;
  }
}
