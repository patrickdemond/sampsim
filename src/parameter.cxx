/*=========================================================================

  Program:  sampsim
  Module:   parameter.h
  Language: C++

=========================================================================*/

#include "parameter.h"

#include <sstream>

namespace sampsim
{
  bool parameter::is_flag() const
  { return this->flag; }
  
  bool parameter::is_on() const
  { return "true" == this->value; }
  
  bool parameter::has_short_name() const
  { return ' ' != this->short_name; }

  char parameter::get_short_name() const
  { return this->short_name; }

  bool parameter::has_long_name() const
  { return this->long_name.length(); }
  
  std::string parameter::get_long_name() const
  { return this->long_name; }

  std::string parameter::get_value() const
  { return 0 == this->value.length() ? this->initial : this->value; }

  int parameter::get_value_as_int() const
  { return atoi( this->get_value().c_str() ); }

  double parameter::get_value_as_double() const
  { return atof( this->get_value().c_str() ); }

  bool parameter::set_value( const bool v ) { this->value = v ? "true" : "false"; return true; }
  bool parameter::set_value( const std::string v ) { this->value = v; return true; }

  std::string parameter::get_usage() const
  {
    std::stringstream stream;

    // switches
    stream << " ";
    if( this->has_short_name() ) stream << "-" << this->short_name << "  ";
    if( this->has_long_name() ) stream << "--" << this->long_name;

    // spacing after switches, then description
    std::string spacing;
    spacing.assign( 25 - stream.str().length(), ' ' );
    stream << spacing << this->description;
    return stream.str();
  }
}
