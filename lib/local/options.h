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
   * @brief A class used to parse command line options
   * @details
   * This class provides a way to parse, process and manage command line options.  It works by first
   * defining all possible options and the number of input parameters expected from a command line
   * based command.  Additionally, configuration files can also be provided which may include values
   * for flags and options.  No more than one parameter may be included per line.  Options with no
   * values (flags) are set to true if found on a line alone.  For example:
   * 
   * verbose
   * 
   * Options with values must be separated by a ":" character.  For example:
   * 
   * seed: 1234
   * 
   * If an option includes only numbers, periods and commas (no other characters or spaces)
   * then it will be inturpreted as an array.  For instance:
   * 
   * coordinate: 1.2,2.4,3.6
   * 
   * Anything proceeding a # is ignored as a comment.
   */
  class options
  {
  private:
    /**
     * @struct flag
     * @brief An internal struct for handling flags
     */
    struct flag
    {
      /**
       * Constructor
       */
      flag() : short_name( ' ' ), long_name( "" ), value( false ), description( "" ) {};

      /**
       * The flag's short (single character) name
       */
      char short_name;

      /**
       * The flag's long name
       */
      std::string long_name;

      /**
       * Whether the flag has been raised or not
       */
      bool value;

      /**
       * The flag's description
       */
      std::string description;

      /**
       * Returns whether the flag has a short (single character) name or not
       */
      bool has_short_name() const { return ' ' != this->short_name; }

      /**
       * Returns whether the flag has a long name or not
       */
      bool has_long_name() const { return 0 < this->long_name.length(); }
    };

    /**
     * @struct flag
     * @brief An internal struct for handling options
     */
    struct option
    {
      /**
       * Constructor
       */
      option() : short_name( ' ' ), long_name( "" ), initial( "" ), value( "" ), description( "" ) {};

      /**
       * The option's short (single character) name
       */
      char short_name;

      /**
       * The option's long name
       */
      std::string long_name;

      /**
       * The options default value
       */
      std::string initial;

      /**
       * The option's value (an empty string if none has been set)
       */
      std::string value;

      /**
       * The option's description
       */
      std::string description;

      /**
       * Returns whether the option has a short (single character) name or not
       */
      bool has_short_name() const { return ' ' != this->short_name; }

      /**
       * Returns whether the option has a long name or not
       */
      bool has_long_name() const { return 0 < this->long_name.length(); }

      /**
       * Returns the value of the option (returns the default value if no value has been set)
       */
      std::string get_value() const { return 0 == this->value.length() ? this->initial : this->value; }
    };

  public:
    /**
     * Constructor
     */
    options( const std::string executable_name );

    /**
     * Adds a heading to the usage message
     * 
     * This method can be used to insert a line with any text.  An end-of-line character is added to
     * the end of the description string, so it should be called once for every line which is to be
     * inserted into the usage message.  Calling this method with an empty string will add an empty
     * line in the usage message.
     */
    void add_heading( const std::string description );

    /**
     * Adds an input to the expected command line arguments
     * 
     * Inputs are mandatory values without a preceeding switch.
     * For example: ./command file1 file2 (where file1 and file2 are inputs).
     */
    void add_input( const std::string name );

    /**
     * Adds an optional flag to the command line arguments
     * 
     * Flags are optional switches which, if included, sets the flag's value to true.
     * For example: ./command -a --super (where the flags 'a' and "super" are set to true).
     */
    void add_flag(
      const char short_name,
      const std::string long_name,
      const std::string description );

    /**
     * Convenience method for a flag without a long name
     */
    void add_flag( const char short_name, const std::string description )
    { this->add_flag( short_name, "", description ); }

    /**
     * Convenience method for a flag without a short name
     */
    void add_flag( const std::string long_name, const std::string description )
    { this->add_flag( ' ', long_name, description ); }

    /**
     * Adds an optional parameter to the command line arguments
     * 
     * Options are optional parameters which, if included, sets the parameter to a particular value.
     * If the option is not provided the the initial value will be used.
     * For example: ./command --method simple (where the option "method" is set to "simple").
     */
    void add_option(
      const char short_name,
      const std::string long_name,
      const std::string initial,
      const std::string description );

    /**
     * Convenience method for an option without a long name
     */
    void add_option( const char short_name, const std::string initial, const std::string description )
    { this->add_option( short_name, "", initial, description ); }

    /**
     * Convenience method for an option without a short name
     */
    void add_option( const std::string long_name, const std::string initial, const std::string description )
    { this->add_option( ' ', long_name, initial, description ); }

    /**
     * Sets the command line arguments
     * 
     * Provide command line arguments in the standard argc and argv forms.  These are the arguments passed
     * to the application's main() function.
     */
    void set_arguments( const int argc, const char** argv );

    /**
     * Process the command line arguments
     * 
     * Processes the arguments provided to the class by set_arguments(), returning true if all arguments
     * were parsed successfully.
     */
    bool process();

    /**
     * Prints the usage message
     */
    void print_usage();

    /**
     * Returns the parsed value for a particular input
     * 
     * Inputs are defined sequentially by the add_input() method.  This method returns the input provided
     * by the command line at the same position as the named input.
     */
    std::string get_input( const std::string name ) const;

    /**
     * Returns true if a flag has been raised (included in the command line)
     */
    bool get_flag( const std::string long_name ) const
    { return this->get_flag( ' ', long_name ); }

    /**
     * Returns true if a flag has been raised (included in the command line)
     */
    bool get_flag( const char short_name ) const
    { return this->get_flag( short_name, "" ); }

    /**
     * Returns the value of an option, or the default value if none is provided
     */
    std::string get_option( const std::string long_name ) const
    { return this->get_option( ' ', long_name ); }

    /**
     * Returns the value of an option, or the default value if none is provided
     */
    std::string get_option( const char short_name ) const
    { return this->get_option( short_name, "" ); }

    /**
     * Convenience method to return an option as an integer
     */
    int get_option_as_int( const std::string long_name ) const
    { return atoi( this->get_option( long_name ).c_str() ); }

    /**
     * Convenience method to return an option as a double
     */
    double get_option_as_double( const std::string long_name ) const
    { return atof( this->get_option( long_name ).c_str() ); }

    /**
     * Convenience method to return an option as an integer
     */
    int get_option_as_int( const char short_name ) const
    { return atoi( this->get_option( short_name ).c_str() ); }

    /**
     * Convenience method to return an option as a double
     */
    double get_option_as_double( const char short_name ) const
    { return atof( this->get_option( short_name ).c_str() ); }

  private:
    /**
     * Generates the usage message and returns it as a string
     */
    std::string get_usage() const;

    /**
     * Internal method for returning the value of a flag
     */
    bool get_flag( const char short_name, const std::string long_name ) const;

    /**
     * Internal method for returning the value of an option
     */
    std::string get_option( const char short_name, const std::string long_name ) const;

    /**
     * Returns a reference to a flag struct or NULL if it is not found
     */
    flag* find_flag( const char short_name );

    /**
     * Returns a reference to a flag struct or NULL if it is not found
     */
    flag* find_flag( const std::string long_name );

    /**
     * Returns a reference to an option struct or NULL if it is not found
     */
    option* find_option( const char short_name );

    /**
     * Returns a reference to an option struct or NULL if it is not found
     */
    option* find_option( const std::string long_name );

    /**
     * Internal method for processing arguments passed by the command line
     */
    bool process_arguments();

    /**
     * Internal method for processing arguments passed in configuration files
     */
    bool process_config_file( const std::string filename );

    /**
     * The name of the executable for which options are being parsed (set by the constructor)
     */
    std::string executable_name;

    /**
     * Whether or not the usage message has already been displayed
     */
    bool usage_printed;

    /**
     * A list of all inputs indexed by name
     */
    std::map< std::string, std::string > input_map;

    /**
     * A list of arguments indexed by sequence
     */
    std::vector< std::string > argument_list;

    /**
     * A list of all configuration files
     */
    std::vector< std::string > config_file_list;

    /**
     * A list of all flags indexed by sequence
     */
    std::vector< flag > flag_list;

    /**
     * A list of all options indexed by sequence
     */
    std::vector< option > option_list;

    /**
     * A list of all lines in the usage message indexed by sequence
     */
    std::vector< std::string > usage_list;
  };
}

/** @} end of doxygen group */

#endif
