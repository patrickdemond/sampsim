/*=========================================================================

  Program:  sampsim
  Module:   square_gps.cxx
  Language: C++

=========================================================================*/

#include "square_gps.h"

#include "building_tree.h"

#include "population.h"
#include "town.h"

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* square_gps::select_next_building( sampsim::building_tree& tree )
  {
    // make sure the strip width has been set
    if( 0 >= this->number_of_squares )
      throw std::runtime_error( "Tried to sample without first setting the number of squares" );
    
    building_list_type building_list = tree.get_building_list();
    building_list_type square_building_list;
    sampsim::town *town = ( *building_list.cbegin() )->get_town();
    coordinate centroid = town->get_centroid();

    // if we don't yet know the width of squares then this is the first iteration so we need to
    // calculate the width of squares and reset which have been selected
    if( 0 >= this->square_width_x )
    {
      this->determine_square_widths( town );
      this->reset_selected_squares();
    }

    int iteration = 0;
    // keep selecting a random point until there is at least one building found in the resulting square
    while( 0 == square_building_list.size() && iteration < 100 )
    {
      // select a random point within the population's bounds
      coordinate p( 2 * centroid.x * utilities::random(), 2 * centroid.y * utilities::random() );

      // determine the bounds of the square the random point falls into
      coordinate lower, upper;
      unsigned int index_x = floor( p.x / this->square_width_x );
      unsigned int index_y = floor( p.y / this->square_width_y );

      // only select a building from this square if it hasn't yet been selected
      if( !this->select_square( index_x, index_y ) )
      {
        lower.x = index_x * this->square_width_x; 
        lower.y = index_y * this->square_width_y; 
        upper.x = lower.x + this->square_width_x;
        upper.y = lower.y + this->square_width_y;

        if( utilities::verbose )
          utilities::output(
            "iteration #%d: searching square at %0.3f, %0.3f with bounds [ %0.3f, %0.3f, %0.3f, %0.3f ]",
            iteration + 1,
            p.x, p.y,
            lower.x, lower.y,
            upper.x, upper.y );

        // make a list of all buildings inside the GPS square
        for( auto it = building_list.cbegin(); it != building_list.cend(); ++it )
        {
          building* b = *it;
          coordinate c = b->get_position();
          if( lower.x <= c.x && c.x < upper.x &&
              lower.y <= c.y && c.y < upper.y ) square_building_list.push_back( b );
        }

        if( 0 == square_building_list.size() && utilities::verbose )
          utilities::output( "no buildings found in GPS square" );
      }

      iteration++;
    }

    if( 0 == square_building_list.size() )
    {
      if( this->all_squares_selected() )
      { // all squares have been selected
        throw std::runtime_error(
          "No more unselected squares left before completing sample (try increasing the number of squares)" );
      }
      else
      { // none of the selected squares have any buildings in them
        throw std::runtime_error(
          "Unable to find building in GPS square after 100 attempts (try decreasing the number of squares)" );
      }
    }

    // select a random building from the list of those in the square
    int index = utilities::random( 0, square_building_list.size() - 1 );
    auto selected_it = square_building_list.begin();
    std::advance( selected_it, index );
    if( utilities::verbose )
      utilities::output(
        "selecting building %d of %d in GPS square",
        index + 1,
        square_building_list.size() );

    // remove the building from the tree so it doesn't get selected twice
    return *selected_it;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void square_gps::set_number_of_squares( unsigned int number_of_squares )
  {
    this->number_of_squares = number_of_squares;
    this->square_width_x = 0;
    this->square_width_y = 0;
    this->reset_selected_squares();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void square_gps::determine_square_widths( sampsim::town *town )
  {
    double tile_width = town->get_population()->get_tile_width();
    this->square_width_x = static_cast< double >( town->get_number_of_tiles_x() * tile_width ) /
                           static_cast< double >( this->number_of_squares );
    this->square_width_y = static_cast< double >( town->get_number_of_tiles_y() * tile_width ) /
                           static_cast< double >( this->number_of_squares );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void square_gps::reset_selected_squares()
  {
    this->selected_squares.assign( 
      this->number_of_squares, 
      std::vector< bool >( this->number_of_squares, false ) );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool square_gps::select_square( const unsigned int index_x, const unsigned int index_y )
  {
    bool selected = this->selected_squares.at( index_x ).at( index_y );
    if( !selected ) this->selected_squares[index_x][index_y] = true;
    return selected;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool square_gps::all_squares_selected() const
  {
    for( auto itx = this->selected_squares.cbegin(); itx != this->selected_squares.cend(); ++itx )
      for( auto ity = itx->cbegin(); ity != itx->cend(); ++ity )
        if( ! *ity ) return false;

    return true;
  }
}
}