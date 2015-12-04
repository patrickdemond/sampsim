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
  opts.add_option( "number_of_sectors", "1",
    "Divide towns into sectors, splitting the sample evently into each sector." );
  opts.add_option( "skip", "1",
    "How many of the nearest households to skip when selecting the next household." );
  opts.add_option( "strip_width", "50", "Width of the strip used to sampling from the centre (in meters)" );
}

void parse_strip_epi_sample( sampsim::options &opts, sampsim::sample::strip_epi *sample )
{
  sample->set_number_of_sectors( opts.get_option_as_int( "number_of_sectors" ) );
  sample->set_skip( opts.get_option_as_int( "skip" ) );
  sample->set_strip_width( opts.get_option_as_double( "strip_width" ) / 1000 );
}
