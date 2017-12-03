/*=========================================================================

  Program:  sampsim
  Module:   arc_epi_sample.h
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
  opts.add_option( "number_of_sectors", "1",
    "Divide towns into sectors, splitting the sample evently into each sector." );
  opts.add_option( "skip", "1",
    "How many of the nearest households to skip when selecting the next household." );
  opts.add_flag( "periphery",
    "Whether to select half of households at the centre and the other half on the periphery." );
  opts.add_option( "arc_angle", "0.5", "Angle of arc when sampling a line from the centre (in degrees)" );
}

void parse_arc_epi_sample( sampsim::options &opts, sampsim::sample::arc_epi *sample )
{
  sample->set_number_of_sectors( opts.get_option_as_int( "number_of_sectors" ) );
  sample->set_skip( opts.get_option_as_int( "skip" ) );
  sample->set_periphery( opts.get_flag( "periphery" ) );
  sample->set_arc_angle( opts.get_option_as_double( "arc_angle" ) / 180 * M_PI );
}
