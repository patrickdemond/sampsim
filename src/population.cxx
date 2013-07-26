/*=========================================================================

  Program:  sampsim
  Module:   population.cxx
  Language: C++

=========================================================================*/

#include "population.h"

#include "building.h"
#include "household.h"
#include "individual.h"
#include "tile.h"
#include "trend.h"
#include "utilities.h"

#include <ctime>
#include <fstream>
#include <json/value.h>
#include <json/writer.h>
#include <utility>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population::population()
  {
    this->seed = "";
    this->number_tiles_x = 0;
    this->number_tiles_y = 0;
    this->tile_width = 0;
    for( unsigned int c = 0; c < population::number_of_weights; c++ ) this->disease_weights[c] = 1.0;
    this->mean_household_population = 0;
    this->mean_income = new trend;
    this->sd_income = new trend;
    this->mean_disease = new trend;
    this->sd_disease = new trend;
    this->population_density = new trend;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  population::~population()
  {
    // delete all tiles
    for( tile_list_type::iterator it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
      utilities::safe_delete( it->second );
    this->tile_list.clear();

    // delete all trends
    delete this->mean_income;
    delete this->sd_income;
    delete this->mean_disease;
    delete this->sd_disease;
    delete this->population_density;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::generate()
  {
    std::pair< int, int > index;

    utilities::output( "generating population" );

    // create tiles
    for( int y = 0; y < this->number_tiles_y; y++ )
    {
      for( int x = 0; x < this->number_tiles_x; x++ )
      {
        // create the tile
        index = std::pair< int, int >( x, y );
        tile *t = new tile( this, index );
        t->set_mean_income( this->mean_income->get_value( t->get_centroid() ) );
        t->set_sd_income( this->sd_income->get_value( t->get_centroid() ) );
        t->set_mean_disease( this->mean_disease->get_value( t->get_centroid() ) );
        t->set_sd_disease( this->sd_disease->get_value( t->get_centroid() ) );
        t->set_population_density( this->population_density->get_value( t->get_centroid() ) );
        t->generate();

        // store it at the specified index
        this->tile_list[index] = t;
      }
    }

    // now that the population has been generated we can determine disease status
    // we are going to do this in a standard generalized-linear-model way, by constructing a linear
    // function of the various contributing factors
    utilities::output( "determining disease status" );

    // create a matrix of all participants (rows) and their various disease predictor factors
    const int population_size = this->count_population();
    individual *ind;
    household *house;
    double value[population::number_of_weights], total[population::number_of_weights];
    for( unsigned int c = 0; c < population::number_of_weights; c++ ) total[c] = 0;
    
    std::vector< double > matrix[population::number_of_weights];
    for( unsigned int c = 0; c < population::number_of_weights; c++ )
      matrix[c].resize( population_size );
    std::vector< individual* > individual_list;
    individual_list.resize( population_size );
    
    int individual_index = 0;

    tile_list_type::const_iterator tile_it;
    building_list_type::const_iterator building_it;
    household_list_type::const_iterator household_it;
    individual_list_type::const_iterator individual_it;

    for( tile_it = this->tile_list.begin();
         tile_it != this->tile_list.end();
         ++tile_it )
    {
      for( building_it = tile_it->second->get_building_list_cbegin();
           building_it != tile_it->second->get_building_list_cend();
           ++building_it )
      {
        for( household_it = ( *building_it )->get_household_list_cbegin();
             household_it != ( *building_it )->get_household_list_cend();
             ++household_it )
        {
          house = ( *household_it );
          value[0] = house->count_population();
          value[1] = house->get_income();
          value[2] = house->get_disease_risk();

          for( individual_it = ( *household_it )->get_individual_list_cbegin();
               individual_it != ( *household_it )->get_individual_list_cend();
               ++individual_it )
          {
            ind = ( *individual_it );
            value[3] = ind->is_adult() ? 1 : 0;
            value[4] = ind->is_male() ? 1 : 0;

            for( unsigned int c = 0; c < population::number_of_weights; c++ )
            {
              total[c] += value[c];
              matrix[c][individual_index] = value[c];
            }

            // keep a reference to the individual for writing to later
            individual_list[individual_index] = ind;
            
            individual_index++;
          }
        }
      }
    }

    // subtract the mean of a column from each of its values and divide by the column's sd
    double mean[population::number_of_weights], sd[population::number_of_weights];

    for( unsigned int c = 0; c < population::number_of_weights; c++ )
    {
      mean[c] = total[c] / population_size;
      sd[c] = 0;
      for( unsigned int i = 0; i < population_size; i++ )
        sd[c] += ( matrix[c][i] - mean[c] ) * ( matrix[c][i] - mean[c] );
      sd[c] = sqrt( sd[c] / ( population_size - 1 ) );

      for( unsigned int i = 0; i < population_size; i++ )
        matrix[c][i] = ( matrix[c][i] - mean[c] ) / sd[c];
    }

    // factor in weights, compute disease probability then set disease status for all individuals
    for( unsigned int i = 0; i < population_size; i++ )
    {
      double eta = 0, probability;
      for( unsigned int c = 0; c < population::number_of_weights; c++ )
        eta += matrix[c][i] * this->disease_weights[c];
      probability = 1 / ( 1 + exp( -eta ) );
      individual_list[i]->set_disease( utilities::random() < probability );
    }

    utilities::output( "finished generating population" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::write( const std::string filename, const bool flat_file ) const
  {
    utilities::output( "writting population to %s", filename.c_str() );

    if( flat_file )
    {
      std::ofstream household_stream( filename + ".household.csv", std::ofstream::out );
      std::ofstream individual_stream( filename + ".individual.csv", std::ofstream::out );
      this->to_csv( household_stream, individual_stream );
      household_stream.close();
      individual_stream.close();
    }
    else
    {
      std::ofstream stream( filename, std::ofstream::out );
      Json::Value root;
      this->to_json( root );
      Json::StyledWriter writer;
      stream << writer.write( root );
      stream.close();
    }

    utilities::output( "finished writting population" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["seed"] = this->seed;
    json["number_tiles_x"] = this->number_tiles_x;
    json["number_tiles_y"] = this->number_tiles_y;
    json["tile_width"] = this->tile_width;
    json["disease_weights"] = Json::Value( Json::arrayValue );
    json["disease_weights"].resize( population::number_of_weights );
    for( unsigned int c = 0; c < population::number_of_weights; c++ )
      json["disease_weights"][c] = this->disease_weights[c];
    json["mean_household_population"] = this->mean_household_population;
    this->mean_income->to_json( json["mean_income"] );
    this->sd_income->to_json( json["sd_income"] );
    this->mean_disease->to_json( json["mean_disease"] );
    this->sd_disease->to_json( json["sd_disease"] );
    this->population_density->to_json( json["population_density"] );
    json["tile_list"] = Json::Value( Json::arrayValue );
    json["tile_list"].resize( this->tile_list.size() );

    int index = 0;
    for( tile_list_type::const_iterator it = this->tile_list.cbegin();
         it != this->tile_list.cend();
         ++it, ++index )
      it->second->to_json( json["tile_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::to_csv( std::ofstream &household_stream, std::ofstream &individual_stream ) const
  {
    // need to reset the static household indexing variable
    utilities::household_index = 0;

    for( tile_list_type::const_iterator it = this->tile_list.cbegin(); it != this->tile_list.cend(); ++it )
      it->second->to_csv( household_stream, individual_stream );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_seed( const std::string seed )
  {
    this->seed = seed;
    utilities::random_engine.seed( atoi( this->seed.c_str() ) );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_number_tiles_x( const int number_tiles_x )
  {
    if( utilities::verbose ) utilities::output( "setting number_tiles_x to %d", number_tiles_x );
    this->number_tiles_x = number_tiles_x;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_number_tiles_y( const int number_tiles_y )
  {
    if( utilities::verbose ) utilities::output( "setting number_tiles_y to %d", number_tiles_y );
    this->number_tiles_y = number_tiles_y;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_tile_width( const double tile_width )
  {
    if( utilities::verbose ) utilities::output( "setting tile_width to %f meters", tile_width );
    this->tile_width = tile_width;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_mean_household_population( const double mean_household_population )
  {
    if( utilities::verbose )
      utilities::output( "setting mean_household_population to %f", mean_household_population );
    this->mean_household_population = mean_household_population;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_income( const trend *mean, const trend *sd )
  {
    if( utilities::verbose )
    {
      utilities::output( "setting income trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting income trend sd to %s", sd->to_string().c_str() );
      utilities::output( "setting income trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting income trend sd to %s", sd->to_string().c_str() );
    }
    this->mean_income->copy( mean );
    this->sd_income->copy( sd );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_disease( const trend *mean, const trend *sd )
  {
    if( utilities::verbose )
    {
      utilities::output( "setting disease trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting disease trend sd to %s", sd->to_string().c_str() );
      utilities::output( "setting disease trend mean to %s", mean->to_string().c_str() );
      utilities::output( "setting disease trend sd to %s", sd->to_string().c_str() );
    }
    this->mean_disease->copy( mean );
    this->sd_disease->copy( sd );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void population::set_population_density( const trend *population_density )
  {
    if( utilities::verbose )
      utilities::output( "setting population density trend to %s", population_density->to_string().c_str() );
    this->population_density->copy( population_density );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  int population::count_population() const
  {
    int count = 0;
    for( tile_list_type::const_iterator it = this->tile_list.cbegin(); it != this->tile_list.cend(); ++it )
      count += it->second->count_population();

    return count;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double population::get_area() const
  {
    return this->tile_width * this->tile_width * this->number_tiles_x * this->number_tiles_y;
  }
}
