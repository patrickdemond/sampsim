/*=========================================================================

  Program:  sampsim
  Module:   tile.cxx
  Language: C++

=========================================================================*/

#include "tile.h"

#include "building.h"
#include "population.h"
#include "town.h"
#include "utilities.h"

#include <algorithm>
#include <json/value.h>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  tile::tile( town *parent, const std::pair< unsigned int, unsigned int > index )
  {
    this->parent = parent;
    this->set_index( index );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  tile::~tile()
  {
    // delete all buildings
    std::for_each( this->building_list.begin(), this->building_list.end(), utilities::safe_delete_type() );
    this->building_list.empty();

    // we're holding a light reference to the parent, don't delete it
    this->parent = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population* tile::get_population() const
  {
    return NULL == this->parent ? NULL : this->parent->get_population();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::create()
  {
    if( utilities::verbose )
      utilities::output( "creating tile at %d, %d", this->index.first , this->index.second );

    // need to keep creating buildings until the population density is met
    unsigned int count = 0;
    unsigned int total_individuals = 0;
    while( static_cast< double >( total_individuals ) / this->get_area() < this->population_density )
    {
      // create the building
      building *b = new building( this );
      b->create();
      total_individuals += b->count_individuals();

      // store it in the building list
      this->building_list.push_back( b );
    }

    if( utilities::verbose )
      utilities::output( "finished creating tile: population %d in %d buildings",
                         this->count_individuals(),
                         this->building_list.size() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::define()
  {
    if( utilities::verbose )
      utilities::output( "defining tile at %d, %d", this->index.first , this->index.second );

    // create the needed distributions
    this->income_distribution.set_lognormal( this->mean_income, this->sd_income );
    this->disease_risk_distribution.set_normal( this->mean_disease, this->sd_disease );

    for( auto it = this->building_list.begin(); it != this->building_list.end(); ++it ) (*it)->define();

    if( utilities::verbose ) utilities::output( "finished defining tile" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::from_json( const Json::Value &json )
  {
    this->index.first = json["x_index"].asUInt();
    this->index.second = json["y_index"].asUInt();
    this->mean_income = json["mean_income"].asDouble();
    this->sd_income = json["sd_income"].asDouble();
    this->mean_disease = json["mean_disease"].asDouble();
    this->sd_disease = json["sd_disease"].asDouble();
    this->population_density = json["population_density"].asDouble();

    this->building_list.reserve( json["building_list"].size() );
    for( unsigned int c = 0; c < json["building_list"].size(); c++ )
    {
      building *b = new building( this );
      b->from_json( json["building_list"][c] );
      this->building_list.push_back( b );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["x_index"] = this->index.first;
    json["y_index"] = this->index.second;
    json["mean_income"] = this->mean_income;
    json["sd_income"] = this->sd_income;
    json["mean_disease"] = this->mean_disease;
    json["sd_disease"] = this->sd_disease;
    json["population_density"] = this->population_density;
    json["building_list"] = Json::Value( Json::arrayValue );

    bool sample_mode = this->get_population()->get_sample_mode();
    unsigned int index = 0;
    for( auto it = this->building_list.cbegin(); it != this->building_list.cend(); ++it )
    {
      building *b = *it;
      if( !sample_mode || b->is_selected() )
      {
        Json::Value child;
        b->to_json( child );
        json["building_list"].append( child );
      }
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    bool sample_mode = this->get_population()->get_sample_mode();
    for( auto it = this->building_list.begin(); it != this->building_list.end(); ++it )
    {
      building *b = *it;
      if( !sample_mode || b->is_selected() ) b->to_csv( household_stream, individual_stream );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::set_index( const std::pair< unsigned int, unsigned int > index )
  {
    // make sure the tile has a parent
    if( NULL == this->parent ) throw std::runtime_error( "Tried to set the index of an orphaned tile" );

    this->index = index;

    // determine the extent
    double width = this->get_population()->get_tile_width();
    coordinate lower( this->index.first, this->index.second );
    lower *= width;
    coordinate upper( this->index.first + 1, this->index.second + 1 );
    upper *= width;

    this->extent = std::pair< coordinate, coordinate >( lower, upper );
    this->centroid = ( lower + upper ) / 2;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  unsigned int tile::count_individuals() const
  {
    bool sample_mode = this->get_population()->get_sample_mode();
    unsigned int count = 0;
    for( auto it = this->building_list.begin(); it != this->building_list.end(); ++it )
      if( !sample_mode || (*it)->is_selected() ) count += (*it)->count_individuals();

    return count;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double tile::get_area() const
  {
    // make sure the tile has a parent
    if( NULL == this->parent ) throw std::runtime_error( "Tried to get the area of an orphaned tile" );

    double width = this->get_population()->get_tile_width();
    return width * width;
  }
}
