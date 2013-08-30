/*=========================================================================

  Program:  sampsim
  Module:   strip_epi.cxx
  Language: C++

=========================================================================*/

#include "strip_epi.h"

#include "building.h"
#include "household.h"

#include <stdexcept>
#include <vector>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::list< household* >::iterator strip_epi::select_next_household( std::list< household* > &list )
  {
    // make sure the strip width has been set
    if( 0 >= this->strip_width )
      throw std::runtime_error( "Tried to sample without first setting the strip width parameter" );

    std::list< household* >::iterator household_it;

    if( NULL == this->current_household )
    {   
      // 1. get list of all buildings in rectangle formed by drawing a line from the centre
      //    of the population to the edge following the random angle, then widening that
      //    line into a rectangle the width of the input parameter "strip width" such that
      //    the original line runs along the centre of the rectagle
      std::vector< std::list< household* >::iterator > initial_households;

      // 2. keep repeating step 2 until the list produced is not empty
      while( 0 == initial_households.size() )
      {
        // pick a random angle in [-PI, PI]
        this->angle = utilities::random() * 2 * M_PI - M_PI;

        for( auto it = list.begin(); it != list.end(); ++it )
        {
          household *household = (*it);
          // TODO: implement
          initial_households.push_back( it );
        }
      }

      // 4. select a random building from the list produced by step 2
      auto initial_it = initial_households.begin();
      std::advance( initial_it, utilities::random( 0, initial_households.size() - 1 ) );
      household_it = *initial_it;
    }
    else
    {
      household_it = epi::select_next_household( list );
    }

    this->current_household = *household_it;
    return household_it;
  }
}
}
