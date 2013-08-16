/*=========================================================================

  Program:  sampsim
  Module:   arc_epi_sample.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which samples a population using the arc EPI method
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

  // define inputs
  opts.add_input( "population_file" );
  opts.add_input( "output_file" );

  // define general parameters
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV files instead of JSON data" );
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
