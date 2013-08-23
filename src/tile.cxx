/*=========================================================================

  Program:  sampsim
  Module:   tile.cxx
  Language: C++

=========================================================================*/

#include "tile.h"

#include "building.h"
#include "population.h"
#include "utilities.h"

#include <algorithm>
#include <ostream>
#include <json/value.h>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  tile::tile( population *parent, const std::pair< int, int > index )
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
  void tile::generate()
  {
    if( utilities::verbose )
      utilities::output( "generating tile at %d, %d", this->index.first , this->index.second );

    // create the needed distributions
    this->income_distribution.set_lognormal( this->mean_income, this->sd_income );
    this->disease_risk_distribution.set_normal( this->mean_disease, this->sd_disease );

    // need to keep creating buildings until the population density is met
    int count = 0;
    while( static_cast< double >( this->count_population() ) / this->get_area() < this->population_density )
    {
      // create the building
      building *b = new building( this );
      b->generate();
      
      // store it in the building list
      this->building_list.push_back( b );
    }

    if( utilities::verbose )
      utilities::output( "finished generating tile: population %d in %d buildins",
                         this->count_population(),
                         this->building_list.size() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::from_json( const Json::Value &json )
  {
    this->index.first = json["x_index"].asInt();
    this->index.second = json["y_index"].asInt();
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
    json["building_list"].resize( this->building_list.size() );

    int index = 0;
    for( auto it = this->building_list.cbegin(); it != this->building_list.cend(); ++it, ++index )
      ( *it )->to_json( json["building_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    for( auto it = this->building_list.begin(); it != this->building_list.end(); ++it )
      ( *it )->to_csv( household_stream, individual_stream );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::set_index( const std::pair< int, int > index )
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
  int tile::count_population() const
  {
    int count = 0;
    for( auto it = this->building_list.begin(); it != this->building_list.end(); ++it )
      count += (*it)->count_population();

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
