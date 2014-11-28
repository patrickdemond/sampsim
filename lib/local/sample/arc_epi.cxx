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

    // check to see if we have to move to the next sector and reset the current building if we do
    double size_fraction = static_cast< double >( this->get_size() ) /
                           static_cast< double >( this->get_number_of_sectors() );
    if( this->get_current_size() > size_fraction * this->get_current_sector() )
      this->current_building = NULL;

    if( NULL == this->current_building )
    {
      building_list_type building_list = tree.get_building_list();

      // 1. get list of all buildings in sector from random angle to random angle + the input
      //    parameter "arc angle"
      building_list_type initial_building_list;

      // 2. keep repeating step 2 until the list produced is not empty
      int iteration = 0;
      while( 0 == initial_building_list.size() && iteration < 100 )
      {
        // if a start angle hasn't been defined then pick a random start angle in the current sector
        if( !this->start_angle_defined )
        {
          std::pair< double, double > angles = this->get_next_sector_range();
          this->start_angle = utilities::random() * ( angles.second - angles.first ) + angles.first;

          if( utilities::verbose )
            utilities::output(
              "Beginning sector %d of %d with a starting angle of %0.3f radians",
              this->get_current_sector() - 1,
              this->get_number_of_sectors(),
              this->start_angle );
        }

        if( utilities::verbose )
          utilities::output( "iteration #%d", iteration + 1 );

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
            if( ( a1 <= a && a <= M_PI ) || ( -M_PI <= a && a < a2 ) )
              initial_building_list.push_back( *it );
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

        iteration++;
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
