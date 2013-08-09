/*=========================================================================

  Program:  sampsim
  Module:   options.h
  Language: C++

=========================================================================*/

/**
 * @class options
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A class used to manage input optionss
 */

#ifndef __options_h
#define __options_h

#include <string>
#include <vector>

namespace sampsim
{
  class options
  {
  private:
    struct flag
    {
      flag() : short_name( ' ' ), long_name( "" ), value( false ), description( "" ) {};
      char short_name;
      std::string long_name;
      bool value;
      std::string description;
      bool has_short_name() const { return ' ' != this->short_name; }
      bool has_long_name() const { return 0 < this->long_name.length(); }
    };

    struct option
    {
      option() : short_name( ' ' ), long_name( "" ), initial( "" ), value( "" ), description( "" ) {};
      char short_name;
      std::string long_name;
      std::string initial;
      std::string value;
      std::string description;
      bool has_short_name() const { return ' ' != this->short_name; }
      bool has_long_name() const { return 0 < this->long_name.length(); }
      std::string get_value() const { return 0 == this->value.length() ? this->initial : this->value; }
    };

  public:
    options( const std::string executable_name );

    void set_min_remaining_arguments( int min ) { this->min_remaining_arguments = min; }
    void set_max_remaining_arguments( int max ) { this->max_remaining_arguments = max; }

    void add_heading( const std::string description );

    void add_flag(
      const char short_name,
      const std::string long_name,
      const std::string description );
    void add_flag( const char short_name, const std::string description )
    { this->add_flag( short_name, "", description ); }
    void add_flag( const std::string long_name, const std::string description )
    { this->add_flag( ' ', long_name, description ); }

    void add_option(
      const char short_name,
      const std::string long_name,
      const std::string initial,
      const std::string description );
    void add_option( const char short_name, const std::string initial, const std::string description )
    { this->add_option( short_name, "", initial, description ); }
    void add_option( const std::string long_name, const std::string initial, const std::string description )
    { this->add_option( ' ', long_name, initial, description ); }

    std::vector< std::string > get_arguments() const;
    void set_arguments( const int argc, char** argv );

    bool process();
    void print_usage();

    bool get_flag( const std::string long_name ) const
    { return this->get_flag( ' ', long_name ); }
    bool get_flag( const char short_name ) const
    { return this->get_flag( short_name, "" ); }
    std::string get_option( const std::string long_name ) const
    { return this->get_option( ' ', long_name ); }
    std::string get_option( const char short_name ) const
    { return this->get_option( short_name, "" ); }
    int get_option_as_int( const std::string long_name ) const
    { return atoi( this->get_option( long_name ).c_str() ); }
    double get_option_as_double( const std::string long_name ) const
    { return atof( this->get_option( long_name ).c_str() ); }
    int get_option_as_int( const char short_name ) const
    { return atoi( this->get_option( short_name ).c_str() ); }
    double get_option_as_double( const char short_name ) const
    { return atof( this->get_option( short_name ).c_str() ); }

  private:
    std::string get_usage() const;

    bool get_flag( const char short_name, const std::string long_name ) const;
    std::string get_option( const char short_name, const std::string long_name ) const;
    flag* find_flag( const char short_name );
    flag* find_flag( const std::string long_name );
    option* find_option( const char short_name );
    option* find_option( const std::string long_name );

    bool process_arguments();
    bool process_config_file( const std::string filename );

    bool usage_printed;
    int min_remaining_arguments;
    int max_remaining_arguments;

    std::vector< std::string > remaining_argument_list;
    std::vector< std::string > argument_list;
    std::vector< std::string > config_file_list;
    std::vector< flag > flag_list;
    std::vector< option > option_list;
    std::vector< std::string > usage_list;
  };
}

/** @} end of doxygen group */

#endif
