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
#include <fstream>
#include <json/value.h>
#include <vector>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  tile::tile( population *parent, std::pair< int, int > index )
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
    this->parent = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::generate()
  {
    if( utilities::verbose )
      utilities::output( "generating tile at %d, %d", this->index.first , this->index.second );

    // need to keep creating buildings until the population density is met
    int count = 0;
    while( static_cast< double >( this->get_population() ) / this->get_area() < this->population_density )
    {
      // create the building
      building *b = new building( this );
      b->generate();
      
      // store it in the building list
      this->building_list.push_back( b );
    }

    if( utilities::verbose )
      utilities::output( "finished generating tile: population %d in %d buildins",
                         this->get_population(),
                         this->building_list.size() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::to_json( Json::Value &json )
  {
    json = Json::Value( Json::objectValue );
    json["x_index"] = this->index.first;
    json["y_index"] = this->index.second;
    json["income_mean"] = this->income.first;
    json["income_sd"] = this->income.second;
    json["population_density"] = this->population_density;
    json["building_list"] = Json::Value( Json::arrayValue );
    json["building_list"].resize( this->building_list.size() );

    int index = 0;
    std::vector< building* >::const_iterator it;
    for( it = this->building_list.begin(); it != this->building_list.end(); ++it, ++index )
      ( *it )->to_json( json["building_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::to_csv( std::ofstream &household_stream, std::ofstream &individual_stream )
  {
    std::vector< building* >::const_iterator it;
    for( it = this->building_list.begin(); it != this->building_list.end(); ++it )
      ( *it )->to_csv( household_stream, individual_stream );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::set_index( const std::pair< int, int > index )
  {
    this->index = index;

    // determine the extent
    double width = this->parent->get_tile_width();
    coordinate lower( this->index.first, this->index.second );
    lower *= width;
    coordinate upper( this->index.first + 1, this->index.second + 1 );
    upper *= width;

    this->extent = std::pair< coordinate, coordinate >( lower, upper );
    this->centroid = ( lower + upper ) / 2;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  int tile::get_population() const
  {
    int count = 0;
    std::vector< building* >::const_iterator it;
    for( it = this->building_list.begin(); it != this->building_list.end(); ++it )
      count += (*it)->get_population();

    return count;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double tile::get_area() const
  {
    double width = this->parent->get_tile_width();
    return width * width;
  }
}
