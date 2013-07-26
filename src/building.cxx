/*=========================================================================

  Program:  sampsim
  Module:   building.cxx
  Language: C++

=========================================================================*/

#include "building.h"

#include "household.h"
#include "population.h"
#include "tile.h"
#include "utilities.h"

#include <fstream>
#include <json/value.h>
#include <random>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building::building( tile *parent )
  {
    this->parent = parent;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population* building::get_population() const { return this->parent->get_population(); }

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
    std::pair< coordinate, coordinate > extent = this->get_tile()->get_extent();
    this->position.x = utilities::random() * ( extent.second.x - extent.first.x );
    this->position.y = utilities::random() * ( extent.second.y - extent.first.y );

    // for now we're only allowing one household per building
    household *h = new household( this );
    h->generate();
    this->household_list.push_back( h );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    this->position.to_json( json["position"] );
    json["household_list"] = Json::Value( Json::arrayValue );
    json["household_list"].resize( this->household_list.size() );

    int index = 0;
    household_list_type::const_iterator it;
    for( it = this->household_list.cbegin(); it != this->household_list.cend(); ++it, ++index )
      ( *it )->to_json( json["household_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::to_csv( std::ofstream &household_stream, std::ofstream &individual_stream ) const
  {
    household_list_type::const_iterator it;
    for( it = this->household_list.begin(); it != this->household_list.end(); ++it )
      ( *it )->to_csv( household_stream, individual_stream );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  int building::count_population() const
  {
    int count = 0;
    household_list_type::const_iterator it;
    for( it = this->household_list.begin(); it != this->household_list.end(); ++it )
      count += (*it)->count_population();

    return count;
  }
}
