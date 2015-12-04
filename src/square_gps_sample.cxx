/*=========================================================================

  Program:  sampsim
  Module:   square_gps_sample.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which samples a population using the strip EPI method
//

#include "square_gps_sample.h"

#include "common.h"

#include <stdexcept>

// main function
int main( const int argc, const char** argv )
{
  int status = EXIT_FAILURE;
  sampsim::options opts( argv[0] );
  setup_sample( opts );
  setup_square_gps_sample( opts );
  sampsim::sample::square_gps *sample = new sampsim::sample::square_gps;

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
        parse_square_gps_sample( opts, sample );
        process_sample( opts, sample );
      }

      status = EXIT_SUCCESS;
    }
  }
  catch( std::exception &e )
  {
    std::cerr << "Uncaught exception: " << e.what() << std::endl;
  }

  sampsim::utilities::safe_delete( sample );
  return status;
}
