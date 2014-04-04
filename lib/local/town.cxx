/*=========================================================================

  Program:  sampsim
  Module:   town.cxx
  Language: C++

=========================================================================*/

#include "town.h"

#include "building.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"
#include "trend.h"
#include "utilities.h"

#include <ctime>
#include <fstream>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include <stdexcept>
#include <utility>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  town::town( population *parent, const unsigned int index )
  {
    this->parent = parent;
    this->selected = false;
    this->index = index;
    this->number_of_tiles_x = 0;
    this->number_of_tiles_y = 0;

  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  town::~town()
  {
    // delete all tiles
    for( auto it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
      utilities::safe_delete( it->second );
    this->tile_list.clear();

    // we're holding a light reference to the parent, don't delete it
    this->parent = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::generate()
  {
    std::pair< unsigned int, unsigned int > index;

    std::stringstream stream;
    stream << "generating town #" << ( this->index + 1 );
    utilities::output( stream.str() );

    population *pop = this->get_population();

    // delete all tiles and turn off sample mode (in case it is on)
    for( auto it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
      utilities::safe_delete( it->second );
    this->tile_list.clear();

    // create the needed distributions
    this->population_distribution.set_poisson( this->mean_household_population - 1 );

    // create tiles
    for( unsigned int y = 0; y < this->number_of_tiles_y; y++ )
    {
      for( unsigned int x = 0; x < this->number_of_tiles_x; x++ )
      {
        // create the tile
        index = std::pair< unsigned int, unsigned int >( x, y );
        tile *t = new tile( this, index );
        t->set_mean_income( pop->get_mean_income()->get_value( t->get_centroid() ) );
        t->set_sd_income( pop->get_sd_income()->get_value( t->get_centroid() ) );
        t->set_mean_disease( pop->get_mean_disease()->get_value( t->get_centroid() ) );
        t->set_sd_disease( pop->get_sd_disease()->get_value( t->get_centroid() ) );
        t->set_population_density( pop->get_population_density()->get_value( t->get_centroid() ) );
        t->generate();

        // store it at the specified index
        this->tile_list[index] = t;
      }
    }

    // now that the town has been generated we can determine disease status
    // we are going to do this in a standard generalized-linear-model way, by constructing a linear
    // function of the various contributing factors
    utilities::output( "determining disease status" );

    // create a matrix of all participants (rows) and their various disease predictor factors
    const unsigned int number_of_individuals = this->count_individuals();
    const unsigned int number_of_disease_weights = pop->get_number_of_disease_weights();
    double value[number_of_disease_weights], total[number_of_disease_weights];
    for( unsigned int c = 0; c < number_of_disease_weights; c++ ) total[c] = 0;
    
    std::vector< double > matrix[number_of_disease_weights];
    for( unsigned int c = 0; c < number_of_disease_weights; c++ )
      matrix[c].resize( number_of_individuals );
    individual_list_type individual_list;
    individual_list.resize( number_of_individuals );
    
    unsigned int individual_index = 0;

    for( auto tile_it = this->tile_list.begin();
         tile_it != this->tile_list.end();
         ++tile_it )
    {
      for( auto building_it = tile_it->second->get_building_list_cbegin();
           building_it != tile_it->second->get_building_list_cend();
           ++building_it )
      {
        value[5] = ( *building_it )->get_pocket_factor();
        for( auto household_it = ( *building_it )->get_household_list_cbegin();
             household_it != ( *building_it )->get_household_list_cend();
             ++household_it )
        {
          value[0] = ( *household_it )->count_individuals();
          value[1] = ( *household_it )->get_income();
          value[2] = ( *household_it )->get_disease_risk();

          for( auto individual_it = ( *household_it )->get_individual_list_cbegin();
               individual_it != ( *household_it )->get_individual_list_cend();
               ++individual_it )
          {
            value[3] = ADULT == ( *individual_it )->get_age() ? 1 : 0;
            value[4] = MALE == ( *individual_it )->get_sex() ? 1 : 0;

            for( unsigned int c = 0; c < number_of_disease_weights; c++ )
            {
              total[c] += value[c];
              matrix[c][individual_index] = value[c];
            }

            // keep a reference to the individual for writing to later
            individual_list[individual_index] = ( *individual_it );
            
            individual_index++;
          }
        }
      }
    }

    // subtract the mean of a column from each of its values and divide by the column's sd
    double mean[number_of_disease_weights], sd[number_of_disease_weights];

    for( unsigned int c = 0; c < number_of_disease_weights; c++ )
    {
      mean[c] = total[c] / number_of_individuals;
      sd[c] = 0;
      for( unsigned int i = 0; i < number_of_individuals; i++ )
        sd[c] += ( matrix[c][i] - mean[c] ) * ( matrix[c][i] - mean[c] );
      sd[c] = sqrt( sd[c] / ( number_of_individuals - 1 ) );

      for( unsigned int i = 0; i < number_of_individuals; i++ )
        matrix[c][i] = ( matrix[c][i] - mean[c] ) / sd[c];
    }

    // factor in weights, compute disease probability then set disease status for all individuals
    for( unsigned int i = 0; i < number_of_individuals; i++ )
    {
      double eta = 0, probability;

      for( unsigned int c = 0; c < number_of_disease_weights; c++ )
        eta += matrix[c][i] * pop->get_disease_weight_by_index( c );
      probability = 1 / ( 1 + exp( -eta ) );
      individual_list[i]->set_disease( utilities::random() < probability );
    }

    stream.str( "" );
    stream << "finished generating town #" << ( this->index + 1 );
    utilities::output( stream.str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::from_json( const Json::Value &json )
  {
    // check to make sure the version is compatible
    if( !json.isMember( "version" ) )
      throw std::runtime_error( "ERROR: Cannot determine population generator version" );
    std::string version = json["version"].asString();
    if( version != utilities::get_version() )
    {
      std::stringstream stream;
      stream << "ERROR: Cannot read population, incompatible version ("
             << version << " != " << utilities::get_version() << ")";
      throw std::runtime_error( stream.str() );
    }

    this->index = json["index"].asUInt();
    this->number_of_tiles_x = json["number_of_tiles_x"].asUInt();
    this->number_of_tiles_y = json["number_of_tiles_y"].asUInt();

    std::pair< unsigned int, unsigned int > index;
    for( unsigned int c = 0; c < json["tile_list"].size(); c++ )
    {
      Json::Value tile_json = json["tile_list"][c];
      index = std::pair< unsigned int, unsigned int >(
        tile_json["x_index"].asUInt(), tile_json["y_index"].asUInt() );
      tile *t = new tile( this, index );
      t->from_json( tile_json );
      this->tile_list[index] = t;
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["version"] = utilities::get_version();
    json["index"] = this->index;
    json["number_of_tiles_x"] = this->number_of_tiles_x;
    json["number_of_tiles_y"] = this->number_of_tiles_y;
    json["tile_list"] = Json::Value( Json::arrayValue );
    json["tile_list"].resize( this->tile_list.size() );

    unsigned int index = 0;
    for( auto it = this->tile_list.cbegin(); it != this->tile_list.cend(); ++it, ++index )
      it->second->to_json( json["tile_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::to_csv(
    std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    // need to reset the static household indexing variable
    utilities::household_index = 0;

    // put in the parameters
    std::stringstream stream;
    stream << "#" << std::endl
           << "# town parameters (index " << this->index << ")" << std::endl
           << "# --------------------------" << std::endl
           << "# number_of_tiles_x: " << this->number_of_tiles_x << std::endl
           << "# number_of_tiles_y: " << this->number_of_tiles_y << std::endl
           << "#" << std::endl;
    
    household_stream << stream.str();
    individual_stream << stream.str();

    for( auto it = this->tile_list.cbegin(); it != this->tile_list.cend(); ++it )
      it->second->to_csv( household_stream, individual_stream );

    household_stream << std::endl;
    individual_stream << std::endl;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_number_of_tiles_x( const unsigned int number_of_tiles_x )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_tiles_x to %d", number_of_tiles_x );
    this->number_of_tiles_x = number_of_tiles_x;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_number_of_tiles_y( const unsigned int number_of_tiles_y )
  {
    if( utilities::verbose ) utilities::output( "setting number_of_tiles_y to %d", number_of_tiles_y );
    this->number_of_tiles_y = number_of_tiles_y;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_number_of_disease_pockets( const unsigned int count )
  {
    if( utilities::verbose ) utilities::output( "setting number of disease pockets to %d", count );
    this->disease_pocket_list.clear();
    coordinate c = this->get_centroid();
    for( unsigned int i = 0; i < count; i++ )
      this->disease_pocket_list.push_back(
        coordinate( 2 * c.x * utilities::random(), 2 * c.y * utilities::random() ) );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::set_mean_household_population( const double mean_household_population )
  {
    if( utilities::verbose )
      utilities::output( "setting mean_household_population to %f", mean_household_population );
    this->mean_household_population = mean_household_population;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  unsigned int town::count_individuals() const
  {
    unsigned int count = 0;
    for( auto it = this->tile_list.cbegin(); it != this->tile_list.cend(); ++it )
      count += it->second->count_individuals();

    return count;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  coordinate town::get_centroid() const
  {
    coordinate centroid( this->number_of_tiles_x, this->number_of_tiles_y );
    centroid *= this->get_population()->get_tile_width() / 2;
    return centroid;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double town::get_area() const
  {
    double tile_width = this->get_population()->get_tile_width();
    return tile_width * tile_width * this->number_of_tiles_x * this->number_of_tiles_y;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void town::unselect()
  {
    this->selected = false;

    // unselect all buildings
    for( auto tile_it = this->tile_list.begin(); tile_it != this->tile_list.end(); ++tile_it )
      for( auto building_it = tile_it->second->get_building_list_cbegin();
           building_it != tile_it->second->get_building_list_cend();
           ++building_it )
        ( *building_it )->unselect();
  }
}
