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
  opts.add_option( "number_of_squares", "64",
    "How many squares (per direction) to divide towns into" );
}

void parse_square_gps_sample( sampsim::options &opts, sampsim::sample::square_gps *sample )
{
  sample->set_number_of_squares( opts.get_option_as_int( "number_of_squares" ) );
}
