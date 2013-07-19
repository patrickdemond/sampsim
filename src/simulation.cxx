/*=========================================================================

  Program:  sampsim
  Module:   simulation.cxx
  Language: C++

=========================================================================*/

#include "simulation.h"

#include "tile.h"
#include "trend.h"
#include "utilities.h"

#include <ctime>
#include <fstream>
#include <json/value.h>
#include <json/writer.h>
#include <map>
#include <utility>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  simulation::simulation()
  {
    this->number_tiles_x = 0;
    this->number_tiles_y = 0;
    this->tile_width = 0;
    this->mean_household_population = 0;
    this->mean_income = new trend;
    this->sd_income = new trend;
    this->population_density = new trend;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  simulation::~simulation()
  {
    // delete all tiles
    std::map< std::pair< int, int >, tile* >::iterator it;
    for( it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
      utilities::safe_delete( it->second );
    this->tile_list.clear();

    // delete all trends
    delete this->mean_income;
    delete this->sd_income;
    delete this->population_density;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void simulation::generate()
  {
    std::pair< int, int > index;
    clock_t time = clock();

    std::cout << "generating simulation" << std::endl;

    // create tiles
    for( int y = 0; y < this->number_tiles_y; y++ )
    {
      for( int x = 0; x < this->number_tiles_x; x++ )
      {
        // create the tile
        index = std::pair< int, int >( x, y );
        tile *t = new tile( this, index );
        t->set_income(
          this->mean_income->get_value( t->get_centroid() ),
          this->sd_income->get_value( t->get_centroid() ) );
        t->set_population_density( this->population_density->get_value( t->get_centroid() ) );
        t->generate();

        // store it at the specified index
        this->tile_list[index] = t;
      }
    }

    time = clock() - time;
    int population = this->get_population();
    double elapsed = (float)time / CLOCKS_PER_SEC;

    std::cout << "finished generating simulation: population " << population
              << " in " << this->get_area() << " km^2 area" << std::endl;
    std::cout << "processing time of " << elapsed << " seconds, generating "
         << population / elapsed << " individuals per second" << std::endl;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void simulation::write( std::string filename )
  {
    std::cout << "writting simulation to " << filename << std::endl;

    Json::Value root;
    this->to_json( root );
    std::ofstream stream( filename, std::ofstream::out );
    Json::StyledWriter writer;
    stream << writer.write( root );
    stream.close();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void simulation::to_json( Json::Value &json )
  {
    json = Json::Value( Json::objectValue );
    json["number_tiles_x"] = this->number_tiles_x;
    json["number_tiles_y"] = this->number_tiles_y;
    json["tile_width"] = this->tile_width;
    json["mean_household_population"] = this->mean_household_population;
    this->mean_income->to_json( json["mean_income"] );
    this->sd_income->to_json( json["sd_income"] );
    this->population_density->to_json( json["population_density"] );
    json["tile_list"] = Json::Value( Json::arrayValue );
    json["tile_list"].resize( this->tile_list.size() );

    int index = 0;
    std::map< std::pair< int, int >, tile* >::const_iterator it;
    for( it = this->tile_list.begin(); it != this->tile_list.end(); ++it, ++index )
      it->second->to_json( json["tile_list"][index] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void simulation::set_number_tiles_x( const int number_tiles_x )
  {
    if( utilities::verbose ) std::cout << "setting number_tiles_x to " << number_tiles_x << std::endl;
    this->number_tiles_x = number_tiles_x;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void simulation::set_number_tiles_y( const int number_tiles_y )
  {
    if( utilities::verbose ) std::cout << "setting number_tiles_y to " << number_tiles_y << std::endl;
    this->number_tiles_y = number_tiles_y;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void simulation::set_tile_width( const double tile_width )
  {
    if( utilities::verbose ) std::cout << "setting tile_width to " << tile_width << " meters" << std::endl;
    this->tile_width = tile_width;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void simulation::set_mean_household_population( const double mean_household_population )
  {
    if( utilities::verbose )
      std::cout << "setting mean_household_population to " << mean_household_population
                << " meters" << std::endl;
    this->mean_household_population = mean_household_population;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void simulation::set_income( const trend *mean, const trend *sd )
  {
    if( utilities::verbose )
      std::cout << "setting income trend mean to " << mean->to_string() << std::endl
                << "setting income trend sd to " << sd->to_string() << std::endl;
    this->mean_income->copy( mean );
    this->sd_income->copy( sd );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void simulation::set_population_density( const trend *population_density )
  {
    if( utilities::verbose )
      std::cout << "setting population density trend to " << population_density->to_string() << std::endl;
    this->population_density->copy( population_density );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  int simulation::get_population() const
  {
    int count = 0;
    std::map< std::pair< int, int >, tile* >::const_iterator it;
    for( it = this->tile_list.begin(); it != this->tile_list.end(); ++it )
      count += it->second->get_population();

    return count;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double simulation::get_area() const
  {
    return this->tile_width * this->tile_width * this->number_tiles_x * this->number_tiles_y;
  }
}
