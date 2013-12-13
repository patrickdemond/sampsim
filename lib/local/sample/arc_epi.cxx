/*=========================================================================

  Program:  sampsim
  Module:   arc_epi.cxx
  Language: C++

=========================================================================*/

#include "arc_epi.h"

#include "building.h"
#include "building_tree.h"

#include <cmath>
#include <json/value.h>
#include <stdexcept>
#include <vector>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* arc_epi::select_next_building( sampsim::building_tree &tree )
  {
    // make sure the arc angle has been set
    if( 0 >= this->arc_angle )
      throw std::runtime_error( "Tried to sample without first setting the arc angle parameter" );

    building* b;

    if( NULL == this->current_building )
    {
      building_list_type building_list = tree.get_building_list();

      // 1. get list of all buildings in sector from random angle to random angle + the input
      //    parameter "arc angle"
      building_list_type initial_building_list;

      // 2. keep repeating step 2 until the list produced is not empty
      int iterations = 0;
      while( 0 == initial_building_list.size() && iterations < 100 )
      {
        // if a start angle hasn't been defined then pick a random start angle in [-PI, PI]
        if( !this->start_angle_defined )
          this->start_angle = utilities::random() * 2 * M_PI - M_PI;
        if( utilities::verbose )
          utilities::output( "selecting starting angle of %0.3f radians", this->start_angle );

        for( auto it = building_list.begin(); it != building_list.end(); ++it )
        {
          building *building = (*it);
          double a1 = this->start_angle - this->arc_angle / 2.0;
          double a2 = this->start_angle + this->arc_angle / 2.0;
          double a = (*it)->get_position().get_a();

          // it is possible that a2 > PI, if so then we need to loop around to -PI
          if( M_PI < a2 )
          {
            // translate a2 into the 3rd quadrant (in negative radians)
            a2 -= 2 * M_PI;

            // test from a1 to PI and from -PI to a2
            if( ( a1 <= a && a <= M_PI ) || ( -M_PI <= a && a < a2 ) ) initial_building_list.push_back( *it );
          }
          // it is possible that a1 < -PI, if so then we need to loop around to PI
          else if( -M_PI > a1 )
          {
            // translate a1 into the 2nd quadrant (in positive radians)
            a1 += 2 * M_PI;

            // test from a1 to PI and from -PI to a2
            if( ( a1 <= a && a <= M_PI ) || ( -M_PI <= a && a < a2 ) ) initial_building_list.push_back( *it );
          }
          else
          {
            // the test is simple, check if a is between angle and angle + arc_angle
            if( a1 <= a && a < a2 ) initial_building_list.push_back( *it );
          }
        }

        if( 0 == initial_building_list.size() && utilities::verbose ) 
          utilities::output( "no buildings found in strip" );

        iterations++;
      }

      if( 0 == initial_building_list.size() )
        throw std::runtime_error(
          "Unable to find initial building after 100 attempts (try increasing the arc angle)" );

      // 3. select a random building from the list produced by step 2
      this->first_building_index = utilities::random( 0, initial_building_list.size() - 1 );
      auto initial_it = initial_building_list.begin();
      std::advance( initial_it, this->first_building_index );
      b = *initial_it;
      if( utilities::verbose )
        utilities::output(
          "selecting building %d of %d in arc",
          this->first_building_index + 1, 
          initial_building_list.size() );
    }
    else
    {
      b = epi::select_next_building( tree );
    }

    this->current_building = b;
    return b;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void arc_epi::to_json( Json::Value &json ) const
  {
    epi::to_json( json );
    json["arc_angle"] = this->arc_angle;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string arc_epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << epi::get_csv_header();
    stream << "# arc_angle: " << this->arc_angle << std::endl;
    return stream.str();
  }
}
}
