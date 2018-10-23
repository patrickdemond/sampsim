/*=========================================================================

  Program:  sampsim
  Module:   enumeration.cxx
  Language: C++

=========================================================================*/

#include "enumeration.h"

#include "building.h"

#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  enumeration_pair_type enumeration::split()
  {
    // determine the dimensions of the new enumerations
    extent_type e1 = extent_type(
      this->extent.first,
      coordinate(
        this->horizontal ? ( this->extent.first.x + this->extent.second.x ) / 2 : this->extent.second.x,
        this->horizontal ? this->extent.second.y : ( this->extent.first.y + this->extent.second.y ) / 2
      )
    );

    extent_type e2 = extent_type(
      coordinate(
        this->horizontal ? ( this->extent.first.x + this->extent.second.x ) / 2 : this->extent.first.x,
        this->horizontal ? this->extent.first.y : ( this->extent.first.y + this->extent.second.y ) / 2
      ),
      this->extent.second
    );

    // now figure out which of the two new emuerations each building belongs to
    building_list_type list1, list2;
    for( auto it = this->building_list.cbegin(); it != this->building_list.cend(); ++it )
    {
      sampsim::building *b = (*it);
      coordinate c = b->get_position();
      if( e1.first.x <= c.x && c.x < e1.second.x && e1.first.y <= c.y && c.y < e1.second.y )
      {
        list1.push_back( b );
      }
      else if( e2.first.x <= c.x && c.x < e2.second.x && e2.first.y <= c.y && c.y < e2.second.y )
      {
        list2.push_back( b );
      }
      else
      {
        std::stringstream stream;
        stream << "ERROR: found out-of-bounds building " << c
               << " while trying to split enumeration with extent "
               << this->extent.first << " to " << this->extent.second;
        throw std::runtime_error( stream.str() );
      }
    }

    return enumeration_pair_type(
      new enumeration( !this->horizontal, e1, list1 ),
      new enumeration( !this->horizontal, e2, list2 )
    );
  }
}
