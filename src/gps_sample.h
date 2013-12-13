/*=========================================================================

  Program:  sampsim
  Module:   gps_sample_setup.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Functions used to set up and parse options for a gps sample
//

#include "options.h"
#include "sample/gps.h"
#include "utilities.h"

void setup_gps_sample( sampsim::options &opts )
{
  opts.add_heading( "" );
  opts.add_heading( "Sampling parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "age", "either", "Restricts sample by age (\"adult\", \"child\" or \"either\")" );
  opts.add_flag( "one_per_household", "Only sample one individual per household" );
  opts.add_option( "sex", "either", "Restricts sample by sex (\"male\", \"female\" or \"either\")" );
  opts.add_option( "size", "1000", "The sample's size (in individuals)" );
}

void parse_gps_sample( sampsim::options &opts, sampsim::sample::gps *sample )
{
  sample->set_age( sampsim::get_age_type( opts.get_option( "age" ) ) );
  sample->set_one_per_household( opts.get_flag( "one_per_household" ) );
  sample->set_sex( sampsim::get_sex_type( opts.get_option( "sex" ) ) );
  sample->set_size( opts.get_option_as_int( "size" ) );
}
