/*=========================================================================

  Program:  sampsim
  Module:   coordinate.cxx
  Language: C++

=========================================================================*/

#include "coordinate.h"

#include "utilities.h"

#include <cmath>
#include <json/value.h>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool coordinate::operator == ( const coordinate c ) const
  {
    return safe_equals( this->x, c.x ) &&
           safe_equals( this->y, c.y ) &&
           safe_equals( this->cx, c.cx ) &&
           safe_equals( this->cy, c.cy );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  coordinate coordinate::operator - ( const coordinate c ) const
  {
    coordinate r( safe_subtract( this->x, c.x ), safe_subtract( this->y, c.y ) );
    return r;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void coordinate::copy( const coordinate* object )
  {
    this->x = object->x;
    this->y = object->y;
    this->cx = object->cx;
    this->cy = object->cy;
  }

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
    return sqrt( this->squared_distance( c ) );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double coordinate::squared_distance( const coordinate c ) const
  {
    double dx = safe_subtract( this->x, c.x );
    double dy = safe_subtract( this->y, c.y );
    return dx*dx + dy*dy;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double coordinate::get_a() const
  {
    return atan2( safe_subtract( this->y, this->cy ), safe_subtract( this->x, this->cx ) );
  }
}
