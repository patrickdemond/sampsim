/*=========================================================================

  Program:  sampsim
  Module:   strip_epi_sample.h
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
  opts.add_option( "use_quadrants", "false",
    "Divide towns into quadrants, splitting the sample evently into each quadrant." );
  opts.add_option( "skip", "1",
    "How many of the nearest households to skip when selecting the next household." );
  opts.add_flag( "periphery",
    "Whether to select half of households at the centre and the other half on the periphery." );
  opts.add_option( "strip_width", "50", "Width of the strip used to sampling from the centre (in meters)" );
}

void parse_strip_epi_sample( sampsim::options &opts, sampsim::sample::strip_epi *sample )
{
  sample->set_use_quadrants( opts.get_flag( "use_quadrants" ) );
  sample->set_skip( opts.get_option_as_int( "skip" ) );
  sample->set_periphery( opts.get_flag( "periphery" ) );
  sample->set_strip_width( opts.get_option_as_double( "strip_width" ) / 1000 );
}
