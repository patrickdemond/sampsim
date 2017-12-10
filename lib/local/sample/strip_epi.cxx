/*=========================================================================

  Program:  sampsim
  Module:   strip_epi.cxx
  Language: C++

=========================================================================*/

#include "strip_epi.h"

#include "building.h"
#include "building_tree.h"
#include "town.h"

#include <json/value.h>
#include <json/writer.h>
#include <stdexcept>
#include <vector>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void strip_epi::copy( const strip_epi* object )
  {
    this->strip_width = object->strip_width;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void strip_epi::determine_initial_building_list( sampsim::building_tree &tree )
  {
    this->initial_building_list.clear();
    building_list_type building_list = tree.get_building_list();
    coordinate centroid = ( *building_list.cbegin() )->get_town()->get_centroid();

    // 2. get list of all buildings in rectangle formed by drawing a line from the centre of the town
    //    to the edge following the start angle, then widening that line into a rectangle the width
    //    of the input parameter "strip width" such that the original line runs along the centre of
    //    the rectagle
    int iteration = 0;
    do
    {
      if( utilities::verbose )
        utilities::output( "iteration #%d (angle %0.2f)", iteration + 1, this->start_angle );

      // determine the line coefficient (for lines making strip of the appropriate width)
      double sin_min_angle = sin( -this->start_angle );
      double cos_min_angle = cos( -this->start_angle );
      double tan_angle = tan( this->start_angle );
      double coef = safe_subtract( centroid.y, centroid.x * tan_angle );
      double offset = this->strip_width / 2;
      double coef1 = safe_subtract( coef, offset );
      double coef2 = coef + offset;

      for( auto it = building_list.begin(); it != building_list.end(); ++it )
      {
        coordinate p = (*it)->get_position();

        // determine the house's coefficient based on a line at the same angle as the strip lines
        // but crossing through the point
        double house_coef = safe_subtract( p.y, p.x * tan_angle );

        if( coef1 <= house_coef && house_coef < coef2 ) 
        {
          // now rotate the point by -angle to see if is in the strip or on the opposite side
          double rotated_x = safe_subtract( p.x, centroid.x ) * cos_min_angle -
                             safe_subtract( p.y, centroid.y ) * sin_min_angle +
                             centroid.x;
          if( centroid.x <= rotated_x ) this->initial_building_list.push_back( *it );
        }
      }

      if( 0 == this->initial_building_list.size() && utilities::verbose )
        utilities::output( "no buildings found in strip" );

      iteration++;

      // increase the angle in case we need another iteration
      this->start_angle += 2*M_PI/1000;
    }
    while( 0 == this->initial_building_list.size() && iteration < 1000 );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void strip_epi::from_json( const Json::Value &json )
  {
    direction_epi::from_json( json );
    this->strip_width = json["strip_width"].asDouble();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void strip_epi::to_json( Json::Value &json ) const
  {
    direction_epi::to_json( json );
    json["strip_width"] = this->strip_width;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string strip_epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << direction_epi::get_csv_header();
    stream << "# strip_width: " << this->strip_width << std::endl;
    return stream.str();
  }
}
}
