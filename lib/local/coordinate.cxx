/*=========================================================================

  Program:  sampsim
  Module:   coordinate.cxx
  Language: C++

=========================================================================*/

#include "coordinate.h"

#include <cmath>
#include <json/value.h>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void coordinate::from_json( const Json::Value &json )
  {
    this->x = json["x"].asDouble();
    this->y = json["y"].asDouble();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void coordinate::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["x"] = this->x;
    json["y"] = this->y;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void coordinate::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    household_stream.precision( 3 );
    household_stream << std::fixed << this->x << "," << this->y << ","
                     << std::fixed << this->get_r() << "," << this->get_a();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double coordinate::distance( const coordinate c ) const
  {
    return sqrt( ( this->x - c.x )*( this->x - c.x ) +
                 ( this->y - c.y )*( this->y - c.y ) );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double coordinate::get_a() const
  {
    return atan2( this->y - this->cy, this->x - this->cx );
  }
}
