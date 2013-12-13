/*=========================================================================

  Program:  sampsim
  Module:   epi.cxx
  Language: C++

=========================================================================*/

#include "epi.h"

#include "building.h"

#include <json/value.h>
#include <limits>
#include <stdexcept>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  epi::epi()
  {
    this->start_angle_defined = false;
    this->start_angle = 0;
    this->first_building_index = 0;
    this->current_building = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::generate()
  {
    // reset the current building before running the parent method
    this->current_building = NULL;
    sample::generate();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::list< building* >::iterator epi::select_next_building( std::list< building* > &list )
  {
    std::list< building* >::iterator building_it;

    // Steps 1, 2 and 3 are done by child classes in select_initial_building()
    if( NULL == this->current_building )
    {
      throw std::runtime_error(
        "Classes extending epi must select an initial building before passing "
        "reponsibility to the parent method" );
    }
    else
    {
      // 5. find the nearest building (distance measured in a straight path (as the crow flies))
      if( utilities::verbose ) utilities::output( "finding the nearest unselected neighbouring building" );
      double min = std::numeric_limits<double>::max();
      for( auto it = list.begin(); it != list.end(); ++it )
      {
        double distance =
          current_building->get_position().distance( (*it)->get_position() );
        if( distance < min )
        {
          building_it = it;
          min = distance;
        }
      }
    }

    this->current_building = *building_it;
    return building_it;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::to_json( Json::Value &json ) const
  {
    sample::to_json( json );
    json["start_angle"] = this->start_angle;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << sample::get_csv_header();
    stream << "# start_angle: " << this->start_angle << std::endl;
    stream << "# first_building_index: " << this->first_building_index << std::endl;
    return stream.str();
  }
}
}
