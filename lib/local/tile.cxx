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
    this->has_river = false;
    this->has_river_cached = false;
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
    // to avoid over-populating a town we want half of the tile to stop adding buildings just after
    // they meet the density and the other half to stop adding buildings just before they meet the
    // density
    unsigned int count = 0;
    unsigned int total_individuals = 0;
    double current_density = 0;
    double area = this->get_area();
    bool stop_after = 0 != ( this->index.first + this->index.second ) % 2;

    while( current_density < this->population_density )
    {
      // create the building
      building *b = new building( this );
      b->create();
      total_individuals += b->count_individuals().second;
      current_density = static_cast< double >( total_individuals ) / area;

      // store it in the building list, but not if we are not stopping after the density is met
      // and this is the last building to be added
      if( !stop_after && current_density >= this->population_density )
      {
        utilities::safe_delete( b );
      }
      else
      {
        this->building_list.push_back( b );
      }
    }

    if( utilities::verbose )
      utilities::output( "finished creating tile: population %d in %d buildings",
                         this->count_individuals().second,
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
  std::pair<unsigned int, unsigned int> tile::count_individuals() const
  {
    bool sample_mode = this->get_population()->get_sample_mode();
    std::pair<unsigned int, unsigned int> count( 0, 0 );
    for( auto it = this->building_list.begin(); it != this->building_list.end(); ++it )
      if( !sample_mode || (*it)->is_selected() )
      {
        std::pair<unsigned int, unsigned int> sub_count = (*it)->count_individuals();
        count.first += sub_count.first;
        count.second += sub_count.second;
      }

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

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool tile::get_has_river()
  {
    if( !this->has_river_cached )
    {
      this->has_river = false;
      if( this->parent->get_has_river() )
      {
        line* river_banks = this->parent->get_river_banks();
        this->has_river =
          river_banks[0].line_inside_bounds( this->extent ) ||
          river_banks[1].line_inside_bounds( this->extent );
      }
      this->has_river_cached = true;
    }

    return this->has_river;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void tile::copy( const tile* object )
  {
    this->mean_income = object->mean_income;
    this->sd_income = object->sd_income;
    this->mean_disease = object->mean_disease;
    this->sd_disease = object->sd_disease;
    this->population_density = object->population_density;
    this->extent.first.copy( &( object->extent.first ) );
    this->extent.second.copy( &( object->extent.second ) );
    this->centroid.copy( &( object->centroid ) );
    this->income_distribution.copy( &( object->income_distribution ) );
    this->disease_risk_distribution.copy( &( object->disease_risk_distribution ) );
    this->has_river = object->has_river;
    this->has_river_cached = object->has_river_cached;
    this->population_density = object->population_density;

    // delete all buildings
    std::for_each( this->building_list.begin(), this->building_list.end(), utilities::safe_delete_type() );
    this->building_list.empty();

    bool sample_mode = this->get_population()->get_sample_mode();
    for( auto it = object->building_list.cbegin(); it != object->building_list.cend(); ++it )
    {
      if( !sample_mode || (*it)->is_selected() )
      {
        building *b = new building( this );
        b->copy( *it );
        this->building_list.push_back( b );
      }
    }
  }
}
