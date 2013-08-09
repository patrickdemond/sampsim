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
    this->min_remaining_arguments = 0;
    this->max_remaining_arguments = -1;

    std::stringstream stream;
    stream << "Usage: " << executable_name << " [options...] <file>";
    this->add_heading( stream.str() );
    this->add_heading( "" );
    this->add_option( 'c', "config", "", "A configuration file containing flags and options" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void options::add_heading( const std::string description )
  {
    this->usage_list.push_back( description );
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void options::add_flag(
    const char short_name,
    const std::string long_name,
    const std::string description )
  {
    flag f;
    f.short_name = short_name;
    f.long_name = long_name;
    f.description = description;
    this->flag_list.push_back( f );

    // now add the usage string for this flag
    std::stringstream stream;
    stream << " ";
    if( f.has_short_name() ) stream << "-" << f.short_name << "  ";
    if( f.has_long_name() ) stream << "--" << f.long_name;
    std::string spacing;
    spacing.assign( 25 - stream.str().length(), ' ' );
    stream << spacing << f.description;
    this->add_heading( stream.str() );
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void options::add_option(
    const char short_name,
    const std::string long_name,
    const std::string initial,
    const std::string description )
  {
    option o;
    o.short_name = short_name;
    o.long_name = long_name;
    o.initial = initial;
    o.description = description;
    this->option_list.push_back( o );

    // now add the usage string for this option
    std::stringstream stream;
    stream << " ";
    if( o.has_short_name() ) stream << "-" << o.short_name << "  ";
    if( o.has_long_name() ) stream << "--" << o.long_name;
    std::string spacing;
    spacing.assign( 25 - stream.str().length(), ' ' );
    stream << spacing << o.description;
    this->add_heading( stream.str() );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::vector< std::string > options::get_arguments() const
  {
    return this->remaining_argument_list;
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

    if( this->process_arguments() )
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
    }

    // command line arguments override config file arguments
    if( config_file_processed ) this->process_arguments();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool options::process_arguments()
  {
    bool invalid = false;
    char short_name = ' ';
    std::string long_name = "";
    std::string error = "";

    // reset the remaining argument list since it will get rebuilt
    this->remaining_argument_list.clear();

    std::vector< std::string >::const_iterator it;
    for( it = this->argument_list.cbegin(); it != this->argument_list.cend(); ++it )
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
          flag *f = this->find_flag( long_name );
          if( NULL != f )
          {
            f->value = true;
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
          flag *f = this->find_flag( short_name );
          if( NULL != f )
          {
            f->value = true;
            short_name = ' ';
          }
        }
      }
      else
      { // defining a value
        if( 0 < long_name.length() )
        { // expecting a value for an option identified by its long name
          option *o = this->find_option( long_name );
          if( NULL != o )
          {
            o->value = arg;
            long_name = "";
          }
          else invalid = true;
        }
        else if( ' ' != short_name )
        { // expecting a value for an option identified by its short name
          option *o = this->find_option( short_name );
          if( NULL != o )
          {
            o->value = arg;
            short_name = ' ';
          }
          else invalid = true;
        }
        else
        {
          this->remaining_argument_list.push_back( arg );
        }
      }

      if( invalid )
      {
        std::cout << "ERROR: Invalid argument \"" << arg << "\"" << std::endl;
        break;
      }
    }

    // now make sure we have the correct number of remaining arguments
    if( this->min_remaining_arguments > this->remaining_argument_list.size() ||
        ( 0 <= this->max_remaining_arguments && 
          this->max_remaining_arguments < this->remaining_argument_list.size() ) )
    {
      std::cout << "ERROR: Wrong number of free arguments " << std::endl;
      invalid = true;
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
    flag* f;
    option* o;

    std::ifstream file( filename, std::ifstream::in );
    while( !file.eof() )
    {
      std::getline( file, line );
      
      // anything after a # is ignored, option and value is deliminated by :
      std::vector< std::string > parts = utilities::explode( line.substr( 0, line.find( '#' ) ), ":" );

      if( 1 == parts.size() )
      { // flag
        key = utilities::trim( parts[0] );

        f = NULL;
        if( 1 == key.length() ) f = this->find_flag( key[0] );
        else f = this->find_flag( key );

        if( NULL != f ) f->value = true;
        else invalid = true;
      }
      else if( 2 == parts.size() )
      { // option
        key = utilities::trim( parts[0] );
        value = utilities::trim( parts[1] );

        o = NULL;
        if( 1 == key.length() ) o = this->find_option( key[0] );
        else o = this->find_option( key );

        if( NULL != o ) o->value = value;
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
    std::stringstream stream;
    std::vector< std::string >::const_iterator it;
    for( it = this->usage_list.cbegin(); it != this->usage_list.cend(); ++it ) stream << (*it) << std::endl;
    return stream.str();
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  bool options::get_flag( const char short_name, const std::string long_name ) const
  {
    std::vector< flag >::const_iterator it;
    for( it = this->flag_list.cbegin(); it != this->flag_list.cend(); ++it )
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
    std::vector< option >::const_iterator it;
    for( it = this->option_list.cbegin(); it != this->option_list.cend(); ++it )
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
    std::vector< flag >::iterator flag_it;
    for( flag_it = this->flag_list.begin(); flag_it != this->flag_list.end(); ++flag_it )
      if( flag_it->has_short_name() && short_name == flag_it->short_name ) return &( *flag_it );
    return NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  options::flag* options::find_flag( const std::string long_name )
  {
    std::vector< flag >::iterator flag_it;
    for( flag_it = this->flag_list.begin(); flag_it != this->flag_list.end(); ++flag_it )
      if( flag_it->has_long_name() && long_name == flag_it->long_name ) return &( *flag_it );
    return NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  options::option* options::find_option( const char short_name )
  {
    std::vector< option >::iterator option_it;
    for( option_it = this->option_list.begin(); option_it != this->option_list.end(); ++option_it )
      if( option_it->has_short_name() && short_name == option_it->short_name ) return &( *option_it );
    return NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  options::option* options::find_option( const std::string long_name )
  {
    std::vector< option >::iterator option_it;
    for( option_it = this->option_list.begin(); option_it != this->option_list.end(); ++option_it )
      if( option_it->has_long_name() && long_name == option_it->long_name ) return &( *option_it );
    return NULL;
  }
}
