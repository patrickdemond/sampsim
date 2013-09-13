/*=========================================================================

  Program:  sampsim
  Module:   strip_epi_sample.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which samples a population using the strip EPI method
//

#include "strip_epi_sample.h"

#include <stdexcept>

// main function
int main( const int argc, const char** argv )
{
  int status = EXIT_FAILURE;
  std::string filename;
  sampsim::options opts( argv[0] );
  sampsim::sample::strip_epi *sample = new sampsim::sample::strip_epi;
  
  // define inputs
  opts.add_input( "population_file" );
  opts.add_input( "output_file" );
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV files instead of JSON data" );
  opts.add_flag( 'v', "verbose", "Be verbose when generating sample" );
  setup_strip_epi_sample( opts );
  opts.add_option( "seed", "", "Seed used by the random generator" );

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
        std::cout << "sampsim strip_epi_sample version " << sampsim::utilities::get_version() << std::endl;
        std::string filename = opts.get_input( "output_file" );
        sampsim::utilities::verbose = opts.get_flag( "verbose" );
        sample->set_seed( opts.get_option( "seed" ) );
        parse_strip_epi_sample( opts, sample );

        if( sample->set_population( opts.get_input( "population_file" ) ) )
        {
          sample->generate();
          sample->write( filename, opts.get_flag( "flat_file" ) );
        }
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
