/*=========================================================================

  Program:  sampsim
  Module:   building.cxx
  Language: C++

=========================================================================*/

#include "building.h"

#include "household.h"
#include "population.h"
#include "town.h"
#include "tile.h"
#include "utilities.h"

#include <json/value.h>
#include <random>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building::building( tile *parent )
  {
    this->parent = parent;
    this->selected = false;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building::~building()
  {
    // delete all households
    std::for_each( this->household_list.begin(), this->household_list.end(), utilities::safe_delete_type() );
    this->household_list.empty();

    // we're holding a light reference to the parent, don't delete it
    this->parent = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  town* building::get_town() const
  { 
    return NULL == this->parent ? NULL : this->parent->get_town();
  } 

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population* building::get_population() const
  {
    return NULL == this->parent ? NULL : this->parent->get_population();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::create()
  {
    // make sure the building has a parent
    if( NULL == this->parent ) throw std::runtime_error( "Tried to create an orphaned building" );

    // determine the building's position
    town* town = this->get_town();
    std::pair< coordinate, coordinate > extent = this->get_tile()->get_extent();
    this->position.x = utilities::random() * ( extent.second.x - extent.first.x ) + extent.first.x;
    this->position.y = utilities::random() * ( extent.second.y - extent.first.y ) + extent.first.y;
    this->position.set_centroid( town->get_centroid() );

    // for now we're only allowing one household per building
    household *h = new household( this );
    h->create();
    this->household_list.push_back( h );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::define()
  {
    for( auto it = this->household_list.begin(); it != this->household_list.end(); ++it ) (*it)->define();

    // determine the pocket factor
    population* pop = this->get_population();
    town* town = this->get_town();
    this->pocket_factor = 0.0;
    for( auto it = town->get_disease_pocket_list_cbegin(); it != town->get_disease_pocket_list_cend(); ++it )
    {
      double distance = this->position.distance( *it ) / pop->get_pocket_scaling();
      std::string type = pop->get_pocket_kernel_type();
      if( "exponential" == type )
      {
        this->pocket_factor += exp( -distance );
      }
      else if( "inverse_square" == type )
      {
        this->pocket_factor += 1 / ( distance * distance );
      }
      else if( "gaussian" == type )
      {
        this->pocket_factor += exp( - ( distance * distance ) );
      }
      else
      {
        std::stringstream stream;
        stream << "Using unknown disease pocket kernel type \"" << type << "\"";
        throw std::runtime_error( stream.str() );
      }
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::from_json( const Json::Value &json )
  {
    this->position.from_json( json["position"] );
    this->position.set_centroid( this->get_town()->get_centroid() );
    
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

    bool sample_mode = this->get_population()->get_sample_mode();
    for( auto it = this->household_list.cbegin(); it != this->household_list.cend(); ++it )
    {
      household *h = *it;
      if( !sample_mode || h->is_selected() )
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
    bool sample_mode = this->get_population()->get_sample_mode();
    for( auto it = this->household_list.begin(); it != this->household_list.end(); ++it )
    {
      household *h = *it;
      if( !sample_mode || h->is_selected() ) h->to_csv( household_stream, individual_stream );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  unsigned int building::count_individuals() const
  {
    bool sample_mode = this->get_population()->get_sample_mode();
    unsigned int count = 0;
    for( auto it = this->household_list.begin(); it != this->household_list.end(); ++it )
      if( !sample_mode || (*it)->is_selected() ) count += (*it)->count_individuals();

    return count;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::select()
  {
    this->selected = true;
    this->get_town()->select();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building::unselect()
  {
    this->selected = false;
    for( auto it = this->household_list.begin(); it != this->household_list.end(); ++it )
      (*it)->unselect();
  }
}
