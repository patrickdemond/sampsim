/*=========================================================================

  Program:  sampsim
  Module:   epi.cxx
  Language: C++

=========================================================================*/

#include "epi.h"

#include "building.h"
#include "building_tree.h"

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
    this->skip = 1;
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
  building* epi::select_next_building( sampsim::building_tree &tree )
  {
    // Steps 1, 2 and 3 are done by child classes in select_initial_building()
    if( NULL == this->current_building )
    {
      throw std::runtime_error(
        "Classes extending epi must select an initial building before passing "
        "reponsibility to the parent method" );
    }

    // 5. find the nearest building (distance measured in a straight path (as the crow flies))

    // we do this step "skip" times, then select the last in the set
    building* b = NULL;
    coordinate position = this->current_building->get_position();
    for( int i = 0; i < this->skip; i++ )
    {
      // find the nearest building, make note of its position and remove it if it isn't the
      // last building in the loop
      b = tree.find_nearest( position );
      position = b->get_position();
      if( i < ( this->skip - 1 ) ) tree.remove( b );
    }
      
    return b;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::set_skip( const unsigned int skip )
  {
    if( utilities::verbose ) utilities::output( "setting skip to %d", skip );
    this->skip = skip;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void epi::to_json( Json::Value &json ) const
  {
    sample::to_json( json );
    json["skip"] = this->skip;
    json["start_angle"] = this->start_angle;
    json["first_building_index"] = this->first_building_index;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string epi::get_csv_header() const
  {
    std::stringstream stream;
    stream << sample::get_csv_header();
    stream << "# skip: " << this->skip << std::endl;
    stream << "# start_angle: " << this->start_angle << std::endl;
    stream << "# first_building_index: " << this->first_building_index << std::endl;
    return stream.str();
  }
}
}
