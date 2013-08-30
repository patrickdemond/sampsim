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

#include <ostream>
#include <json/value.h>
#include <random>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building::building( tile *parent )
  {
    this->parent = parent;
    this->set_sample_mode( this->parent->get_sample_mode() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population* building::get_population() const
  {
    return NULL == this->parent ? NULL : this->parent->get_population();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building::~building()
  {
    // delete all households
    std::for_each( this->household_list.begin(), this->household_list.end(), utilities::safe_delete_type() );

    // we're holding a light reference to the parent, don't delete it
    this->parent = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::generate()
  {
    // make sure the building has a parent
    if( NULL == this->parent ) throw std::runtime_error( "Tried to generate an orphaned building" );

    // determine the building's position
    coordinate centroid = this->get_population()->get_centroid();
    std::pair< coordinate, coordinate > extent = this->get_tile()->get_extent();
    this->position.x = utilities::random() * ( extent.second.x - extent.first.x ) + extent.first.x;
    this->position.y = utilities::random() * ( extent.second.y - extent.first.y ) + extent.first.y;
    this->position.set_centroid( this->get_population()->get_centroid() );

    // for now we're only allowing one household per building
    household *h = new household( this );
    h->generate();
    this->household_list.push_back( h );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::from_json( const Json::Value &json )
  {
    this->position.from_json( json["position"] );
    
    this->household_list.reserve( json["household_list"].size() );
    for( unsigned int c = 0; c < json["household_list"].size(); c++ )
    {
      household *h = new household( this );
      h->from_json( json["household_list"][c] );
      this->household_list.push_back( h );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    this->position.to_json( json["position"] );
    json["household_list"] = Json::Value( Json::arrayValue );

    for( auto it = this->household_list.cbegin(); it != this->household_list.cend(); ++it )
    {
      household *h = *it;
      if( !this->get_sample_mode() || h->is_selected() )
      {
        Json::Value child;
        h->to_json( child );
        json["household_list"].append( child );
      }
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::to_csv(
    std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    for( auto it = this->household_list.begin(); it != this->household_list.end(); ++it )
    {
      household *h = *it;
      if( !this->get_sample_mode() || h->is_selected() ) h->to_csv( household_stream, individual_stream );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  int building::count_population() const
  {
    int count = 0;
    for( auto it = this->household_list.begin(); it != this->household_list.end(); ++it )
      if( !this->get_sample_mode() || (*it)->is_selected() )
        count += (*it)->count_population();

    return count;
  }
}
