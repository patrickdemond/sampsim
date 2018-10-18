/*=========================================================================

  Program:  sampsim
  Module:   building_enumeration.cxx
  Language: C++

=========================================================================*/

#include "building_enumeration.h"

#include "building.h"
#include "town.h"

#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::pair< building_enumeration::enumeration, building_enumeration::enumeration > building_enumeration::enumeration::split()
  {
    // build the two new enumerations by splitting the current one in the opposite direction
    enumeration e1, e2;
    
    e1.horizontal = !this->horizontal;
    e1.lower_left_coordinate = this->lower_left_coordinate;
    e1.upper_right_coordinate = coordinate(
      this->upper_right_coordinate.x / ( this->horizontal ? 2 : 1 ),
      this->upper_right_coordinate.y / ( this->horizontal ? 1 : 2 )
    );

    e2.horizontal = !this->horizontal;
    e2.lower_left_coordinate = coordinate(
      this->lower_left_coordinate.x / ( this->horizontal ? 2 : 1 ),
      this->lower_left_coordinate.y / ( this->horizontal ? 1 : 2 )
    );
    e2.upper_right_coordinate = this->upper_right_coordinate;

    // now figure out which of the two new emuerations each building belongs to
    for( auto it = this->building_list.cbegin(); it != this->building_list.cend(); ++it )
    {
      sampsim::building *b = (*it);
      coordinate c = b->get_position();
      if( e1.lower_left_coordinate.x <= c.x && c.x < e1.upper_right_coordinate.x &&
          e1.lower_left_coordinate.y <= c.y && c.y < e1.upper_right_coordinate.y )
      {
        e1.building_list.push_back( b );
      }
      else if( e2.lower_left_coordinate.x <= c.x && c.x < e2.upper_right_coordinate.x &&
               e2.lower_left_coordinate.y <= c.y && c.y < e2.upper_right_coordinate.y )
      {
        e2.building_list.push_back( b );
      }
      else
      {
        throw std::runtime_error( "ERROR: found out-of-bounds building while trying to split enumeration" );
      }
    }

    return std::pair< enumeration, enumeration >( e1, e2 );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_enumeration::building_enumeration( building_list_type building_list, unsigned int threshold )
  {
    this->threshold = threshold;

    sampsim::town *t = building_list.front()->get_town();
    enumeration e;
    e.lower_left_coordinate = coordinate( 0, 0 );
    e.upper_right_coordinate = coordinate( t->get_x_width(), t->get_y_width() );
    e.horizontal = true;
    e.building_list = building_list;
    this->build( e );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_enumeration::building_enumeration( const building_enumeration& building_enumeration )
  {
    this->enumeration_list = building_enumeration.enumeration_list;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building_enumeration::build( enumeration& enumeration )
  {
    // if the enumeration is below the threshold then add it to the enumeration list
    if( enumeration.building_list.size() < this->threshold )
    {
      this->enumeration_list.push_back( enumeration );
    }
    else
    {
      // split the enumeration in two and build each one
      auto enums = enumeration.split();
      this->build( enums.first );
      this->build( enums.second );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string building_enumeration::to_string( enumeration* )
  {
    return "";
  }
}
