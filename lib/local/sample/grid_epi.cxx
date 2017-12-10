/*=========================================================================

  Program:  sampsim
  Module:   grid_epi.cxx
  Language: C++

=========================================================================*/

#include "grid_epi.h"

#include "building.h"
#include "building_tree.h"
#include "population.h"
#include "town.h"

#include <json/value.h>
#include <json/writer.h>
#include <stdexcept>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void grid_epi::copy( const grid_epi* object )
  {
    this->set_number_of_squares( object->number_of_squares );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* grid_epi::select_next_building( sampsim::building_tree &tree )
  {
    building* b = NULL;

    if( NULL == this->current_building )
    {
      // 2. get list of all buildings in the area determined by the sub-algorithm in the chosen direction
      this->determine_initial_building_list( tree );
      if( 0 == this->initial_building_list.size() )
        throw std::runtime_error(
          "Unable to find initial building after 1000 attempts.  You must either lower the sample size or increase the initial selection area." );

      // 3. select a building from the list produced by step 2
      this->first_building_index = utilities::random( 0, this->initial_building_list.size() - 1 );
      auto initial_it = this->initial_building_list.begin();
      std::advance( initial_it, this->first_building_index );
      b = *initial_it;
      if( utilities::verbose )
        utilities::output(
          "selected building %d of %d in arc",
          this->first_building_index + 1,
          this->initial_building_list.size() );
    }
    else
    {
      b = epi::select_next_building( tree );
    }

    this->current_building = b;
    return b;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void grid_epi::determine_initial_building_list( sampsim::building_tree &tree )
  {
    // make sure the number of squares is set and greater than 0
    if( 0 >= this->number_of_squares )
      throw std::runtime_error( "Tried to sample without first setting the number of squares" );

    this->initial_building_list.clear();
    building_list_type building_list = tree.get_building_list();
    sampsim::town *town = ( *building_list.cbegin() )->get_town();

    // if we don't yet know the width of squares then this is the first iteration so we need to
    // calculate the width of squares and reset which have been selected
    if( 0 >= this->square_width_x ) this->determine_square_widths( town );

    // 2. select a random square in the grid and return all buildings found in that square
    int iteration = 0;
    do
    {
      coordinate lower, upper;
      unsigned int square_x = utilities::random( 1, this->number_of_squares );
      unsigned int square_y = utilities::random( 1, this->number_of_squares );

      lower.x = this->square_width_x * ( square_x - 1 );
      lower.y = this->square_width_y * ( square_y - 1 );
      upper.x = this->square_width_x * square_x;
      upper.y = this->square_width_y * square_y;

      if( utilities::verbose )
        utilities::output( "iteration #%d: searching square %d,%d with bounds [ %0.3f, %0.3f, %0.3f, %0.3f ]",
        iteration + 1,
        square_x,
        square_y,
        lower.x, lower.y,
        upper.x, upper.y );

      // make a list of all buildings inside the grid square
      for( auto it = building_list.cbegin(); it != building_list.cend(); ++it )
      {
        building* b = *it;
        coordinate c = b->get_position();
        if( lower.x <= c.x && c.x < upper.x &&
            lower.y <= c.y && c.y < upper.y ) this->initial_building_list.push_back( b );
      }

      if( 0 == this->initial_building_list.size() && utilities::verbose )
        utilities::output( "no buildings found in selected grid square" );

      iteration++;
    }
    while( 0 == this->initial_building_list.size() && iteration < 1000 );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void grid_epi::set_number_of_squares( unsigned int number_of_squares )
  {
    this->number_of_squares = number_of_squares;
    this->square_width_x = 0;
    this->square_width_y = 0;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void grid_epi::determine_square_widths( sampsim::town *town )
  {
    double tile_width = town->get_population()->get_tile_width();
    this->square_width_x = static_cast< double >( town->get_number_of_tiles_x() * tile_width ) /
                           static_cast< double >( this->number_of_squares );
    this->square_width_y = static_cast< double >( town->get_number_of_tiles_y() * tile_width ) /
                           static_cast< double >( this->number_of_squares );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void grid_epi::from_json( const Json::Value &json )
  {
    epi::from_json( json );
    this->set_number_of_squares( json["number_of_squares"].asUInt() );
    // square widths are automatically calculated by the set_number_of_squares() method
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void grid_epi::to_json( Json::Value &json ) const
  {
    epi::to_json( json );
    json["number_of_squares"] = this->number_of_squares;
    json["square_width_x"] = this->square_width_x;
    json["square_width_y"] = this->square_width_y;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string grid_epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << epi::get_csv_header();
    stream << "# number_of_squares: " << this->number_of_squares << std::endl;
    stream << "# square_width_x: " << this->square_width_x << std::endl;
    stream << "# square_width_y: " << this->square_width_y << std::endl;

    return stream.str();
  }
}
}
