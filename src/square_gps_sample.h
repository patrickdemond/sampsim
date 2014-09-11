/*=========================================================================

  Program:  sampsim
  Module:   square_gps_sample_setup.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Functions used to set up and parse options for a square_gps sample
//

#include "options.h"
#include "sample/square_gps.h"
#include "utilities.h"

void setup_square_gps_sample( sampsim::options &opts )
{
  opts.add_heading( "" );
  opts.add_heading( "Sampling parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "age", "either", "Restricts sample by age (\"adult\", \"child\" or \"either\")" );
  opts.add_flag( "one_per_household", "Only sample one individual per household" );
  opts.add_option( "sex", "either", "Restricts sample by sex (\"male\", \"female\" or \"either\")" );
  opts.add_option( "samples", "1", "The number of times to sample the population" );
  opts.add_option( "size", "1000", "How many individuals to select in each sample" );
  opts.add_option( "number_of_squares", "64",
    "How many squares (per direction) to divide towns into" );
}

void parse_square_gps_sample( sampsim::options &opts, sampsim::sample::square_gps *sample )
{
  sample->set_age( sampsim::get_age_type( opts.get_option( "age" ) ) );
  sample->set_one_per_household( opts.get_flag( "one_per_household" ) );
  sample->set_sex( sampsim::get_sex_type( opts.get_option( "sex" ) ) );
  sample->set_number_of_samples( opts.get_option_as_int( "samples" ) );
  sample->set_size( opts.get_option_as_int( "size" ) );
  sample->set_number_of_squares( opts.get_option_as_int( "number_of_squares" ) );
}
