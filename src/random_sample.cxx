/*=========================================================================

  Program:  sampsim
  Module:   random_sample.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which randomly samples a population
//

#include "options.h"
#include "sample/random.h"
#include "utilities.h"

#include <stdexcept>

// main function
int main( int argc, char** argv )
{
  int status = EXIT_FAILURE;
  std::string filename;
  sampsim::options opts( argv[0] );
  sampsim::sample::random *sample = new sampsim::sample::random;
  
  // define inputs
  opts.add_input( "population_file" );
  opts.add_input( "output_file" );

  // define general parameters
  opts.add_flag( 'f', "flat_file", "Whether to output data in two CSV files instead of JSON data" );
  opts.add_flag( 'v', "verbose", "Be verbose when generating sample" );
  opts.add_heading( "" );
  opts.add_heading( "Sampling parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "age", "either", "Restricts sample by age (\"adult\", \"child\" or \"either\")" );
  opts.add_flag( "one_per_household", "Only sample one individual per household" );
  opts.add_option( "sex", "either", "Restricts sample by sex (\"male\", \"female\" or \"either\")" );
  opts.add_option( "size", "1000", "The sample's size (in individuals)" );

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
        std::string filename = opts.get_input( "output_file" );
        sampsim::utilities::verbose = opts.get_flag( "verbose" );

        // launch application
        sample->set_seed( opts.get_option( "seed" ) );
        sample->set_size( opts.get_option_as_int( "size" ) );
        sample->set_age_type( sampsim::sample::get_age_type( opts.get_option( "age" ) ) );
        sample->set_sex_type( sampsim::sample::get_sex_type( opts.get_option( "sex" ) ) );
        sample->set_one_per_household( opts.get_flag( "one_per_household" ) );
        sample->generate();
        sample->write( filename, opts.get_flag( "flat_file" ) );
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