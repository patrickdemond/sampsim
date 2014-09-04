/*=========================================================================

  Program:  sampsim
  Module:   arc_epi_sample_setup.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Functions used to set up an parse options for an arc_epi sample
//

#include "options.h"
#include "sample/arc_epi.h"
#include "utilities.h"

void setup_arc_epi_sample( sampsim::options &opts )
{
  opts.add_heading( "" );
  opts.add_heading( "Sampling parameters (overrides config files):" );
  opts.add_heading( "" );
  opts.add_option( "age", "either", "Restricts sample by age (\"adult\", \"child\" or \"either\")" );
  opts.add_flag( "one_per_household", "Only sample one individual per household" );
  opts.add_option( "sex", "either", "Restricts sample by sex (\"male\", \"female\" or \"either\")" );
  opts.add_option( "samples", "1", "The number of times to sample the population" );
  opts.add_option( "skip", "1",
    "How many of the nearest households to skip when selecting the next household." );
  opts.add_option( "size", "1000", "How many individuals to select in each sample" );
  opts.add_option( "arc_angle", "0.5", "Angle of arc when sampling a line from the centre (in degrees)" );
}

void parse_arc_epi_sample( sampsim::options &opts, sampsim::sample::arc_epi *sample )
{
  sample->set_age( sampsim::get_age_type( opts.get_option( "age" ) ) );
  sample->set_one_per_household( opts.get_flag( "one_per_household" ) );
  sample->set_sex( sampsim::get_sex_type( opts.get_option( "sex" ) ) );
  sample->set_number_of_samples( opts.get_option_as_int( "samples" ) );
  sample->set_skip( opts.get_option_as_int( "skip" ) );
  sample->set_size( opts.get_option_as_int( "size" ) );
  sample->set_arc_angle( opts.get_option_as_double( "arc_angle" ) / 180 * M_PI );
}
