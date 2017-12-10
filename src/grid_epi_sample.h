/*=========================================================================

  Program:  sampsim
  Module:   grid_epi_sample.h
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Functions used to set up an parse options for an grid_epi sample
//

#include "options.h"
#include "sample/grid_epi.h"
#include "utilities.h"

void setup_grid_epi_sample( sampsim::options &opts )
{
  opts.add_option( "skip", "1",
    "How many of the nearest households to skip when selecting the next household." );
  opts.add_option( "number_of_squares", "30", "The number of squares (per direction) to divide towns into." );
}

void parse_grid_epi_sample( sampsim::options &opts, sampsim::sample::grid_epi *sample )
{
  sample->set_skip( opts.get_option_as_int( "skip" ) );
  sample->set_number_of_squares( opts.get_option_as_int( "number_of_squares" ) );
}
