/*=========================================================================

  Program:  sampsim
  Module:   circle_gps_sample_setup.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Functions used to set up and parse options for a circle_gps sample
//

#include "options.h"
#include "sample/circle_gps.h"
#include "utilities.h"

void setup_circle_gps_sample( sampsim::options &opts )
{
  opts.add_option(
    "radius", "500", "What distance from a GPS point to include when searching for households (in meters)" );
}

void parse_circle_gps_sample( sampsim::options &opts, sampsim::sample::circle_gps *sample )
{
  sample->set_radius( opts.get_option_as_double( "radius" ) / 1000 );
}
