/*=========================================================================

  Program:  sampsim
  Module:   arc_epi.cxx
  Language: C++

=========================================================================*/

#include "arc_epi.h"

#include "building.h"

#include <cmath>
#include <json/value.h>
#include <json/writer.h>
#include <stdexcept>
#include <vector>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void arc_epi::copy( const arc_epi* object )
  {
    this->arc_angle = object->arc_angle;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void arc_epi::determine_initial_building_list( building_list_type &building_list )
  {
    this->initial_building_list.clear();

    coordinate c = this->get_angle_centroid();

    // 2. get list of all buildings in the arc defined by the start angle and arc width
    int iteration = 0;
    while( 0 == this->initial_building_list.size() && iteration < 1000 )
    {
      if( utilities::verbose )
        utilities::output( "iteration #%d", iteration + 1 );

      double a1 = safe_subtract( this->start_angle, this->arc_angle / 2.0 );
      double a2 = this->start_angle + this->arc_angle / 2.0;

      // it is possible that a1 < -PI, if so then we need to loop around to PI
      // translate a1 into the 2nd quadrant (in positive radians)
      while( -M_PI > a1 ) a1 += 2 * M_PI;

      // it is possible that a2 > PI, if so then we need to loop around to -PI
      // translate a2 into the 3rd quadrant (in negative radians)
      while( M_PI < a2 ) a2 -= 2 * M_PI;

      for( auto it = building_list.begin(); it != building_list.end(); ++it )
      {
        building *building = (*it);

        // ignore any building not in the current quadrant (when not using quadrants this will always be true)
        if( this->in_current_quadrant( building ) )
        {
          // get the building's angle based on the current angle's centroid
          coordinate p = (*it)->get_position();
          p.set_centroid( c );
          double a = (*it)->get_position().get_a();

          if( a1 > a2 )
          {
            // the arc crosses over the -pi/pi boundary
            if( ( a1 <= a && a <= M_PI ) || ( -M_PI <= a && a < a2 ) )
              this->initial_building_list.push_back( *it );
          }
          else
          {
            if( a1 <= a && a < a2 ) this->initial_building_list.push_back( *it );
          }
        }
      }

      if( 0 == this->initial_building_list.size() && utilities::verbose ) 
        utilities::output( "no buildings found in arc" );

      iteration++;

      // increase the angle in case we need another iteration
      if( 0 == this->initial_building_list.size() && iteration < 1000 ) this->start_angle += this->arc_angle;
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void arc_epi::from_json( const Json::Value &json )
  {
    direction_epi::from_json( json );
    this->arc_angle = json["arc_angle"].asDouble();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void arc_epi::to_json( Json::Value &json ) const
  {
    direction_epi::to_json( json );
    json["arc_angle"] = this->arc_angle;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string arc_epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << direction_epi::get_csv_header();
    stream << "# arc_angle: " << this->arc_angle << std::endl;
    return stream.str();
  }
}
}
