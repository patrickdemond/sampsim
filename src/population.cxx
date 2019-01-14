/*=========================================================================

  Program:  sampsim
  Module:   population.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which processes populations in various ways
//

#include "options.h"
#include "population.h"
#include "sample/arc_epi.h"
#include "sample/circle_gps.h"
#include "sample/enumeration.h"
#include "sample/grid_epi.h"
#include "sample/random.h"
#include "sample/square_gps.h"
#include "sample/strip_epi.h"
#include "utilities.h"

#include <stdexcept>

// main function
int main( const int argc, const char** argv )
{
  int status = EXIT_FAILURE;
  sampsim::options opts( argv[0] );

  // define inputs
  opts.add_input( "input_file" );
  opts.add_option(
    't', "type", "population", "Identifies the input file's data type (population, arc_epi, circle_gps, etc)" );
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV \"flat\" files" );
  opts.add_flag( 's', "summary_file", "Whether to output summary data of the population" );
  opts.add_flag( 'a', "variance_file", "Whether to output variance data of a sample" );
  opts.add_flag( 'q', "quiet", "Do not generate any output" );

  try
  {
    // parse the command line arguments
    opts.set_arguments( argc, argv );
    if( opts.process() )
    {
      // now either show the help or run the application
      if( opts.get_flag( "help" ) )
      {
        opts.print_usage();
      }
      else
      {
        std::string input_filename = opts.get_input( "input_file" );
        std::string type = opts.get_option( "type" );
        bool flat_file = opts.get_flag( "flat_file" );
        bool summary_file = opts.get_flag( "summary_file" );
        bool variance_file = opts.get_flag( "variance_file" );
        sampsim::utilities::quiet = opts.get_flag( "quiet" );

        // determine what to do with the input file based on its extention(s)
        std::vector< std::string > parts = sampsim::utilities::explode( input_filename, "." );
        if( 4 > parts.size() ||
            "gz" != parts.back() ||
            "tar" != parts.at( parts.size()-2 ) ||
            "json" != parts.at( parts.size()-3 ) )
        {
          std::stringstream stream;
          stream << "Cannot read file \"" << input_filename << "\", only .json.tar.gz files can be read";
          throw std::runtime_error( stream.str() );
        }

        std::string base_name = input_filename.substr( 0, input_filename.size() - 12 );

        if( "population" == type )
        {
          sampsim::population *pop = new sampsim::population;
          pop->read( input_filename );
          if( flat_file ) pop->write( base_name, true );
          if( summary_file ) pop->write_summary( base_name );
          sampsim::utilities::safe_delete( pop );
        }
        else if( "arc_epi" == type )
        {
          sampsim::sample::arc_epi *sample = new sampsim::sample::arc_epi;
          sample->read( input_filename );
          if( flat_file ) sample->write( base_name, true );
          if( summary_file ) sample->write_summary( base_name );
          if( variance_file ) sample->write_variance( base_name );
          sampsim::utilities::safe_delete( sample );
        }
        else if( "circle_gps" == type )
        {
          sampsim::sample::circle_gps *sample = new sampsim::sample::circle_gps;
          sample->read( input_filename );
          if( flat_file ) sample->write( base_name, true );
          if( summary_file ) sample->write_summary( base_name );
          if( variance_file ) sample->write_variance( base_name );
          sampsim::utilities::safe_delete( sample );
        }
        else if( "enumeration" == type )
        {
          sampsim::sample::enumeration *sample = new sampsim::sample::enumeration;
          sample->read( input_filename );
          if( flat_file ) sample->write( base_name, true );
          if( summary_file ) sample->write_summary( base_name );
          if( variance_file ) sample->write_variance( base_name );
          sampsim::utilities::safe_delete( sample );
        }
        else if( "grid_epi" == type )
        {
          sampsim::sample::grid_epi *sample = new sampsim::sample::grid_epi;
          sample->read( input_filename );
          if( flat_file ) sample->write( base_name, true );
          if( summary_file ) sample->write_summary( base_name );
          if( variance_file ) sample->write_variance( base_name );
          sampsim::utilities::safe_delete( sample );
        }
        else if( "random" == type )
        {
          sampsim::sample::random *sample = new sampsim::sample::random;
          sample->read( input_filename );
          if( flat_file ) sample->write( base_name, true );
          if( summary_file ) sample->write_summary( base_name );
          if( variance_file ) sample->write_variance( base_name );
          sampsim::utilities::safe_delete( sample );
        }
        else if( "square_gps" == type )
        {
          sampsim::sample::square_gps *sample = new sampsim::sample::square_gps;
          sample->read( input_filename );
          if( flat_file ) sample->write( base_name, true );
          if( summary_file ) sample->write_summary( base_name );
          if( variance_file ) sample->write_variance( base_name );
          sampsim::utilities::safe_delete( sample );
        }
        else if( "strip_epi" == type )
        {
          sampsim::sample::strip_epi *sample = new sampsim::sample::strip_epi;
          sample->read( input_filename );
          if( flat_file ) sample->write( base_name, true );
          if( summary_file ) sample->write_summary( base_name );
          if( variance_file ) sample->write_variance( base_name );
          sampsim::utilities::safe_delete( sample );
        }
        else
        {
          std::stringstream stream;
          stream << "Unknown file type \"" << type << "\", must be one of "
                 << "\"population\", \"arc_epi\", \"circle_gps\", \"grid_epi\", \"random\", \"sqaure_gps\" or \"strip_epi\"";
          throw std::runtime_error( stream.str() );
        }
      }

      status = EXIT_SUCCESS;
    }
  }
  catch( std::exception &e )
  {
    std::cerr << "Uncaught exception: " << e.what() << std::endl;
  }

  return status;
}
