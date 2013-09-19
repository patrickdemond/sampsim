/*=========================================================================

  Program:  sampsim
  Module:   strip_epi.cxx
  Language: C++

=========================================================================*/

#include "strip_epi.h"

#include "building.h"
#include "household.h"
#include "population.h"

#include <json/value.h>
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
        if( utilities::verbose )
          utilities::output( "selecting starting angle of %0.3f radians", this->angle );

        // determine the line coefficient (for lines making strip of the appropriate width)
        double sin_min_angle = sin( -angle );
        double cos_min_angle = cos( -angle );
        double tan_angle = tan( angle );
        coordinate c = this->population->get_centroid();
        double coef = c.y - c.x * tan_angle;
        double offset = this->strip_width / 2;
        double coef1 = coef - offset;
        double coef2 = coef + offset;

        for( auto it = list.begin(); it != list.end(); ++it )
        {
          coordinate p = (*it)->get_building()->get_position();

          // determine the house's coefficient based on a line at the same angle as the strip lines
          // but crossing through the point
          double house_coef = p.y - p.x * tan_angle;

          if( coef1 <= house_coef && house_coef < coef2 ) 
          {
            // now rotate the point by -angle to see if is in the strip or on the opposite side
            double rotated_x = ( p.x - c.x ) * cos_min_angle - ( p.y - c.y ) * sin_min_angle + c.x;
            if( c.x <= rotated_x ) initial_households.push_back( it );
          }
        }

        if( 0 == initial_households.size() && utilities::verbose )
          utilities::output( "no households found in strip" );
      }

      // 3. select a random building from the list produced by step 2
      this->first_house_index = utilities::random( 0, initial_households.size() - 1 );
      auto initial_it = initial_households.begin();
      std::advance( initial_it, this->first_house_index );
      household_it = *initial_it;
      if( utilities::verbose )
        utilities::output(
          "selecting household %d of %d in strip",
          this->first_house_index + 1,
          initial_households.size() );
    }
    else
    {
      household_it = epi::select_next_household( list );
    }

    this->current_household = *household_it;
    return household_it;
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
