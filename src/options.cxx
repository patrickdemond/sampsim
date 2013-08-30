/*=========================================================================

  Program:  sampsim
  Module:   options.h
  Language: C++

=========================================================================*/

#include "options.h"

#include "utilities.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  options::options( const std::string executable_name )
  {
    this->usage_printed = false;
    this->executable_name = executable_name;
    this->add_flag( 'h', "help", "Prints usage help" );
    this->add_option( 'c', "config", "", "A configuration file containing flags and options" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void options::add_heading( const std::string description )
  {
    this->usage_list.push_back( description );
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void options::add_input( const std::string name )
  {
    this->input_map[name] = "";
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void options::add_flag(
    const char short_name,
    const std::string long_name,
    const std::string description )
  {
    flag flag;
    flag.short_name = short_name;
    flag.long_name = long_name;
    flag.description = description;
    this->flag_list.push_back( flag );

    // now add the usage string for this flag
    std::stringstream stream;
    stream << " ";
    if( flag.has_short_name() ) stream << "-" << flag.short_name << "  ";
    if( flag.has_long_name() ) stream << "--" << flag.long_name;
    std::string spacing;
    spacing.assign( 25 - stream.str().length(), ' ' );
    stream << spacing << flag.description;
    this->add_heading( stream.str() );
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void options::add_option(
    const char short_name,
    const std::string long_name,
    const std::string initial,
    const std::string description )
  {
    option option;
    option.short_name = short_name;
    option.long_name = long_name;
    option.initial = initial;
    option.description = description;
    this->option_list.push_back( option );

    // now add the usage string for this option
    std::stringstream stream;
    stream << " ";
    if( option.has_short_name() ) stream << "-" << option.short_name << "  ";
    if( option.has_long_name() ) stream << "--" << option.long_name;
    std::string spacing;
    spacing.assign( 25 - stream.str().length(), ' ' );
    stream << spacing << option.description;
    this->add_heading( stream.str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void options::set_arguments( const int argc, char** argv )
  {
    this->argument_list.clear();
    for( int c = 0; c < argc; c++ ) this->argument_list.push_back( argv[c] );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool options::process()
  {
    bool config_file_processed = false;
    bool success = this->process_arguments();

    if( success )
    {
      std::string config_file = this->get_option( "config" );
      while( 0 < config_file.length() )
      {
        this->find_option( "config" )->value = "";
        if( !this->process_config_file( config_file ) )
        {
          config_file_processed = false;
          break;
        }
        config_file = this->get_option( "config" );
        if( false == config_file_processed ) config_file_processed = true;
      }

      // command line arguments override config file arguments
      if( config_file_processed ) success = this->process_arguments();
    }

    return success;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool options::process_arguments()
  {
    bool invalid = false;
    char short_name = ' ';
    std::string long_name = "";
    std::string error = "";
    flag *flag;
    option *option;

    // make a temporary vector of all inputs
    std::vector< std::string > input_list;

    for( auto it = this->argument_list.cbegin(); it != this->argument_list.cend(); ++it )
    {
      // skip the first argument (it's the executable's name
      if( this->argument_list.cbegin() == it ) continue;

      std::string arg = (*it);
      if( "--" == arg.substr( 0, 2 ) )
      { // defining a long name
        if( 0 < long_name.length() ) invalid = true; // already defining an option
        else if( 3 >= arg.length() ) invalid = true; // options must be more than one character long
        else
        {
          long_name = arg.substr( 2, std::string::npos );

          // see if this is a flag
          if( NULL != ( flag = this->find_flag( long_name ) ) )
          {
            flag->value = true;
            long_name = "";
          }
        }
      }
      else if( '-' == arg[0] )
      { // defning a short name
        if( ' ' != short_name ) invalid = true; // already defining an option
        else if( 1 >= arg.length() ) invalid = true; // no option character provided
        else
        {
          short_name = arg[1];

          // see if this is a flag
          flag = this->find_flag( short_name );
          if( NULL != ( flag = this->find_flag( short_name ) ) )
          {
            flag->value = true;
            short_name = ' ';
          }
        }
      }
      else
      { // defining a value
        if( 0 < long_name.length() )
        { // expecting a value for an option identified by its long name
          if( NULL != ( option = this->find_option( long_name ) ) )
          {
            option->value = arg;
            long_name = "";
          }
          else invalid = true;
        }
        else if( ' ' != short_name )
        { // expecting a value for an option identified by its short name
          if( NULL != ( option = this->find_option( short_name ) ) )
          {
            option->value = arg;
            short_name = ' ';
          }
          else invalid = true;
        }
        else
        {
          input_list.push_back( arg );
        }
      }

      if( invalid )
      {
        std::cout << "ERROR: Invalid ";
        if( 0 < long_name.length() ) std::cout << "option \"--" << long_name << "\"";
        else if( ' ' != short_name ) std::cout << "option \"-" << short_name << "\"";
        else std::cout << "argument \"" << arg << "\"";
        std::cout << std::endl;
        break;
      }
    }

    // make sure there are no left over options
    if( !invalid )
    {
      if( 0 < long_name.length() )
      {
        std::cout << "ERROR: expecting a value after \"--" << long_name << "\"" << std::endl;
        invalid = true;
      }
      else if( ' ' != short_name )
      {
        std::cout << "ERROR: expecting a value after \"-" << short_name << "\"" << std::endl;
        invalid = true;
      }

      // only process the input arguments if the help switch is off
      if( !this->find_flag( "help" )->value )
      {
        // now make sure we have the correct number of inputs
        if( input_list.size() != this->input_map.size() )
        {
          std::cout << "ERROR: Wrong number of free (input) arguments " << std::endl;
          invalid = true;
        }
        else
        {
          // populate the input map
          auto list_it = input_list.cbegin();
          auto map_it = this->input_map.rbegin();

          for( ; list_it != input_list.cend() && map_it != this->input_map.rend(); ++list_it, ++map_it )
            map_it->second = (*list_it);
        }
      }
    }

    if( invalid )
    {
      std::cout << std::endl;
      this->print_usage();
    }

    return !invalid;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool options::process_config_file( const std::string filename )
  {
    bool invalid = false;
    std::string line, key, value;
    flag* flag;
    option* option;

    std::ifstream file( filename, std::ifstream::in );
    while( !file.eof() )
    {
      std::getline( file, line );
      
      // anything after a # is ignored, option and value is deliminated by :
      std::vector< std::string > parts = utilities::explode( line.substr( 0, line.find( '#' ) ), ":" );

      if( 1 == parts.size() )
      { // flag
        key = utilities::trim( parts[0] );

        flag = NULL;
        if( 1 == key.length() ) flag = this->find_flag( key[0] );
        else flag = this->find_flag( key );

        if( NULL != flag ) flag->value = true;
        else invalid = true;
      }
      else if( 2 == parts.size() )
      { // option
        key = utilities::trim( parts[0] );
        value = utilities::trim( parts[1] );

        option = NULL;
        if( 1 == key.length() ) option = this->find_option( key[0] );
        else option = this->find_option( key );

        if( NULL != option ) option->value = value;
        else invalid = true;
      }
      else if( 2 < parts.size() ) invalid = true;

      if( invalid )
      {
        std::cout << "ERROR: Error parsing config file at \"" << line << "\"" << std::endl;
        break;
      }
    }

    file.close();

    return !invalid;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void options::print_usage()
  {
    if( !this->usage_printed )
    {
      std::cout << this->get_usage();
      this->usage_printed = true;
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string options::get_usage() const
  {
    // start by printing the command line
    std::stringstream stream;
    stream << "Usage: " << this->executable_name << " [options...]";
    for( auto input_it = this->input_map.crbegin(); input_it != this->input_map.crend(); ++input_it )
      stream << " <" << input_it->first << ">";
    stream << std::endl << std::endl;

    // then the options/flags/comments
    for( auto usage_it = this->usage_list.cbegin(); usage_it != this->usage_list.cend(); ++usage_it )
      stream << (*usage_it) << std::endl;
    return stream.str();
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string options::get_input( const std::string name ) const
  {
    auto it = this->input_map.find( name );
    if( this->input_map.cend() == it )
    {
      std::stringstream stream;
      stream << "Tried to get input \"" << name << "\" which doesn't exist";
      throw std::runtime_error( stream.str() );
    }

    return it->second;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool options::get_flag( const char short_name, const std::string long_name ) const
  {
    for( auto it = this->flag_list.cbegin(); it != this->flag_list.cend(); ++it )
      if( ( 0 < long_name.length() && it->long_name == long_name ) ||
          ( ' ' != short_name && it->short_name == short_name ) ) return it->value;

    // if we got here then the flag isn't in the flag list
    std::stringstream stream;
    stream << "Tried to get value of invalid flag \"";
    if( 0 < long_name.length() ) stream << long_name;
    else stream << short_name;
    stream << "\"";
    throw std::runtime_error( stream.str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string options::get_option( const char short_name, const std::string long_name ) const
  {
    for( auto it = this->option_list.cbegin(); it != this->option_list.cend(); ++it )
      if( ( 0 < long_name.length() && it->long_name == long_name ) ||
          ( ' ' != short_name && it->short_name == short_name ) ) return it->get_value();

    // if we got here then the option isn't in the option list
    std::stringstream stream;
    stream << "Tried to get value of invalid option \"";
    if( 0 < long_name.length() ) stream << long_name;
    else stream << short_name;
    stream << "\"";
    throw std::runtime_error( stream.str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  options::flag* options::find_flag( const char short_name )
  {
    for( auto flag_it = this->flag_list.begin(); flag_it != this->flag_list.end(); ++flag_it )
      if( flag_it->has_short_name() && short_name == flag_it->short_name ) return &( *flag_it );
    return NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  options::flag* options::find_flag( const std::string long_name )
  {
    for( auto flag_it = this->flag_list.begin(); flag_it != this->flag_list.end(); ++flag_it )
      if( flag_it->has_long_name() && long_name == flag_it->long_name ) return &( *flag_it );
    return NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  options::option* options::find_option( const char short_name )
  {
    for( auto option_it = this->option_list.begin(); option_it != this->option_list.end(); ++option_it )
      if( option_it->has_short_name() && short_name == option_it->short_name ) return &( *option_it );
    return NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  options::option* options::find_option( const std::string long_name )
  {
    for( auto option_it = this->option_list.begin(); option_it != this->option_list.end(); ++option_it )
      if( option_it->has_long_name() && long_name == option_it->long_name ) return &( *option_it );
    return NULL;
  }
}
