/*=========================================================================

  Program:  sampsim
  Module:   gps.cxx
  Language: C++

=========================================================================*/

#include "gps.h"

#include "building_tree.h"

#include "town.h"

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* gps::select_next_building( sampsim::building_tree& tree )
  {
    // make sure the strip width has been set
    if( 0 >= this->radius )
      throw std::runtime_error( "Tried to sample without first setting the sampling radius" );
    
    building_list_type building_list = tree.get_building_list();
    building_list_type circle_building_list;
    coordinate centroid = ( *building_list.cbegin() )->get_town()->get_centroid();

    int iterations = 0;
    // keep selecting a random point until there is at least one building found in the resulting circle
    while( 0 == circle_building_list.size() && iterations < 100 )
    {
      // select a random point within the population's bounds
      coordinate p( 2 * centroid.x * utilities::random(), 2 * centroid.y * utilities::random() );
      if( utilities::verbose )
        utilities::output( "searching circle at %0.3f, %0.3f with radius %0.3f", p.x, p.y, this->radius );

      // make a list of all buildings inside the GPS circle
      for( auto it = building_list.cbegin(); it != building_list.cend(); ++it )
      {
        building* b = *it;
        if( this->radius >= b->get_position().distance( p ) ) circle_building_list.push_back( b );
      }

      if( 0 == circle_building_list.size() && utilities::verbose )
        utilities::output( "no buildings found in GPS circle" );

      iterations++;
    }

    if( 0 == circle_building_list.size() )
      throw std::runtime_error(
        "Unable to find building in GPS circle after 100 attempts (try increasing the radius)" );

    // select a random building from the list of those in the circle
    int index = utilities::random( 0, circle_building_list.size() - 1 );
    auto selected_it = circle_building_list.begin();
    std::advance( selected_it, index );
    if( utilities::verbose )
      utilities::output(
        "selecting building %d of %d in GPS circle",
        index + 1,
        circle_building_list.size() );

    // remove the building from the tree so it doesn't get selected twice
    return *selected_it;
  }
}
}
