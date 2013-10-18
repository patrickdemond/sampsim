/*=========================================================================

  Program:  sampsim
  Module:   options.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_options_h
#define __sampsim_options_h

#include <map>
#include <string>
#include <vector>

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  /**
   * @class options
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A class used to manage input optionss
   * @details
   * 
   */
  class options
  {
  private:
    /**
     * 
     */
    struct flag
    {
      /**
       * 
       */
      flag() : short_name( ' ' ), long_name( "" ), value( false ), description( "" ) {};

      /**
       * 
       */
      char short_name;

      /**
       * 
       */
      std::string long_name;

      /**
       * 
       */
      bool value;

      /**
       * 
       */
      std::string description;

      /**
       * 
       */
      bool has_short_name() const { return ' ' != this->short_name; }

      /**
       * 
       */
      bool has_long_name() const { return 0 < this->long_name.length(); }
    };

    /**
     * 
     */
    struct option
    {
      /**
       * 
       */
      option() : short_name( ' ' ), long_name( "" ), initial( "" ), value( "" ), description( "" ) {};

      /**
       * 
       */
      char short_name;

      /**
       * 
       */
      std::string long_name;

      /**
       * 
       */
      std::string initial;

      /**
       * 
       */
      std::string value;

      /**
       * 
       */
      std::string description;

      /**
       * 
       */
      bool has_short_name() const { return ' ' != this->short_name; }

      /**
       * 
       */
      bool has_long_name() const { return 0 < this->long_name.length(); }

      /**
       * 
       */
      std::string get_value() const { return 0 == this->value.length() ? this->initial : this->value; }
    };

  public:
    /**
     * 
     */
    options( const std::string executable_name );

    /**
     * 
     */
    void add_heading( const std::string description );

    /**
     * 
     */
    void add_input( const std::string name );

    /**
     * 
     */
    void add_flag(
      const char short_name,
      const std::string long_name,
      const std::string description );

    /**
     * 
     */
    void add_flag( const char short_name, const std::string description )
    { this->add_flag( short_name, "", description ); }

    /**
     * 
     */
    void add_flag( const std::string long_name, const std::string description )
    { this->add_flag( ' ', long_name, description ); }

    /**
     * 
     */
    void add_option(
      const char short_name,
      const std::string long_name,
      const std::string initial,
      const std::string description );

    /**
     * 
     */
    void add_option( const char short_name, const std::string initial, const std::string description )
    { this->add_option( short_name, "", initial, description ); }

    /**
     * 
     */
    void add_option( const std::string long_name, const std::string initial, const std::string description )
    { this->add_option( ' ', long_name, initial, description ); }

    /**
     * 
     */
    void set_arguments( const int argc, const char** argv );

    /**
     * 
     */
    bool process();

    /**
     * 
     */
    void print_usage();

    /**
     * 
     */
    std::string get_input( const std::string name ) const;

    /**
     * 
     */
    bool get_flag( const std::string long_name ) const
    { return this->get_flag( ' ', long_name ); }

    /**
     * 
     */
    bool get_flag( const char short_name ) const
    { return this->get_flag( short_name, "" ); }

    /**
     * 
     */
    std::string get_option( const std::string long_name ) const
    { return this->get_option( ' ', long_name ); }

    /**
     * 
     */
    std::string get_option( const char short_name ) const
    { return this->get_option( short_name, "" ); }

    /**
     * 
     */
    int get_option_as_int( const std::string long_name ) const
    { return atoi( this->get_option( long_name ).c_str() ); }

    /**
     * 
     */
    double get_option_as_double( const std::string long_name ) const
    { return atof( this->get_option( long_name ).c_str() ); }

    /**
     * 
     */
    int get_option_as_int( const char short_name ) const
    { return atoi( this->get_option( short_name ).c_str() ); }

    /**
     * 
     */
    double get_option_as_double( const char short_name ) const
    { return atof( this->get_option( short_name ).c_str() ); }

  private:
    /**
     * 
     */
    std::string get_usage() const;

    /**
     * 
     */
    bool get_flag( const char short_name, const std::string long_name ) const;

    /**
     * 
     */
    std::string get_option( const char short_name, const std::string long_name ) const;

    /**
     * 
     */
    flag* find_flag( const char short_name );

    /**
     * 
     */
    flag* find_flag( const std::string long_name );

    /**
     * 
     */
    option* find_option( const char short_name );

    /**
     * 
     */
    option* find_option( const std::string long_name );

    /**
     * 
     */
    bool process_arguments();

    /**
     * 
     */
    bool process_config_file( const std::string filename );

    /**
     * 
     */
    std::string executable_name;

    /**
     * 
     */
    bool usage_printed;

    /**
     * 
     */
    std::map< std::string, std::string > input_map;

    /**
     * 
     */
    std::vector< std::string > argument_list;

    /**
     * 
     */
    std::vector< std::string > config_file_list;

    /**
     * 
     */
    std::vector< flag > flag_list;

    /**
     * 
     */
    std::vector< option > option_list;

    /**
     * 
     */
    std::vector< std::string > usage_list;
  };
}

/** @} end of doxygen group */

#endif
