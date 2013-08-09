/*=========================================================================

  Program:  sampsim
  Module:   strip_epi_sample.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which samples a population using the strip EPI method
//

#include "options.h"
#include "utilities.h"

#include <stdexcept>

// main function
int main( int argc, char** argv )
{
  int status = EXIT_FAILURE;
  std::string filename;
  sampsim::options opts( argv[0] );
  opts.set_min_remaining_arguments( 1 );
  opts.set_max_remaining_arguments( 1 );

  // define general parameters
  std::string usage = "Usage: ";
  usage += argv[0];
  usage += " [options...] <file>";
  opts.add_heading( usage );
  opts.add_heading( "" );
  opts.add_option( 'c', "config", "", "A configuration file containing sampling parameters" );
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV files instead of JSON data" );
  opts.add_flag( 'h', "help", "Prints this help" );
  opts.add_flag( 'v', "verbose", "Be verbose when generating sample" );

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
        // launch application
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
