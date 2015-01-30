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
  building* strip_epi::select_next_building( sampsim::building_tree &tree )
  {
    // make sure the strip width has been set
    if( 0 >= this->strip_width )
      throw std::runtime_error( "Tried to sample without first setting the strip width parameter" );

    building* b;

    // check to see if we have to move to the next sector and reset the current building if we do
    double size_fraction = static_cast< double >( this->get_size() ) /
                           static_cast< double >( this->get_number_of_sectors() );
    if( this->get_current_size() > size_fraction * ( this->get_current_sector() ) )
      this->current_building = NULL;

    if( NULL == this->current_building )
    {   
      building_list_type building_list = tree.get_building_list();
      coordinate centroid = ( *building_list.cbegin() )->get_town()->get_centroid();

      // 1. get list of all buildings in rectangle formed by drawing a line from the centre
      //    of the town to the edge following the random angle, then widening that line into
      //    a rectangle the width of the input parameter "strip width" such that the original
      //    line runs along the centre of the rectagle
      building_list_type initial_buildings;

      // 2. if the start angle is not explicitly set then keep repeating step 2 until the list
      //    produced is not empty
      int iteration = 0;
      do
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

        // determine the line coefficient (for lines making strip of the appropriate width)
        double sin_min_angle = sin( -this->start_angle );
        double cos_min_angle = cos( -this->start_angle );
        double tan_angle = tan( this->start_angle );
        double coef = centroid.y - centroid.x * tan_angle;
        double offset = this->strip_width / 2;
        double coef1 = coef - offset;
        double coef2 = coef + offset;

        for( auto it = building_list.begin(); it != building_list.end(); ++it )
        {
          coordinate p = (*it)->get_position();

          // determine the house's coefficient based on a line at the same angle as the strip lines
          // but crossing through the point
          double house_coef = p.y - p.x * tan_angle;

          if( coef1 <= house_coef && house_coef < coef2 ) 
          {
            // now rotate the point by -angle to see if is in the strip or on the opposite side
            double rotated_x = ( p.x - centroid.x ) * cos_min_angle -
                               ( p.y - centroid.y ) * sin_min_angle +
                               centroid.x;
            if( centroid.x <= rotated_x ) initial_buildings.push_back( *it );
          }
        }

        if( 0 == initial_buildings.size() && utilities::verbose )
          utilities::output( "no buildings found in strip" );

        iteration++;
      }
      while( !this->start_angle_defined && 0 == initial_buildings.size() && iteration < 100 );

      if( 0 == initial_buildings.size() )
        throw std::runtime_error(
          "Unable to find initial building after 100 attempts (try widening the strip width)" );

      // 3. select a random building from the list produced by step 2
      this->first_building_index = utilities::random( 0, initial_buildings.size() - 1 );
      auto initial_it = initial_buildings.begin();
      std::advance( initial_it, this->first_building_index );
      b = *initial_it;

      if( utilities::verbose )
        utilities::output(
          "selecting building %d of %d in strip",
          this->first_building_index + 1,
          initial_buildings.size() );
    }
    else
    {
      b = epi::select_next_building( tree );
    }

    this->current_building = b;
    return b;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void strip_epi::from_json( const Json::Value &json )
  {
    sample::from_json( json );
    this->strip_width = json["strip_width"].asDouble();
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
