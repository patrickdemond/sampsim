/*=========================================================================

  Program:  sampsim
  Module:   arc_epi.cxx
  Language: C++

=========================================================================*/

#include "arc_epi.h"

#include "building.h"
#include "household.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  arc_epi::arc_epi()
  {
    this->angle = 0;
    this->arc_angle = 0;
    this->current_household = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::list< household* >::iterator arc_epi::select_next_household( std::list< household* > &list )
  {
    // make sure the arc angle has been set
    if( 0 >= this->arc_angle )
      throw std::runtime_error( "Tried to sample without first setting the arc angle parameter" );

    std::list< household* >::iterator household_it;

    // 1. select a random angle between 0 and 360 (done in the constructor)
    // only do this step if we haven't already chosen a household
    if( NULL == this->current_household )
    {
      // 2. get list of all buildings in sector from random angle to random angle + the input
      //    parameter "arc angle"
      std::vector< std::list< household* >::iterator > initial_households;

      // 3. keep repeating step 2 until the list produced is not empty
      while( 0 == initial_households.size() )
      {
        // pick a random angle in [-PI, PI]
        this->angle = utilities::random() * 2 * M_PI - M_PI;

        for( auto it = list.begin(); it != list.end(); ++it )
        {
          household *household = (*it);
          double a1 = this->angle;
          double a2 = this->angle + this->arc_angle;
          double a = (*it)->get_building()->get_position().get_a();

          // it is possible that a2 > PI, if so then we need to loop around to -PI
          if( M_PI < a2 )
          {
            // translate a2 into the 4th quadrant
            a2 -= 2 * M_PI;

            // test from a1 to PI and from -PI to a2
            if( ( a1 <= a && a <= M_PI ) || ( -M_PI <= a && a < a2 ) ) initial_households.push_back( it );
          }
          else
          {
            // the test is simple, check if a is between angle and angle + arc_angle
            if( a1 <= a && a < a2 ) initial_households.push_back( it );
          }
        }
      }

      // 4. select a random building from the list produced by step 2
      auto initial_it = initial_households.begin();
      std::advance( initial_it, utilities::random( 0, initial_households.size() - 1 ) );
      household_it = *initial_it;
    }
    else
    {
      building *current_building = this->current_household->get_building();

      // 5. sample all households in that building
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
        // 6. find the nearest building (distance measured in a straight path (as the crow flies))
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
