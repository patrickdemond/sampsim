/*=========================================================================

  Program:  sampsim
  Module:   parameter.h
  Language: C++

=========================================================================*/

/**
 * @class parameter
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A class used to manage input parameters
 */

#ifndef __sampsim_parameter_h
#define __sampsim_parameter_h

#include <string>

namespace sampsim
{
  class parameter
  {
  public:
    parameter() :
      flag( false ), short_name( '\0' ), long_name( "" ), initial( "" ), description( "" ), value( "" ) {};
    parameter( const bool f, const char sn, const std::string ln, const bool i, const std::string d ) :
      flag( f ), short_name( sn ), long_name( ln ), initial( i ? "true" : "false" ),
      description( d ), value( "" ) {};
    parameter( const bool f, const char sn, const std::string ln, const std::string i, const std::string d ) :
      flag( f ), short_name( sn ), long_name( ln ), initial( i ),
      description( d ), value( "" ) {};

    bool is_flag() const;
    bool is_on() const;
    bool has_short_name() const;
    char get_short_name() const;
    bool has_long_name() const;
    std::string get_long_name() const;
    std::string get_value() const;
    int get_value_as_int() const;
    double get_value_as_double() const;
    std::string get_usage() const;

    bool set_value( const bool v );
    bool set_value( const std::string v );

  private:
    bool flag;
    char short_name;
    std::string long_name;
    std::string initial;
    std::string description;
    std::string value;
  };
}

/** @} end of doxygen group */

#endif
