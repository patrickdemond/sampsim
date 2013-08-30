/*=========================================================================

  Program:  sampsim
  Module:   epi.cxx
  Language: C++

=========================================================================*/

#include "epi.h"

#include "building.h"
#include "household.h"

#include <limits>
#include <stdexcept>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::list< household* >::iterator epi::select_next_household( std::list< household* > &list )
  {
    std::list< household* >::iterator household_it;

    // Steps 1, 2 and 3 are done by child classes in select_initial_household()
    if( NULL == this->current_household )
    {
      throw std::runtime_error(
        "Classes extending epi must select an initial household before passing "
        "reponsibility to the parent method" );
    }
    else
    {
      building *current_building = this->current_household->get_building();

      // 4. sample all households in that building
      bool found = false;
      for( auto it = list.begin(); it != list.end(); ++it )
      {
        if( (*it)->get_building() == current_building )
        {
          household_it = it;
          found = true;
        }
      }

      if( !found )
      {
        // 5. find the nearest building (distance measured in a straight path (as the crow flies))
        double min = std::numeric_limits<double>::max();
        for( auto it = list.begin(); it != list.end(); ++it )
        {
          double distance =
            current_building->get_position().distance( (*it)->get_building()->get_position() );
          if( distance < min )
          {
            household_it = it;
            min = distance;
          }
        }
      }
    }

    this->current_household = *household_it;
    return household_it;
  }
}
}
