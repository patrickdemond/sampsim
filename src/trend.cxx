/*=========================================================================

  Program:  sampsim
  Module:   trend.cxx
  Language: C++

=========================================================================*/

#include "trend.h"

#include <json/value.h>
#include <sstream>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  trend::trend(
    const double b00,
    const double b01,
    const double b10,
    const double b02,
    const double b20,
    const double b11 )
  {
    this->b00 = b00;
    this->b01 = b01;
    this->b10 = b10;
    this->b02 = b02;
    this->b20 = b20;
    this->b11 = b11;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void trend::copy( const trend* t )
  {
    this->b00 = t->b00;
    this->b01 = t->b01;
    this->b10 = t->b10;
    this->b02 = t->b02;
    this->b20 = t->b20;
    this->b11 = t->b11;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double trend::get_value( const coordinate c )
  {
    return this->b00 +
           this->b01 * c.x +
           this->b10 * c.y +
           this->b02 * c.x * c.x +
           this->b20 * c.y * c.y +
           this->b11 * c.x * c.y;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string trend::to_string() const
  {
    bool first = true;
    std::stringstream stream;

    if( 0 != this->b00 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << this->b00;
    }

    if( 0 != this->b01 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << this->b01 << "x";
    }

    if( 0 != this->b10 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << this->b10 << "y";
    }

    if( 0 != this->b02 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << this->b02 << "x^2";
    }

    if( 0 != this->b20 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << this->b20 << "y^2";
    }

    if( 0 != this->b11 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << this->b11 << "xy";
    }

    return 0 == stream.str().length() ? "0" : stream.str();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void trend::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["b00"] = this->b00;
    json["b01"] = this->b01;
    json["b10"] = this->b10;
    json["b02"] = this->b02;
    json["b20"] = this->b20;
    json["b11"] = this->b11;
  }
}
