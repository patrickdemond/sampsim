/*=========================================================================

  Program:  sampsim
  Module:   strip_epi_sample_setup.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Functions used to set up an parse options for an strip_epi sample
//

#include "options.h"
#include "sample/strip_epi.h"
#include "utilities.h"

void setup_strip_epi_sample( sampsim::options &opts )
{
  opts.add_heading( "" );
  opts.add_heading( "Sampling parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "age", "either", "Restricts sample by age (\"adult\", \"child\" or \"either\")" );
  opts.add_flag( "one_per_household", "Only sample one individual per household" );
  opts.add_option( "sex", "either", "Restricts sample by sex (\"male\", \"female\" or \"either\")" );
  opts.add_option( "size", "1000", "The sample's size (in individuals)" );
  opts.add_option( "strip_width", "50", "Width of the strip used to sampling from the centre (in meters)" );
}

void parse_strip_epi_sample( sampsim::options &opts, sampsim::sample::strip_epi *sample )
{
  sample->set_age( sampsim::get_age_type( opts.get_option( "age" ) ) );
  sample->set_one_per_household( opts.get_flag( "one_per_household" ) );
  sample->set_sex( sampsim::get_sex_type( opts.get_option( "sex" ) ) );
  sample->set_size( opts.get_option_as_int( "size" ) );
  sample->set_strip_width( opts.get_option_as_double( "strip_width" ) / 1000 );
}
