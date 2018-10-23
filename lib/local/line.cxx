/*=========================================================================

  Program:  sampsim
  Module:   line.cxx
  Language: C++

=========================================================================*/

#include "line.h"

#include <iostream>
#include <json/value.h>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void line::copy( const line* object )
  {
    this->intercept.copy( &( object->intercept ) );
    this->angle = object->angle;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void line::from_json( const Json::Value &json )
  {
    this->intercept.x = json["x"].asDouble();
    this->intercept.y = json["y"].asDouble();
    this->angle = json["angle"].asDouble();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void line::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["x"] = this->intercept.x;
    json["y"] = this->intercept.y;
    json["angle"] = this->angle;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double line::distance_from_point( coordinate point ) const
  {
    double tan_angle = tan( this->angle );
    return std::abs( safe_subtract( point.x, this->intercept.x ) *
                     safe_subtract( tan_angle, safe_subtract( point.y, this->intercept.y ) ) ) /
             sqrt( tan_angle*tan_angle + 1.0 );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool line::line_inside_bounds( extent_type extent )
  {
    double tan_angle = tan( this->angle );
    double b = safe_subtract( this->intercept.y, this->intercept.x * tan_angle );
    double y1 = tan_angle * extent.first.x + b;
    double y2 = tan_angle * extent.second.x + b;
    return ( y1 > extent.first.y || y2 > extent.first.y ) &&
           ( y1 < extent.second.y || y2 < extent.second.y );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool line::point_inside_strip( coordinate c, line l1, line l2 )
  {
    // the point is inside the strip if its distance from both lines is less than the distance between
    // the two lines
    double line_distance = l1.distance_from_point( l2.intercept );
    return l1.distance_from_point( c ) < line_distance &&
           l2.distance_from_point( c ) < line_distance;
  }
}
