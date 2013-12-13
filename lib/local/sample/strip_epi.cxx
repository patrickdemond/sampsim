/*=========================================================================

  Program:  sampsim
  Module:   strip_epi.cxx
  Language: C++

=========================================================================*/

#include "strip_epi.h"

#include "building.h"
#include "population.h"

#include <json/value.h>
#include <stdexcept>
#include <vector>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::list< building* >::iterator strip_epi::select_next_building( std::list< building* > &list )
  {
    // make sure the strip width has been set
    if( 0 >= this->strip_width )
      throw std::runtime_error( "Tried to sample without first setting the strip width parameter" );

    std::list< building* >::iterator building_it;

    if( NULL == this->current_building )
    {   
      // 1. get list of all buildings in rectangle formed by drawing a line from the centre
      //    of the population to the edge following the random angle, then widening that
      //    line into a rectangle the width of the input parameter "strip width" such that
      //    the original line runs along the centre of the rectagle
      std::vector< std::list< building* >::iterator > initial_buildings;

      // 2. keep repeating step 2 until the list produced is not empty
      int iterations = 0;
      while( 0 == initial_buildings.size() && iterations < 100 )
      {
        // if a start angle hasn't been defined then pick a random start angle in [-PI, PI]
        if( !this->start_angle_defined )
          this->start_angle = utilities::random() * 2 * M_PI - M_PI;
        if( utilities::verbose )
          utilities::output( "selecting starting angle of %0.3f radians", this->start_angle );

        // determine the line coefficient (for lines making strip of the appropriate width)
        double sin_min_angle = sin( -this->start_angle );
        double cos_min_angle = cos( -this->start_angle );
        double tan_angle = tan( this->start_angle );
        coordinate c = this->population->get_centroid();
        double coef = c.y - c.x * tan_angle;
        double offset = this->strip_width / 2;
        double coef1 = coef - offset;
        double coef2 = coef + offset;

        for( auto it = list.begin(); it != list.end(); ++it )
        {
          coordinate p = (*it)->get_position();

          // determine the house's coefficient based on a line at the same angle as the strip lines
          // but crossing through the point
          double house_coef = p.y - p.x * tan_angle;

          if( coef1 <= house_coef && house_coef < coef2 ) 
          {
            // now rotate the point by -angle to see if is in the strip or on the opposite side
            double rotated_x = ( p.x - c.x ) * cos_min_angle - ( p.y - c.y ) * sin_min_angle + c.x;
            if( c.x <= rotated_x ) initial_buildings.push_back( it );
          }
        }

        if( 0 == initial_buildings.size() && utilities::verbose )
          utilities::output( "no buildings found in strip" );

        iterations++;
      }

      if( 0 == initial_buildings.size() )
        throw std::runtime_error(
          "Unable to find initial building after 100 attempts (try widening the strip width)" );

      // 3. select a random building from the list produced by step 2
      this->first_building_index = utilities::random( 0, initial_buildings.size() - 1 );
      auto initial_it = initial_buildings.begin();
      std::advance( initial_it, this->first_building_index );
      building_it = *initial_it;
      if( utilities::verbose )
        utilities::output(
          "selecting building %d of %d in strip",
          this->first_building_index + 1,
          initial_buildings.size() );
    }
    else
    {
      building_it = epi::select_next_building( list );
    }

    this->current_building = *building_it;
    return building_it;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void strip_epi::to_json( Json::Value &json ) const
  {
    epi::to_json( json );
    json["strip_width"] = this->strip_width;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string strip_epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << epi::get_csv_header();
    stream << "# strip_width: " << this->strip_width << std::endl;
    return stream.str();
  }
}
}
